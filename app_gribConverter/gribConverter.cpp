/* 
 * File:   similarityCalculator.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on September 17, 2018, 5:20 PM
 */

/** @file */

#include "eccodes.h"
#include "AnEn.h"
#include "AnEnIO.h"
#include "colorTexts.h"
#include "CommonExeFunctions.h"

#include "boost/filesystem.hpp"
#include "boost/date_time/gregorian/gregorian.hpp"
#include "boost/program_options/options_description.hpp"
#include "boost/program_options/parsers.hpp"
#include "boost/program_options/variables_map.hpp"

#include <chrono>
#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <cassert>
#include <exception>
#include <algorithm>

using namespace std;
namespace fs = boost::filesystem;

const static int _SECS_IN_DAY = 24 * 60 * 60;

/**
 * getXY gets the xs and ys of a parameter.
 * 
 * @param xs A vector to store the x values.
 * @param ys A vector to store the y values.
 * @param file The file to read data from.
 * @param par_id The ID of the parameter. This can be found using the 
 * grib_ls utility or by looking up in the ecCodes parameter database,
 * \link{http://apps.ecmwf.int/codes/grib/param-db/}.
 * @param level The level of the parameter.
 * @param par_key The key name of the parameter.
 * @param level_key The key name of the level.
 * @param val_key The key name of the value.
 */
void getXY(vector<double> & xs, vector<double> & ys, string file, long par_id,
        long level, string par_key = "paramId", string level_key = "level",
        string val_key = "values") {

    int ret;
    double* p_xs, *p_ys, *p_vals;
    size_t len;
    codes_index* index;
    codes_handle* h = NULL;

    // Construct query string
    string query_str(par_key);
    query_str.append(",");
    query_str.append(level_key);

    // Send query request
    index = codes_index_new_from_file(0, &file[0u], query_str.c_str(), &ret);
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

/**
 * getDoubles gets an array of values based on the paramId and the level.
 * The values are in row-major as the convention of C++.
 * 
 * @param vals A vector to store the data array.
 * @param file The file to read data from.
 * @param par_id The ID of the parameter. This can be found using the 
 * grib_ls utility or by looking up in the ecCodes parameter database,
 * \link{http://apps.ecmwf.int/codes/grib/param-db/}.
 * @param level The level of the parameter.
 * @param par_key The key name of the parameter.
 * @param level_key The key name of the level.
 * @param val_key The key name of the value.
 */
void getDoubles(vector<double> & vals, string file, long par_id, long level,
        string par_key = "paramId", string level_key = "level",
        string val_key = "values") {

    int ret;
    double* p_vals;
    size_t vals_len;
    codes_index* index;
    codes_handle* h = NULL;

    // Construct query string
    string query_str(par_key);
    query_str.append(",");
    query_str.append(level_key);

    // Send query request
    index = codes_index_new_from_file(0, &file[0u], query_str.c_str(), &ret);
    CODES_CHECK(ret, 0);

    // Select index based on par_key and level_key
    CODES_CHECK(codes_index_select_long(index, par_key.c_str(), par_id), 0);
    CODES_CHECK(codes_index_select_long(index, level_key.c_str(), level), 0);

    // Get data size
    h = codes_handle_new_from_index(index, &ret);
    CODES_CHECK(ret, 0);
    CODES_CHECK(codes_get_size(h, val_key.c_str(), &vals_len), 0);

    // Get data values
    p_vals = (double*) malloc(vals_len * sizeof (double));
    CODES_CHECK(codes_get_double_array(h, val_key.c_str(), p_vals, &vals_len), 0);

    // Copy data to vector
    vals.clear();
    vals.resize(vals_len);
    copy(p_vals, p_vals + vals_len, vals.begin());

    // Housekeeping
    free(p_vals);
    CODES_CHECK(codes_handle_delete(h), 0);
    codes_index_delete(index);

    return;
}

/**
 * toForecasts function reads data multiple forecast files and write them in
 * Forecasts NetCDf file that is ready to be used to compute Analog Ensembles.
 * 
 * Parameter and Station information are determined from the data in file. Parameter
 * is determined by the combination of the parameter ID and the level number. Station
 * is determined by the x (or lon) and y (or lat) variable.
 * 
 * FLT and Time information are determined from the file names using regular expressions.
 * The first variable matched in the regular expression will be used. For example,
 * we have a regular expression like this
 *         '.*nam_218_\d{8}_\d{4}_(\d{3})\.grb2$'
 * 
 * This expression has one extract variable '(\d{3})', and therefore the string
 * matched here will be used to calculate the FLT or Time information. The Time information
 * will be determined by calculating the duration in second from the original date.
 * 
 * @param files_in A vector of file names that are found in the folder. The folder
 * is provided in the program option.
 * @param file_out The output file path.
 * @param pars_id A vector of parameters ID that will be read from each file.
 * @param pars_new_name A vector of names that will assign to the parameters in the new
 * Forecasts NetCDF file.
 * @param crcl_pars_id A vector of parameters ID for circular parameters.
 * @param levels A vector of level numbers to specify for each parameter.
 * @param regex_time_str The regular expression used to extract time information.
 * @param regex_flt_str The regular expression used to extract FLT information.
 * @param flt_interval The FLT interval in seconds. This interval will be used to multiply
 * the FLT information extracted from the file name to determine the actual offset in seconds
 * for each FLT timestamp from the origin.
 * @param delimited Whether the extract Time information is delimited. A string like
 * '19900701' will be undelimited; a string like '1990-07-01' or '1990/07/01' is delimited.
 * @param verbose An integer for the verbose level.
 */
void toForecasts(const vector<string> & files_in, const string & file_out,
        const vector<long> & pars_id, const vector<string> & pars_new_name,
        const vector<long> & crcl_pars_id, const vector<long> & levels,
        string regex_time_str, string regex_flt_str, double flt_interval,
        bool delimited = false, int verbose = 0) {

    if (verbose >= 3) cout << "Convert GRIB2 files to Forecasts" << endl;

    // Basic checks
    if (files_in.size() == 0) throw runtime_error("Error: No files input specified!");
    if (pars_id.size() == 0) throw runtime_error("Error: No parameter Id specified!");
    if (pars_id.size() != pars_new_name.size())
        throw runtime_error("Error: The numbers of parameter Ids and new names!");
    if (pars_id.size() != levels.size())
        throw runtime_error("Error: The numbers of parameter Ids and levels do not match!");

    // Read station xs and ys based on the first parameter in the list
    if (verbose >= 3) cout << "Reading station information ... " << endl;
    anenSta::Stations stations;
    vector<double> xs, ys;
    getXY(xs, ys, files_in[0], pars_id[0], levels[0]);
    if (xs.size() != ys.size()) throw runtime_error("Error: the number of xs and ys do not match!");

    for (size_t i = 0; i < xs.size(); i++) {
        anenSta::Station station(xs[i], ys[i]);
        stations.push_back(station);
    }
    assert(stations.size() == xs.size());

    // Read parameters based on input options
    if (verbose >= 3) cout << "Reading parameter information ... " << endl;
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
    if (verbose >= 3) cout << "Reading time and FLT information ... " << endl;
    anenTime::Times times;
    regex regex_time;
    smatch match;

    try {
        regex_time = regex(regex_time_str);
    } catch (const std::regex_error& e) {
        cout << BOLDRED << "Error: Can't use the regular expression " << regex_time_str << RESET << endl;
        throw e;
    }

    // Prepare flts
    anenTime::FLTs flts;
    regex regex_flt;

    try {
        regex_flt = regex(regex_flt_str);
    } catch (const std::regex_error& e) {
        cout << BOLDRED << "Error: Can't use the regular expression " << regex_flt_str << RESET << endl;
        throw e;
    }

    // Read times and flts
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
        cout << "Allocating memory double [" << parameters.size() << "]["
            << stations.size() << "][" << times.size() << "][" << flts.size()
            << "] ... " << endl;
    }
    Forecasts_array forecasts(parameters, stations, times, flts);
    auto & data = forecasts.data();

    // This is created to keep track of the return condition for each file
    vector<bool> file_flags(files_in.size(), true);

    if (verbose >= 3) cout << "Reading data ... " << endl;

#if defined(_OPENMP)
#pragma omp parallel for default(none) schedule(static) \
shared(files_in, regex_flt, flt_interval, regex_time, delimited, time_start, \
flts, times, data, forecasts, pars_id, levels, file_flags, verbose, cout) \
private(match, time_end)
#endif
    for (size_t i = 0; i < files_in.size(); i++) {

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
                getDoubles(data_vec, files_in[i], pars_id[j], levels[j]);

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
    if (verbose >= 3) cout << "Writing Forecasts file ... " << endl;
    AnEnIO io("Write", file_out, "Forecasts");
    io.handleError(io.writeForecasts(forecasts));

    if (verbose >= 3) cout << "Done!" << endl;
    return;
}

/**
 * toObservations function reads data from multiple observation files and write them 
 * in Observations NetCDF fiel that is ready to be used to compute Analog Ensembles.
 * 
 * Parameter and Station information are determined from the data in file. Parameter
 * is determined by the combination of the parameter ID and the level number. Station
 * is determined by the x (or lon) and y (or lat) variable.
 * 
 * Time information are determined from the file names using regular expressions.
 * The first variable matched in the regular expression will be used. For example,
 * we have a regular expression like this
 *         '.*nam_218_\d{8}_(\d{4})_\d{3}\.grb2$'
 * 
 * This expression has one extract variable '(\d{4})', and therefore the string
 * matched here will be used to calculate the Time information. The Time information
 * will be determined by calculating the duration in second from the original date.
 * 
 * @param files_in A vector of file names that are found in the folder. The folder
 * is provided in the program option.
 * @param file_out The output file path.
 * @param pars_id A vector of parameters ID that will be read from each file.
 * @param pars_new_name A vector of names that will assign to the parameters in the new
 * Forecasts NetCDF file.
 * @param crcl_pars_id A vector of parameters ID for circular parameters.
 * @param levels A vector of level numbers to specify for each parameter.
 * @param regex_time_str The regular expression used to extract time information.
 * @param delimited Whether the extract Time information is delimited. A string like
 * '19900701' will be undelimited; a string like '1990-07-01' or '1990/07/01' is delimited.
 * @param verbose An integer for the verbose level.
 */
void toObservations(const vector<string> & files_in, const string & file_out,
        const vector<long> & pars_id, const vector<string> & pars_new_name,
        const vector<long> & crcl_pars_id, const vector<long> & levels,
        string regex_time_str, bool delimited = false, int verbose = 0) {

    if (verbose >= 3) cout << "Convert GRIB2 files to Observations" << endl;

    //Basic checks
    if (files_in.size() == 0) throw runtime_error("Error: No files input specified!");
    if (pars_id.size() == 0) throw runtime_error("Error: No parameter Id specified!");
    if (pars_id.size() != pars_new_name.size())
        throw runtime_error("Error: The numbers of parameter Ids and new names!");
    if (pars_id.size() != levels.size())
        throw runtime_error("Error: The numbers of parameter Ids and levels do not match!");


    // Read station xs and ys based on the first parameter in the list
    if (verbose >= 3) cout << "Reading station information ... " << endl;
    anenSta::Stations stations;
    vector<double> xs, ys;
    getXY(xs, ys, files_in[0], pars_id[0], levels[0]);
    if (xs.size() != ys.size()) throw runtime_error("Error: the number of xs and ys do not match!");

    for (size_t i = 0; i < xs.size(); i++) {
        anenSta::Station station(xs[i], ys[i]);
        stations.push_back(station);
    }
    assert(stations.size() == xs.size());

    // Read parameters based on input options
    if (verbose >= 3) cout << "Reading parameter information ... " << endl;
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
    if (verbose >= 3) cout << "Reading time and FLT information ... " << endl;
    anenTime::Times times;
    regex regex_time;
    smatch match;

    try {
        regex_time = regex(regex_time_str);
    } catch (const std::regex_error& e) {
        cout << BOLDRED << "Error: Can't use the regular expression " << regex_time_str << RESET << endl;
        throw e;
    }

    // Read times
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
    if (verbose >= 3) cout << "Reading data information ..." << endl;
    Observations_array observations(parameters, stations, times);
    auto & data = observations.data();
    
    // This is created to keep track of the return condition for each file
    vector<bool> file_flags(files_in.size(), true);

#if defined(_OPENMP)
#pragma omp parallel for default(none) schedule(static) \
shared(files_in, regex_time, delimited, time_start, times, data, \
observations, pars_id, levels, file_flags, cout, verbose) \
private(match, time_end)
#endif
    for (size_t i = 0; i < files_in.size(); i++) {

#pragma omp critical
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
                getDoubles(data_vec, files_in[i], pars_id[j], levels[j]);

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
    if (verbose >= 3) cout << "Writing Observations file ... " << endl;
    AnEnIO io("Write", file_out, "Observations");
    io.handleError(io.writeObservations(observations));

    if (verbose >= 3) cout << "Done!" << endl;
    return;
}

int main(int argc, char** argv) {

    namespace po = boost::program_options;

    // Required variables
    string folder, file_out, regex_time_str, regex_flt_str, output_type;
    vector<long> pars_id, levels;
    double unit_flt;

    // Optional variables
    int verbose;
    bool delimited, overwrite_output;
    string ext, config_file;
    vector<long> crcl_pars_id;
    vector<string> pars_new_name;

    try {
        po::options_description desc("Available options");
        desc.add_options()
                ("help,h", "Print help information for options.")
                ("config,c", po::value<string>(), "Set the configuration file path. Command line options overwrite options in configuration file.")

                ("output-type", po::value<string>(&output_type)->required(), "Set the output type. Currently it supports 'Forecasts'.")
                ("folder", po::value<string>(&folder)->required(), "Set the data folder.")
                ("output,o", po::value<string>(&file_out)->required(), "Set the output file path.")
                ("regex-time", po::value<string>(&regex_time_str)->required(), "Set the regular expression for time. Time information will be extracted from the file name.")
                ("regex-flt", po::value<string>(&regex_flt_str)->required(), "Set the regular expression for FLT. FLT information will be extracted from the file name.")
                ("pars-id", po::value< vector<long> >(&pars_id)->multitoken()->required(), "Set the parameters ID that will be read from the file.")
                ("levels", po::value< vector<long> >(&levels)->multitoken()->required(), "Set the levels for each parameters.")
                ("flt-interval", po::value<double>(&unit_flt)->required(), "Set the interval in seconds for FLT.")

                ("delimited", po::bool_switch(&delimited)->default_value(false), "The extracted time message is delimited by ambiguous character (1990-01-01).")
                ("ext", po::value<string>(&ext)->default_value(".grb2"), "Set the file extension.")
                ("circulars-id", po::value< vector<long> >(&crcl_pars_id)->multitoken(), "Set the IDs of circular parameters.")
                ("parameters-new-name", po::value< vector<string> >(&pars_new_name)->multitoken(), "Set the new names of each parameter.")
                ("overwrite", po::bool_switch(&overwrite_output)->default_value(false), "Overwrite the output file.")
                ("verbose,v", po::value<int>(&verbose)->default_value(0), "Set the verbose level.");

        // process unregistered keys and notify users about my guesses
        vector<string> available_options;
        auto lambda = [&available_options](const boost::shared_ptr<boost::program_options::option_description> option) {
            available_options.push_back("--" + option->long_name());
        };
        for_each(desc.options().begin(), desc.options().end(), lambda);

        // Parse the command line first
        po::variables_map vm;
        po::parsed_options parsed = po::command_line_parser(argc, argv).options(desc).allow_unregistered().run();
        store(parsed, vm);

        if (vm.count("help") || argc == 1) {
            cout << GREEN << "Analog Ensemble program --- GRIB Converter"
                << RESET << endl << desc << endl;
            return 0;
        }

        auto unregistered_keys = po::collect_unrecognized(parsed.options, po::exclude_positional);
        if (unregistered_keys.size() != 0) {
            guess_arguments(unregistered_keys, available_options);
            return 1;
        }

        // Then parse the configuration file
        if (vm.count("config")) {
            // If configuration file is specfied, read it first.
            // The variable won't be written until we call notify.
            //
            config_file = vm["config"].as<string>();
        }

        if (!config_file.empty()) {
            ifstream ifs(config_file.c_str());
            if (!ifs) {
                cout << BOLDRED << "Error: Can't open configuration file " << config_file << RESET << endl;
                return 1;
            } else {
                auto parsed_config = parse_config_file(ifs, desc, true);

                auto unregistered_keys_config = po::collect_unrecognized(parsed_config.options, po::exclude_positional);
                if (unregistered_keys_config.size() != 0) {
                    guess_arguments(unregistered_keys_config, available_options);
                    return 1;
                }

                store(parsed_config, vm);
            }
        }

        notify(vm);

        if (vm.count("parameters-new-name")) {
            if (pars_new_name.size() != pars_id.size()) {
                throw runtime_error("Error: the number of new names and parameters do not match!");
            }
        } else {
            pars_new_name.clear();
            for (const auto & i : pars_id) {
                pars_new_name.push_back(to_string(i));
            }
        }

        if (output_type != "Forecasts") {
            throw runtime_error("Error: Specified output type is not supported.");
        }

    } catch (...) {
        handle_exception(current_exception());
        return 1;
    }

    // List all files in a folder
    fs::recursive_directory_iterator it(folder), endit;
    vector<string> files_in;
    while (it != endit) {
        if (fs::is_regular_file(*it) && it->path().extension() == ext)
            files_in.push_back(folder + it->path().filename().string());
        it++;
    }

    // Sort the file list so that files are in incremental order
    sort(files_in.begin(), files_in.end());

    if (fs::exists(fs::path(file_out))) {
        if (overwrite_output) {
            fs::remove(fs::path(file_out));
            if (verbose >= 3) cout << "Remove existing output file " << file_out << endl;
        } else {
            cout << BOLDRED << "Error: Output file exists!" << RESET << endl;
            return 1;
        }
    }

    if (verbose >= 4) {
        cout << "folder: " << folder << endl
                << "file_out: " << file_out << endl
                << "regex_time_str: " << regex_time_str << endl
                << "regex_flt_str: " << regex_flt_str << endl
                << "output_type: " << output_type << endl
                << "pars_id: " << pars_id << endl
                << "levels: " << levels << endl
                << "verbose: " << verbose << endl
                << "delimited: " << delimited << endl
                << "ext: " << ext << endl
                << "config_file: " << config_file << endl
                << "crcl_pars_id: " << crcl_pars_id << endl
                << "pars_new_name: " << pars_new_name << endl
                << "input files (computed internally): " << files_in << endl;
    }

    // Call function to convert GRIB
    if (output_type == "Forecasts") {
        toForecasts(files_in, file_out, pars_id, pars_new_name,
                crcl_pars_id, levels, regex_time_str, regex_flt_str,
                unit_flt, delimited, verbose);
    } else if (output_type == "Observations") {
        toObservations(files_in, file_out, pars_id, pars_new_name,
                crcl_pars_id, levels, regex_time_str, delimited, verbose);
    } else {
        cout << BOLDRED << "Error: Output type " << output_type << " is not supported!"
                << RESET << endl;
        return 1;
    }

    return 0;
}
