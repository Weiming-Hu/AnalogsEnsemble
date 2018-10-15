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

// http://www.wmo.int/pages/prog/www/WMOCodes/WMO306_vI2/LatestVERSION/WMO306_vI2_GRIB2_CodeFlag_en.pdf
// http://www.cosmo-model.org/content/model/documentation/grib/pdtemplate_4.11.htm

namespace gribConverter {

    void getXY(vector<double> & xs, vector<double> & ys, string file, long par_id,
            long level, string par_key, string level_key, string val_key) {

        int ret;
        double *p_xs, *p_ys, *p_vals;
        size_t len;
        codes_index* index;
        codes_handle* h;

        // Turn on support for multi fields messages
        codes_grib_multi_support_off(0);

        // Construct query string
        string query_str(par_key);
        query_str.append(",");
        query_str.append(level_key);

        // Send query request
        index = codes_index_new_from_file(0, 
                const_cast<char*>(file.c_str()), query_str.c_str(), &ret);
        CODES_CHECK(ret, 0);

        // Select index based on par_key and level_key
        CODES_CHECK(codes_index_select_long(index, par_key.c_str(), par_id), 0);
        CODES_CHECK(codes_index_select_long(index, level_key.c_str(), level), 0);

        // Get data size
        h = codes_handle_new_from_index(index, &ret);
        CODES_CHECK(codes_get_size(h, val_key.c_str(), &len), 0);

        // Get data values
        p_vals = (double*) malloc(len * sizeof (double));
        p_xs = (double*) malloc(len * sizeof (double));
        p_ys = (double*) malloc(len * sizeof (double));

        CODES_CHECK(codes_grib_get_data(h, p_ys, p_xs, p_vals), 0);

        // Copy lat/lon to vector
        xs.clear();
        ys.clear();
        xs.resize(len);
        ys.resize(len);
        copy(p_xs, p_xs + len, xs.begin());
        copy(p_ys, p_ys + len, ys.begin());

        // Housekeeping
        free(p_vals);
        free(p_xs);
        free(p_ys);
        CODES_CHECK(codes_handle_delete(h), 0);
        codes_index_delete(index);

        return;
    }

    void MY_CODES_CHECK(int ret, int caller) {
        if (ret != CODES_SUCCESS) {
            const char* p_str = codes_get_error_message(ret);
            throw runtime_error(p_str);
        }
    }

    void getDoubles(vector<double> & vals, string file, long par_id, long level,
            string type, string par_key, string level_key,
            string type_key, string val_key) {

        int ret;
        double* p_vals;
        size_t vals_len;
        codes_index* index = NULL;
        codes_handle* h = NULL;

        long par_id_h = 0, level_h = 0;
        string type_h;
        char* type_tmp;
        size_t str_len;

        // Turning multi field support on is dangerous because the indexing functions will
        // only find the message and the handle created from the message points to the first
        // field. For example, if a message has multiple fields, say U and V component, the 
        // message containing V can be found, but then handle created to read the message will
        // only point to the first field which is U. Please read on and see how I take care of
        // this situation.
        //
        codes_grib_multi_support_on(0);

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
        index = codes_index_new_from_file(0, const_cast<char*>(file.c_str()), query_str.c_str(), &ret);
        MY_CODES_CHECK(ret, 0);

        // Select index based on par_key and level_key, level_type
        MY_CODES_CHECK(codes_index_select_string(index, type_key.c_str(),
                    const_cast<char*> (type.c_str())), 0);
        MY_CODES_CHECK(codes_index_select_long(index, par_key.c_str(), par_id), 0);
        MY_CODES_CHECK(codes_index_select_long(index, level_key.c_str(), level), 0);

        while ((h = codes_handle_new_from_index(index, &ret)) != NULL ) {

            // At this point we should have a valid index that points to the message 
            // that we want to read. However, we should deal with the multi field
            // situation. We check whether the first field is what we want.
            //
            CODES_CHECK(codes_get_length(h, type_key.c_str(), &str_len), 0);
            type_tmp = (char*)malloc(str_len*sizeof(char));
            CODES_CHECK(codes_get_string(h, type_key.c_str(), type_tmp, &str_len),0);
            CODES_CHECK(codes_get_long(h, par_key.c_str(), &par_id_h),0);
            CODES_CHECK(codes_get_long(h, level_key.c_str(), &level_h),0);

            if ((par_id_h != par_id) || (level_h != level) || (type != string(type_tmp))) {
                // Not this field, skip it and read the next one
                free(type_tmp);
                MY_CODES_CHECK(codes_handle_delete(h), 0);
                continue;
            }

            // If the first field is exactly what we want we read then read it

            // Get data size
            MY_CODES_CHECK(ret, 0);
            MY_CODES_CHECK(codes_get_size(h, val_key.c_str(), &vals_len), 0);

            // Get data values
            p_vals = (double*) malloc(vals_len * sizeof (double));
            MY_CODES_CHECK(codes_get_double_array(h, val_key.c_str(), p_vals, &vals_len), 0);

            // Copy data to vector
            vals.clear();
            vals.resize(vals_len);
            copy(p_vals, p_vals + vals_len, vals.begin());

            // Housekeeping
            free(p_vals);
            MY_CODES_CHECK(codes_handle_delete(h), 0);
            codes_index_delete(index);

            free(type_tmp);
            return;
        }

        // If the field is still not found, try the third way to access data by traversing all
        // the messaging individually without using the index mechanism.
        //
        FILE* in = fopen(file.c_str(), "r");
        
        while ((h = codes_handle_new_from_file(0, in, PRODUCT_GRIB, &ret)) != NULL ) {

            CODES_CHECK(ret,0);

            CODES_CHECK(codes_get_long(h, par_key.c_str(), &par_id_h),0);
            if (par_id_h != par_id) {
                codes_handle_delete(h);
                continue;
            }

            CODES_CHECK(codes_get_long(h, level_key.c_str(), &level_h),0);
            if (level_h != level) {
                codes_handle_delete(h);
                continue;
            }

            CODES_CHECK(codes_get_length(h, type_key.c_str(), &str_len), 0);
            type_tmp = (char*)malloc(str_len*sizeof(char));
            CODES_CHECK(codes_get_string(h, type_key.c_str(), type_tmp, &str_len),0);
            if (type != string(type_tmp)) {
                codes_handle_delete(h);
                free(type_tmp);
                continue;
            }

            MY_CODES_CHECK(ret, 0);
            MY_CODES_CHECK(codes_get_size(h, val_key.c_str(), &vals_len), 0);

            // Get data values
            p_vals = (double*) malloc(vals_len * sizeof (double));
            MY_CODES_CHECK(codes_get_double_array(h, val_key.c_str(), p_vals, &vals_len), 0);

            // Copy data to vector
            vals.clear();
            vals.resize(vals_len);
            copy(p_vals, p_vals + vals_len, vals.begin());

            // Housekeeping
            free(p_vals);
            free(type_tmp);
            codes_handle_delete(h);
            fclose(in);

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

    void toForecasts(const vector<string> & files_in, const string & file_out,
            const vector<long> & pars_id, const vector<string> & pars_new_name,
            const vector<long> & crcl_pars_id, const vector<long> & levels,
            const vector<string> & level_types,
            const string & par_key, const string & level_key,
            const string & type_key, const string & val_key,
            string regex_time_str, string regex_flt_str,
            double flt_interval, bool delimited, int verbose) {

        if (verbose >= 3) cout << GREEN << "Convert GRIB2 files to Forecasts" << RESET << endl;

        // Read station xs and ys based on the first parameter in the list
        if (verbose >= 3) cout << GREEN << "Reading station information ... " << RESET << endl;
        anenSta::Stations stations;
        vector<double> xs, ys;
        getXY(xs, ys, files_in[0], pars_id[0], levels[0], par_key, level_key, val_key);
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
        if (verbose >= 4) cout << parameters;
        assert(parameters.size() == pars_new_name.size());

        // Prepare times
        if (verbose >= 3) cout << GREEN << "Reading time and FLT information ... " << RESET << endl;
        anenTime::Times times;
        regex regex_time;
        smatch match;

        try {
            regex_time = regex(regex_time_str);
        } catch (const std::regex_error& e) {
            cout << BOLDRED << "Error: Can't use the regular expression " << regex_time_str << RESET << endl;
            throw;
        }

        // Prepare flts
        anenTime::FLTs flts;
        regex regex_flt;

        try {
            regex_flt = regex(regex_flt_str);
        } catch (const std::regex_error& e) {
            cout << BOLDRED << "Error: Can't use the regular expression " << regex_flt_str << RESET << endl;
            throw;
        }

        // Read times and flts
        // Within this loop, it is also checking whether each file is valid
        //
        vector<double> times_vec, flts_vec;
        boost::gregorian::date time_start{anenTime::_ORIGIN_YEAR,
            anenTime::_ORIGIN_MONTH, anenTime::_ORIGIN_DAY}, time_end;
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
            }

            if (regex_search(file.begin(), file.end(), match, regex_flt)) {
                flts_vec.push_back(stoi(match[1]) * flt_interval);
            }
        }

        sort(flts_vec.begin(), flts_vec.end());
        flts.insert(flts.end(), flts_vec.begin(), flts_vec.end());

        sort(times_vec.begin(), times_vec.end());
        times.insert(times.end(), times_vec.begin(), times_vec.end());

        if (verbose >= 4) {
            cout << times << flts;
        }

        // Read data
        if (verbose >= 3) {
            cout << GREEN << "Allocating memory for double array [" << parameters.size()
                << "][" << stations.size() << "][" << times.size() << "]["
                << flts.size() << "] ... " << RESET << endl;
        }
        Forecasts_array forecasts(parameters, stations, times, flts);
        auto & data = forecasts.data();

        // This is created to keep track of the return condition for each file
        vector<bool> file_flags(files_in.size(), true);

        if (verbose >= 3) cout << GREEN << "Reading data ... " << RESET << endl;

#if defined(_OPENMP)
#pragma omp parallel for default(none) schedule(static) \
        shared(files_in, regex_flt, flt_interval, regex_time, delimited, time_start, \
                flts, times, data, forecasts, pars_id, levels, file_flags, cout, verbose, \
                level_types, par_key, level_key, type_key, val_key) private(match, time_end)
#endif
        for (size_t i = 0; i < files_in.size(); i++) {

#if defined(_OPENMP)
#pragma omp critical
#endif
            if (verbose >= 3) cout << "\t reading data from file " << files_in[i] << endl;

            // Find out flt index
            if (!regex_search(files_in[i].begin(), files_in[i].end(), match, regex_flt))
                throw runtime_error("Error: Counld not find flt index in flts.");
            size_t index_flt = flts.getTimeIndex(stoi(match[1]) * flt_interval);

            // Find out time index
            if (!regex_search(files_in[i].begin(), files_in[i].end(), match, regex_time))
                throw runtime_error("Error: Could not find time index in times.");

            if (delimited) {
                time_end = boost::gregorian::from_string(string(match[1]));
            } else {
                time_end = boost::gregorian::from_undelimited_string(string(match[1]));
            }
            boost::gregorian::date_duration duration = time_end - time_start;
            size_t index_time = times.getTimeIndex(duration.days() * _SECS_IN_DAY);

            vector<double> data_vec;
            for (size_t j = 0; j < pars_id.size(); j++) {

                if (file_flags[i]) {
                    try {
                        getDoubles(data_vec, files_in[i], pars_id[j], levels[j],
                                level_types[j], par_key, level_key, type_key, val_key);
                    } catch (...) {
#if defined(_OPENMP)
#pragma omp critical
#endif
                        cout << BOLDRED << "Error when reading " << pars_id[j] << " "
                            << levels[j] << " " << level_types[j] << " from file " << files_in[i]
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
            if (!file_flags[i]) cout << BOLDRED << "Error: The " << i+1
                << "th file in the input file lists caused a problem while reading data!"
                    << RESET << endl;
        }

        // Write forecasts
        if (verbose >= 3) cout << GREEN << "Writing Forecasts file ... " << RESET << endl;
        AnEnIO io("Write", file_out, "Forecasts", verbose);
        io.handleError(io.writeForecasts(forecasts));

        if (verbose >= 3) cout << GREEN << "Done!" << RESET << endl;
        return;
    }

    void toObservations(const vector<string> & files_in, const string & file_out,
            const vector<long> & pars_id, const vector<string> & pars_new_name,
            const vector<long> & crcl_pars_id, const vector<long> & levels,
            const vector<string> & level_types,
            const string & par_key, const string & level_key,
            const string & type_key, const string & val_key,
            string regex_time_str, bool delimited, int verbose) {

        if (verbose >= 3) cout << GREEN << "Convert GRIB2 files to Observations" << RESET << endl;

        // Read station xs and ys based on the first parameter in the list
        if (verbose >= 3) cout << GREEN << "Reading station information ... " << RESET << endl;
        anenSta::Stations stations;
        vector<double> xs, ys;
        getXY(xs, ys, files_in[0], pars_id[0], levels[0], par_key, level_key, val_key);
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
        if (verbose >= 4) cout << parameters;
        assert(parameters.size() == pars_new_name.size());

        // Prepare times
        if (verbose >= 3) cout << GREEN << "Reading time information ... " << RESET << endl;
        anenTime::Times times;
        regex regex_time;
        smatch match;

        try {
            regex_time = regex(regex_time_str);
        } catch (const std::regex_error& e) {
            cout << BOLDRED << "Error: Can't use the regular expression " << regex_time_str << RESET << endl;
            throw;
        }

        // Read times
        // Within this loop, it is also checking whether each file is valid
        //
        vector<double> times_vec;
        boost::gregorian::date time_start{anenTime::_ORIGIN_YEAR,
            anenTime::_ORIGIN_MONTH, anenTime::_ORIGIN_DAY}, time_end;
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
            }
        }

        sort(times_vec.begin(), times_vec.end());
        times.insert(times.end(), times_vec.begin(), times_vec.end());

        if (verbose >= 4) cout << times;
        // Read data
        if (verbose >= 3) cout << GREEN << "Reading data information ..." << RESET << endl;
        Observations_array observations(parameters, stations, times);
        auto & data = observations.data();

        // This is created to keep track of the return condition for each file
        vector<bool> file_flags(files_in.size(), true);

#if defined(_OPENMP)
#pragma omp parallel for default(none) schedule(static) \
        shared(files_in, regex_time, delimited, time_start, times, data, \
                observations, pars_id, levels, file_flags, cout, verbose, level_types, \
                par_key, level_key, type_key, val_key) private(match, time_end)
#endif
        for (size_t i = 0; i < files_in.size(); i++) {

#if defined(_OPENMP)
#pragma omp critical
#endif
            if (verbose >= 3) cout << "\t reading data from file " << files_in[i] << endl;

            // Find out time index
            if (!regex_search(files_in[i].begin(), files_in[i].end(), match, regex_time))
                throw runtime_error("Error: Could not find time index in times.");

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
                        getDoubles(data_vec, files_in[i], pars_id[j], levels[j],
                                level_types[j], par_key, level_key, type_key, val_key);
                    } catch (...) {
#if defined(_OPENMP)
#pragma omp critical
#endif
                        cout << BOLDRED << "Error when reading " << pars_id[j] << " "
                            << levels[j] << " " << level_types[j] << " from file " << files_in[i]
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
            if (!file_flags[i]) cout << BOLDRED << "Error: The " << i+1
                << "th file in the input file lists caused a problem while reading data!"
                    << RESET << endl;
        }

        // Write observations
        if (verbose >= 3) cout << GREEN << "Writing Observations file ... " << RESET << endl;
        AnEnIO io("Write", file_out, "Observations", verbose);
        io.handleError(io.writeObservations(observations));

        if (verbose >= 3) cout << GREEN << "Done!" << RESET << endl;
        return;
    }

}
