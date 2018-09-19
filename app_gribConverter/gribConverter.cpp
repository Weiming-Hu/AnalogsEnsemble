/* 
 * File:   similarityCalculator.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on September 17, 2018, 5:20 PM
 */
#include "eccodes.h"
#include "AnEn.h"
#include "AnEnIO.h"
#include "colorTexts.h"
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

// A convenient function for printing vectors

template<class T>
ostream& operator<<(ostream& os, const vector<T>& v) {
    copy(v.begin(), v.end(), ostream_iterator<T>(os, ","));
    return os;
}

void handle_exception(const exception_ptr & eptr) {
    // handle general exceptions
    try {
        if (eptr) {
            rethrow_exception(eptr);
        }
    } catch (const exception & e) {
        cerr << RED << e.what() << RESET << endl;
    } catch (...) {
        cerr << RED << "ERROR: caught unknown exceptions!" << RESET << endl;
    }
}

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

void convertForecasts(const vector<string> & files_in, const string & file_out,
        const vector<long> & pars_id, const vector<string> & pars_new_name,
        const vector<long> & crcl_pars_id, const vector<long> & levels,
        string regex_time_str, string regex_flt_str, double unit_flt,
        bool delimited = false) {

    // Basic checks
    if (files_in.size() == 0) throw runtime_error("Error: No files input specified!");
    if (pars_id.size() == 0) throw runtime_error("Error: No parameter Id specified!");
    if (pars_id.size() != pars_new_name.size())
        throw runtime_error("Error: The numbers of parameter Ids and new names!");
    if (pars_id.size() != levels.size())
        throw runtime_error("Error: The numbers of parameter Ids and levels do not match!");

    // Read station xs and ys based on the first parameter in the list
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
    anenPar::Parameters parameters;
    for (size_t i = 0; i < pars_new_name.size(); i++) {
        if (find(crcl_pars_id.begin(), crcl_pars_id.end(), pars_id[i]) != crcl_pars_id.end()) {
            anenPar::Parameter parameter(pars_new_name[i], true);
            parameters.push_back(parameter);
        } else {
            anenPar::Parameter parameter(pars_new_name[i], false);
            parameters.push_back(parameter);
        }
    }
    assert(parameters.size() == pars_new_name.size());

    // Prepare times
    anenTime::Times times;
    regex regex_time;
    smatch match;

    try {
        regex_time = regex(regex_time_str);
    } catch (const std::regex_error& e) {
        cout << BOLDRED << "Error: Can't not use the regular expression " << regex_time_str << RESET << endl;
        throw e;
    }

    // Prepare flts
    anenTime::FLTs flts;
    regex regex_flt;

    try {
        regex_flt = regex(regex_flt_str);
    } catch (const std::regex_error& e) {
        cout << BOLDRED << "Error: Can't not use the regular expression " << regex_flt_str << RESET << endl;
        throw e;
    }

    // Read times and flts
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
            times.push_back(duration.days() * _SECS_IN_DAY);
        }

        if (regex_search(file.begin(), file.end(), match, regex_flt)) {
            flts.push_back(stoi(match[1]) * unit_flt);
        }
    }

    // Read data
    Forecasts_array forecasts(parameters, stations, times, flts);
    auto & data = forecasts.data();

    // This is created to keep track of the return condition for each file
    vector<bool> file_flags(files_in.size(), true);

#if defined(_OPENMP)
#pragma omp parallel for default(none) schedule(static) \
shared(files_in, regex_flt, unit_flt, regex_time, delimited, time_start, \
flts, times, data, forecasts, pars_id, levels, file_flags) private(match, time_end)
#endif
    for (size_t i = 0; i < files_in.size(); i++) {

        // Find out flt index
        regex_search(files_in[i].begin(), files_in[i].end(), match, regex_flt);
        size_t index_flt = flts.getTimeIndex(stoi(match[1]) * unit_flt);

        // Find out time index
        regex_search(files_in[i].begin(), files_in[i].end(), match, regex_time);

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

    // Write forecasts
    AnEnIO io("Write", file_out, "Forecasts");
    io.handleError(io.writeForecasts(forecasts));

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
    bool delimited;
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
                ("verbose,v", po::value<int>(&verbose)->default_value(0), "Set the verbose level.");

        // Parse the command line first
        po::variables_map vm;
        store(po::command_line_parser(argc, argv).options(desc).run(), vm);
        
        if (vm.count("config")) {
            // If configuration file is specfied, read it first.
            // The variable won't be written until we call notify.
            //
            config_file = vm["config"].as<string>();
        }

        // Then parse the configuration file
        if (!config_file.empty()) {
            ifstream ifs(config_file.c_str());
            if (!ifs) {
                cout << BOLDRED << "Error: Can't open configuration file " << config_file << RESET << endl;
                return 1;
            } else {
                store(parse_config_file(ifs, desc), vm);
            }
        }

        if (vm.count("help") || argc == 1) {
            cout << GREEN << "GRIB Converter" << RESET << endl << desc << endl;
            return 0;
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
            files_in.push_back(string() + it->path().filename().string());
        it++;
    }

    // Sort the file list so that files are in incremental order
    sort(files_in.begin(), files_in.end());

    // Call function to convert GRIB to Forecasts
    if (output_type == "Forecasts") {
        convertForecasts(files_in, file_out, pars_id, pars_new_name,
                crcl_pars_id, levels, regex_time_str, regex_flt_str,
                unit_flt, delimited);
    }

    return 0;
}