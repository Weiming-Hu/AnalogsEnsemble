/* 
 * File:   gribConverterFunctions.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on September 17, 2018, 5:20 PM
 */

/** @file */

#include <stdio.h>
#include <stdlib.h>
#include "gribConverterFunctions.h"

#if defined(_CODE_PROFILING)
#include <ctime>
#endif

// http://www.wmo.int/pages/prog/www/WMOCodes/WMO306_vI2/LatestVERSION/WMO306_vI2_GRIB2_CodeFlag_en.pdf
// http://www.cosmo-model.org/content/model/documentation/grib/pdtemplate_4.11.htm

namespace gribConverter {

    void MY_CODES_CHECK(int ret, int caller) {
        if (ret != CODES_SUCCESS) {
            const char* p_str = codes_get_error_message(ret);
            throw runtime_error(p_str);
        }
    }

    void getXY(vector<double> & xs, vector<double> & ys, string file, long par_id,
            long level, string par_key, string level_key, string val_key) {

        int ret;
        double *p_vals;
        size_t len;
        codes_index* index;
        codes_handle* h;

        // Turn off support for multi fields messages
        codes_grib_multi_support_off(0);

        // Construct query string
        string query_str(par_key);
        query_str.append(",");
        query_str.append(level_key);

        // Send query request
        index = codes_index_new_from_file(0, 
                const_cast<char*>(file.c_str()), query_str.c_str(), &ret);

        if (ret) throw runtime_error(codes_get_error_message(ret));
        // CODES_CHECK(ret, 0);

        // Select index based on par_key and level_key
        MY_CODES_CHECK(codes_index_select_long(index, par_key.c_str(), par_id), 0);
        MY_CODES_CHECK(codes_index_select_long(index, level_key.c_str(), level), 0);

        // Get data size
        h = codes_handle_new_from_index(index, &ret);
        MY_CODES_CHECK(codes_get_size(h, val_key.c_str(), &len), 0);

        // Get data values
        p_vals = (double*) malloc(len * sizeof (double));

        // Copy lat/lon to vector
        xs.clear();
        ys.clear();
        xs.resize(len);
        ys.resize(len);
        MY_CODES_CHECK(codes_grib_get_data(h, ys.data(), xs.data(), p_vals), 0);

        // Housekeeping
        free(p_vals);
        MY_CODES_CHECK(codes_handle_delete(h), 0);
        codes_index_delete(index);

        return;
    }

    void getDoubles(vector<double> & vals, const string & file, long par_id, long level,
            const string & type, const string & par_key, const string & level_key,
            const string & type_key, const string & val_key) {

#if defined(_CODE_PROFILING)
        clock_t time_start = clock();
#endif   
        
        int ret;
        size_t vals_len;
        codes_index* index = NULL;
        codes_handle* h = NULL;

        long par_id_h = 0, level_h = 0;
        char* type_tmp;
        size_t str_len;
        
        query_grib_eccodes_(
                file, par_id, level, type,
                par_key, level_key, type_key,
                &index, &ret);

        // Traversing all the messaging individually without using the index mechanism.
        FILE* in = fopen(file.c_str(), "r");

        while ((h = codes_handle_new_from_file(0, in, PRODUCT_GRIB, &ret)) != NULL ) {

            MY_CODES_CHECK(ret,0);

            MY_CODES_CHECK(codes_get_long(h, par_key.c_str(), &par_id_h),0);
            if (par_id_h != par_id) {
                codes_handle_delete(h);
                continue;
            }

            MY_CODES_CHECK(codes_get_long(h, level_key.c_str(), &level_h),0);
            if (level_h != level) {
                codes_handle_delete(h);
                continue;
            }

            MY_CODES_CHECK(codes_get_length(h, type_key.c_str(), &str_len), 0);
            type_tmp = (char*)malloc(str_len*sizeof(char));
            MY_CODES_CHECK(codes_get_string(h, type_key.c_str(), type_tmp, &str_len),0);
            if (type != string(type_tmp)) {
                codes_handle_delete(h);
                free(type_tmp);
                continue;
            }

            MY_CODES_CHECK(ret, 0);
            MY_CODES_CHECK(codes_get_size(h, val_key.c_str(), &vals_len), 0);

            // Copy data to vector
            vals.clear();
            vals.resize(vals_len);
            MY_CODES_CHECK(codes_get_double_array(h, val_key.c_str(), vals.data(), &vals_len), 0);

            // Housekeeping
            free(type_tmp);
            codes_index_delete(index);
            codes_handle_delete(h);
            fclose(in);
            
#if defined(_CODE_PROFILING)
            clock_t time_end = clock();
            double duration_full = (float) (time_end - time_start) / CLOCKS_PER_SEC;
            double duration_pre = (float) (time_pre - time_start) / CLOCKS_PER_SEC;
            double duration_while_1 = (float) (time_end - time_pre) / CLOCKS_PER_SEC;

            cout << endl << "CPU Time profiling for gribConverterFunctions::getDoubles:" << endl
                    << "Total time: " << duration_full << " seconds (100%)" << endl
                    << "Query time: " << duration_pre << " seconds (" << duration_pre / duration_full * 100 << "%)" << endl
                    << "Reading time (phase 1 + 2): " << duration_while_1 << " seconds (" << duration_while_1 / duration_full * 100 << "%)" << endl
                    << "-----------------------------------------------------" << endl;
#endif
            
            return;
        }

        fclose(in);
        codes_index_delete(index);
        if (h) MY_CODES_CHECK(codes_handle_delete(h), 0);

        stringstream ss;
        ss << BOLDRED << "Error: There is no message for "
            << par_key << " " << par_id << " " << level_key << " "
            << level << " " << type_key << " " << type << RESET << endl;
        throw runtime_error(ss.str());
    }

    void toForecasts(vector<string> & files_in, const string & file_out,
            const vector<long> & pars_id, const vector<string> & pars_new_name,
            const vector<long> & crcl_pars_id, const vector<long> & levels,
            const vector<string> & level_types,
            const string & par_key, const string & level_key,
            const string & type_key, const string & val_key,
            string regex_time_str, string regex_flt_str,
            double flt_interval, bool delimited, bool skip_data, int verbose) {

        if (verbose >= 3) cout << GREEN << "Convert GRIB2 files to Forecasts" << RESET << endl;

        // Create write io device early in the process to make sure the file does
        // not alreay exist.
        //
        AnEnIO io("Write", file_out, "Forecasts", verbose);

        // Read station xs and ys from the first available file from the list
        if (verbose >= 3) cout << GREEN << "Reading station information ... " << RESET << endl;
        anenSta::Stations stations;
        vector<double> xs, ys;

        for (size_t i_file = 0; i_file < files_in.size(); i_file++) {
            try {
                getXY(xs, ys, files_in[i_file], pars_id[i_file],
                        levels[i_file], par_key, level_key, val_key);
                break;
            } catch (exception & e) {
                cout << RED << "Skipping file ( " << files_in[i_file]
                    << " ): " << e.what() << RESET << endl;
            }
        }

        if (xs.size() != ys.size()) throw runtime_error("Error: the number of xs and ys do not match!");

        for (size_t i = 0; i < xs.size(); i++) {
            anenSta::Station station(xs[i], ys[i]);
            stations.push_back(station);
        }
        assert(stations.size() == xs.size());

        // Read parameters based on input options
        if (verbose >= 3) cout << GREEN << "Reading parameter information ... " << RESET << endl;
        anenPar::Parameters parameters;
        for (size_t i = 0; i < pars_new_name.size(); i++) {
            bool circular = false;
            if (find(crcl_pars_id.begin(), crcl_pars_id.end(), pars_id[i]) != crcl_pars_id.end()) {
                circular = true;
            }

            anenPar::Parameter parameter(pars_new_name[i], circular);
            parameters.push_back(parameter);
        }
        if (verbose >= 5) cout << parameters;
        assert(parameters.size() == pars_new_name.size());

        // Prepare times
        if (verbose >= 3) cout << GREEN << "Reading time and FLT information ... " << RESET << endl;
        anenTime::Times times;
        regex regex_time;
        smatch match_time, match_flt;

        try {
            regex_time = regex(regex_time_str);
        } catch (const regex_error& e) {
            cerr << BOLDRED << "Error: Can't use the regular expression " << regex_time_str << RESET << endl;
            throw;
        }

        // Prepare flts
        anenTime::FLTs flts;
        regex regex_flt;

        try {
            regex_flt = regex(regex_flt_str);
        } catch (const regex_error& e) {
            cerr << BOLDRED << "Error: Can't use the regular expression " << regex_flt_str << RESET << endl;
            throw;
        }

        // Read times and flts
        // Within this loop, it is also checking whether each file is valid
        //
        vector<double> times_vec, flts_vec;
        boost::gregorian::date time_start{anenTime::_ORIGIN_YEAR,
            anenTime::_ORIGIN_MONTH, anenTime::_ORIGIN_DAY}, time_end;
        vector<string> files_in_selected;

        for (const auto & file : files_in) {
            if (regex_search(file.begin(), file.end(), match_time, regex_time) &&
                    regex_search(file.begin(), file.end(), match_flt, regex_flt)) {

                // Assumption
                // - the first 4 digits to be the year
                // - the next 2 digits to be the month
                // - the next 2 digits to be the day
                //
                if (delimited) {
                    time_end = boost::gregorian::from_string(string(match_time[1]));
                } else {
                    time_end = boost::gregorian::from_undelimited_string(string(match_time[1]));
                }
                boost::gregorian::date_duration duration = time_end - time_start;

                times_vec.push_back(duration.days() * _SECS_IN_DAY);
                flts_vec.push_back(stoi(match_flt[1]) * flt_interval);
                files_in_selected.push_back(file);
            }
        }

        swap(files_in, files_in_selected);
        if (verbose >= 5) {
            cout << "Input files (computed internally): " << files_in << endl;
        }

        sort(flts_vec.begin(), flts_vec.end());
        flts.insert(flts.end(), flts_vec.begin(), flts_vec.end());

        sort(times_vec.begin(), times_vec.end());
        times.insert(times.end(), times_vec.begin(), times_vec.end());

        if (verbose >= 5) {
            cout << times << flts;
        }
        
        // Allocating memory for data
        if (verbose >= 3) {
            cout << GREEN << "Allocating memory for double array [" << parameters.size()
                    << "][" << stations.size() << "][" << times.size() << "]["
                    << flts.size() << "] ... " << RESET << endl;
        }
        Forecasts_array forecasts(parameters, stations, times, flts);
        
        if (skip_data) {
            // If data will not be written to the file
            if (verbose >= 3) cout << GREEN << "Do not write data values ... " << RESET << endl;
            
        } else {
            // Read data
            auto & data = forecasts.data();

            // This is created to keep track of the return condition for each file
            vector<bool> file_flags(files_in.size(), true);

            if (verbose >= 3) cout << GREEN << "Reading data ... " << RESET << endl;

            for (size_t i = 0; i < files_in.size(); i++) {

                // Create a const reference to the file list to avoid modification.
                // Regex function requires a const string otherwise it gives error.
                //
                const string & file_in = files_in[i];

                if (verbose >= 3) {
                    cout << "\t reading data from file " << file_in << endl;
                }

                // Find out flt index
                if (!regex_search(file_in.begin(), file_in.end(), match_flt, regex_flt))
                    throw runtime_error("Error: Counld not find flt index in flts.");
                size_t index_flt = flts.getTimeIndex(stoi(match_flt[1]) * flt_interval);

                // Find out time index
                if (!regex_search(file_in.begin(), file_in.end(), match_time, regex_time))
                    throw runtime_error("Error: Could not find time index in times.");

                if (delimited) {
                    time_end = boost::gregorian::from_string(string(match_time[1]));
                } else {
                    time_end = boost::gregorian::from_undelimited_string(string(match_time[1]));
                }
                boost::gregorian::date_duration duration = time_end - time_start;
                size_t index_time = times.getTimeIndex(duration.days() * _SECS_IN_DAY);

                vector<double> data_vec;
                for (size_t j = 0; j < pars_id.size(); j++) {

                    if (file_flags[i]) {
                        try {
                            getDoubles(data_vec, file_in, pars_id[j], levels[j],
                                    level_types[j], par_key, level_key, type_key, val_key);
                        } catch (const exception & e) {
                            cerr << BOLDRED << "Error when reading " << pars_id[j] << " "
                                    << levels[j] << " " << level_types[j] << " from file " << file_in
                                    << ": " << e.what() << RESET << endl;
                            continue;
                        } catch (...) {
                            cerr << BOLDRED << "Unknown error when reading " << pars_id[j] << " "
                                    << levels[j] << " " << level_types[j] << " from file " << file_in
                                    << RESET << endl;
                            continue;
                        }

                        if (data_vec.size() == forecasts.getStations().size()) {

                            for (size_t k = 0; k < data_vec.size(); k++) {
                                data[j][k][index_time][index_flt] = data_vec[k];
                            } // End of loop for stations

                        } else {
                            file_flags[i] = false;
                        }
                    }

                } // End of loop for parameters
            } // End of loop for files

            for (size_t i = 0; i < file_flags.size(); i++) {
                if (!file_flags[i]) cerr << BOLDRED << "Error: The " << i + 1
                        << "th file in the input file lists caused a problem while reading data!"
                        << RESET << endl;
            }
        }

        // Write forecasts
        if (verbose >= 3) cout << GREEN << "Writing Forecasts file ... " << RESET << endl;
        handleError(io.writeForecasts(forecasts));

        if (verbose >= 3) cout << GREEN << "Done!" << RESET << endl;
        return;
    }

    void toObservations(vector<string> & files_in, const string & file_out,
            const vector<long> & pars_id, const vector<string> & pars_new_name,
            const vector<long> & crcl_pars_id, const vector<long> & levels,
            const vector<string> & level_types,
            const string & par_key, const string & level_key,
            const string & type_key, const string & val_key,
            string regex_time_str, bool delimited, bool skip_data, int verbose) {

        if (verbose >= 3) cout << GREEN << "Convert GRIB2 files to Observations" << RESET << endl;

        // Create write io device early in the process to make sure the file does
        // not alreay exist.
        //
        AnEnIO io("Write", file_out, "Observations", verbose);

        // Read station xs and ys based on the first parameter in the list
        if (verbose >= 3) cout << GREEN << "Reading station information ... " << RESET << endl;
        anenSta::Stations stations;
        vector<double> xs, ys;

        for (size_t i_file = 0; i_file < files_in.size(); i_file++) {
            try {
                getXY(xs, ys, files_in[i_file], pars_id[i_file],
                        levels[i_file], par_key, level_key, val_key);
                break;
            } catch (exception & e) {
                cout << RED << "Skipping file ( " << files_in[i_file]
                    << " ): " << e.what() << RESET << endl;
            }
        }

        if (xs.size() != ys.size()) throw runtime_error("Error: the number of xs and ys do not match!");

        for (size_t i = 0; i < xs.size(); i++) {
            anenSta::Station station(xs[i], ys[i]);
            stations.push_back(station);
        }
        assert(stations.size() == xs.size());

        // Read parameters based on input options
        if (verbose >= 3) cout << GREEN << "Reading parameter information ... " << RESET << endl;
        anenPar::Parameters parameters;
        for (size_t i = 0; i < pars_new_name.size(); i++) {
            bool circular = false;
            if (find(crcl_pars_id.begin(), crcl_pars_id.end(), pars_id[i]) != crcl_pars_id.end()) {
                circular = true;
            }

            anenPar::Parameter parameter(pars_new_name[i], circular);
            parameters.push_back(parameter);
        }
        if (verbose >= 5) cout << parameters;
        assert(parameters.size() == pars_new_name.size());

        // Prepare times
        if (verbose >= 3) cout << GREEN << "Reading time information ... " << RESET << endl;
        anenTime::Times times;
        regex regex_time;
        smatch match;

        try {
            regex_time = regex(regex_time_str);
        } catch (const regex_error& e) {
            cerr << BOLDRED << "Error: Can't use the regular expression " << regex_time_str << RESET << endl;
            throw;
        }

        // Read times
        // Within this loop, it is also checking whether each file is valid
        //
        vector<double> times_vec;
        boost::gregorian::date time_start{anenTime::_ORIGIN_YEAR,
            anenTime::_ORIGIN_MONTH, anenTime::_ORIGIN_DAY}, time_end;
        vector<string> files_in_selected;

        for (const auto & file : files_in) {
            if (regex_search(file.begin(), file.end(), match, regex_time)) {

                // Assumption
                // - the first 4 digits to be the year
                // - the next 2 digits to be the month
                // - the next 2 digits to be the day
                //
                if (delimited) {
                    time_end = boost::gregorian::from_string(string(match[1]));
                } else {
                    time_end = boost::gregorian::from_undelimited_string(string(match[1]));
                }
                boost::gregorian::date_duration duration = time_end - time_start;

                times_vec.push_back(duration.days() * _SECS_IN_DAY);
                files_in_selected.push_back(file);
            }
        }

        swap(files_in, files_in_selected);
        if (verbose >= 5) {
            cout << "Input files (computed internally): " << files_in << endl;
        }

        sort(times_vec.begin(), times_vec.end());
        times.insert(times.end(), times_vec.begin(), times_vec.end());

        if (verbose >= 5) cout << times;
        
        // Allocating memory for data        
        if (verbose >= 3) {
            cout << GREEN << "Allocating memory for double array [" << parameters.size()
                    << "][" << stations.size() << "][" << times.size() << "] ... "
                    << RESET << endl;
        }
        Observations_array observations(parameters, stations, times);
        
        if (skip_data) {
            // If data will not be written to the file
            if (verbose >= 3) cout << GREEN << "Do not write data values ... " << RESET << endl;
            
        } else {
            // Read data
            if (verbose >= 3) cout << GREEN << "Reading data information ..." << RESET << endl;
            auto & data = observations.data();

            // This is created to keep track of the return condition for each file
            vector<bool> file_flags(files_in.size(), true);

            for (size_t i = 0; i < files_in.size(); i++) {

                // Create a const reference to the file list to avoid modification.
                // Regex function requires a const string otherwise it gives error.
                //
                const string & file_in = files_in[i];

                if (verbose >= 3) {
                    cout << "\t reading data from file " << file_in << endl;
                }

                // Find out time index
                if (delimited) {
                    time_end = boost::gregorian::from_string(string(match[1]));
                } else {
                    time_end = boost::gregorian::from_undelimited_string(string(match[1]));
                }
                boost::gregorian::date_duration duration = time_end - time_start;
                size_t index_time = times.getTimeIndex(duration.days() * _SECS_IN_DAY);

                for (size_t j = 0; j < pars_id.size(); j++) {
                    if (file_flags[i]) {
                        vector<double> data_vec;
                        try {
                            getDoubles(data_vec, file_in, pars_id[j], levels[j],
                                    level_types[j], par_key, level_key, type_key, val_key);
                        } catch (const exception & e) {
                            cerr << BOLDRED << "Error when reading " << pars_id[j] << " "
                                    << levels[j] << " " << level_types[j] << " from file " << file_in
                                    << ": " << e.what() << RESET << endl;
                            continue;
                        } catch (...) {
                            cerr << BOLDRED << "Unknown error when reading " << pars_id[j] << " "
                                    << levels[j] << " " << level_types[j] << " from file " << file_in
                                    << RESET << endl;
                            continue;
                        }

                        if (data_vec.size() == observations.getStations().size()) {

                            for (size_t k = 0; k < data_vec.size(); k++) {
                                data[j][k][index_time] = data_vec[k];
                            } // End of loop for stations

                        } else {
                            file_flags[i] = false;
                        }
                    }

                } // End of loop for parameters
            } // End of loop for files

            for (size_t i = 0; i < file_flags.size(); i++) {
                if (!file_flags[i]) cerr << BOLDRED << "Error: The " << i + 1
                        << "th file in the input file lists caused a problem while reading data!"
                        << RESET << endl;
            }
        }
        
        // Write observations
        if (verbose >= 3) cout << GREEN << "Writing Observations file ... " << RESET << endl;
        handleError(io.writeObservations(observations));

        if (verbose >= 3) cout << GREEN << "Done!" << RESET << endl;
        return;
    }

    void query_grib_eccodes_(
            string file, long par_id, long level, string type,
            string par_key, string level_key, string type_key,
            codes_index** p_index, int* p_ret) {

        codes_grib_multi_support_off(0);

        // First we use the fast way to index the message.

        // Construct query string
        string query_str(par_key);
        query_str.append(":l,");
        query_str.append(level_key);
        query_str.append(":l,");
        query_str.append(type_key);
        query_str.append(":s");

        // Send query request
        // The select string function requires a char* rather than const char*,
        // hence the following cast
        //
        *p_index = codes_index_new_from_file(0, const_cast<char*>(file.c_str()), query_str.c_str(), p_ret);
        MY_CODES_CHECK(*p_ret, 0);
        if (!*p_index) {
            throw runtime_error("Eccodes index is empty after creation.");
        }

        // Select index based on par_key and level_key, level_type
        MY_CODES_CHECK(codes_index_select_string(*p_index, type_key.c_str(),
                    const_cast<char*> (type.c_str())), 0);
        MY_CODES_CHECK(codes_index_select_long(*p_index, par_key.c_str(), par_id), 0);
        MY_CODES_CHECK(codes_index_select_long(*p_index, level_key.c_str(), level), 0);
        if (!*p_index) {
            throw runtime_error("Eccodes index is empty after query.");
        }
                
        return;
    }
}
