/* 
 * File:   AnEnReadGrib.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 * 
 * Created on February 7, 2020, 1:02 PM
 */

#include <numeric>
#include <sstream>
#include <algorithm>
#include <stdexcept>

#include "eccodes.h"
#include "Functions.h"
#include "FunctionsIO.h"
#include "AnEnReadGrib.h"

using namespace std;
using namespace boost::gregorian;
using namespace boost::xpressive;

AnEnReadGrib::AnEnReadGrib() {
    Config config;
    verbose_ = config.verbose;
}

AnEnReadGrib::AnEnReadGrib(const AnEnReadGrib& orig) {
    verbose_ = orig.verbose_;
}

AnEnReadGrib::AnEnReadGrib(Verbose verbose) : verbose_(verbose) {
}

AnEnReadGrib::~AnEnReadGrib() {
}

void
AnEnReadGrib::readForecasts(Forecasts & forecasts,
        const vector<ParameterGrib> & grib_parameters,
        const vector<string> & files,
        const string & regex_str,
        size_t flt_unit_in_seconds, bool delimited,
        vector<int> stations_index) const {

    /*
     * Read meta information for forecasts
     */
    readForecastsMeta_(forecasts, grib_parameters, files, regex_str,
            flt_unit_in_seconds, delimited, stations_index);

   /*
     * Read forecast data values
     */
    if (verbose_ >= Verbose::Progress) cout << "Reading forecast ..." << endl;
    bool ret;
    int err = 0;
    double* p_data = nullptr;
    Time file_time, file_flt;

    // Initialize variables in advance
    size_t data_len, parameter_i, time_i, flt_i;
    size_t num_stations = forecasts.getStations().size();
    long current_id, current_level;
    char* current_level_type;
    codes_handle *h = nullptr;
    FILE* in = nullptr;
    size_t str_len;
    const Times & times = forecasts.getTimes();
    const Times & flts = forecasts.getFLTs();

    // This variable counts the number of failures when reading files
    size_t failed_files = 0;
    size_t read_files = 0;

    // These regular expressions can be reused
    sregex rex = sregex::compile(regex_str);

    // This is the start time
    date start_day(from_string(Time::_origin));

    // Turn on support for multi fields messages
    codes_grib_multi_support_on(0);

    for (const auto & file : files) {

        // Determine the time and flt index for this file
        ret = FunctionsIO::parseFilename(file_time, file_flt, file, start_day,
                rex, flt_unit_in_seconds, delimited);

        // Skip this file if the file is not recognized
        if (ret) {
            if (verbose_ >= Verbose::Detail) cout << file << " --> Time: " << file_time.toString()
                << " Lead time: " << file_flt << " " << Time::_unit << endl;

            read_files++;

        } else {
            if (verbose_ >= Verbose::Debug) cout << "Skip " << file << endl;
            continue;
        }

        // Get index
        time_i = times.getIndex(file_time);
        flt_i = flts.getIndex(file_flt);

        // Define the parameter indices to find. This variable is used to
        // avoid searching for already found variables and to avoid excessive
        // function calls to eccodes.
        //
        vector<size_t> parameters_i(grib_parameters.size());
        iota(parameters_i.begin(), parameters_i.end(), 0);

        try {

            in = fopen(file.c_str(), "r");

            while ((h = codes_handle_new_from_file(0, in, PRODUCT_GRIB, &err)) != NULL) {
                if (err) throw runtime_error(codes_get_error_message(err));

                for (auto it = parameters_i.begin(); it != parameters_i.end(); ++it) {
                    
                    parameter_i = *it;

                    // Create a reference to the current parameter to avoid copy
                    const auto & current_parameter = grib_parameters[parameter_i];

                    // Check whether we have found the correct parameter id
                    err = codes_get_long(h, ParameterGrib::_key_id.c_str(), &current_id);
                    if (err) throw runtime_error(codes_get_error_message(err));
                    if (current_id != current_parameter.getId()) continue;

                    // Check whether we have found the correct level type
                    err = codes_get_length(h, ParameterGrib::_key_level_type.c_str(), &str_len);
                    if (err) throw runtime_error(codes_get_error_message(err));
                    current_level_type = new char[str_len];

                    err = codes_get_string(h, ParameterGrib::_key_level_type.c_str(), current_level_type, &str_len);
                    if (err) throw runtime_error(codes_get_error_message(err));

                    if (current_level_type != current_parameter.getLevelType()) {
                        delete [] current_level_type;
                        continue;
                    }
                    
                    delete [] current_level_type;

                    // Check whether we have found the correct level
                    err = codes_get_long(h, ParameterGrib::_key_level.c_str(), &current_level);
                    if (err) throw runtime_error(codes_get_error_message(err));
                    if (current_level != current_parameter.getLevel()) continue;

                    // Read data from the GRIB file
                    if (stations_index.empty()) {
                        if (codes_get_size(h, ParameterGrib::_key_values.c_str(), &data_len)) {
                            ostringstream msg;
                            msg << "Failed to read variable length for id: " << current_parameter.getId()
                                    << ", level: " << current_parameter.getLevel() << ", type of level: "
                                    << current_parameter.getLevelType() << endl
                                    << "The original message from Eccodes: " << codes_get_error_message(err);
                            throw runtime_error(msg.str());
                        }

                        p_data = new double [data_len];

                        codes_get_double_array(h, ParameterGrib::_key_values.c_str(), p_data, &data_len);

                    } else {
                        data_len = stations_index.size();

                        p_data = new double [data_len];

                        if (codes_get_double_elements(h, ParameterGrib::_key_values.c_str(), stations_index.data(), data_len, p_data)) {
                            ostringstream msg;
                            msg << "Failed to read variable for id: " << current_parameter.getId()
                                    << ", level: " << current_parameter.getLevel() << ", type of level: " << current_parameter.getLevelType() << endl
                                    << "The original message from Eccodes: " << codes_get_error_message(err);
                            throw runtime_error(msg.str());
                        }
                    }

                    if (num_stations != data_len) {
                        ostringstream msg;
                        msg << "The number of data values (" << data_len
                                << ") do not match the number of stations (" << num_stations
                                << "). Do you have duplicates in station coordinates?";
                        throw runtime_error(msg.str());
                    }

                    // Set values into the forecasts
                    for (size_t station_i = 0; station_i < data_len; ++station_i) {
                        forecasts.setValue(p_data[station_i], parameter_i, station_i, time_i, flt_i);
                    }

                    delete [] p_data;
                    
                    // Remove the parameter index that has been found
                    parameters_i.erase(it);
                    
                    // This handle has been processed. I can skip to the next handle.
                    break;
                }

                codes_handle_delete(h);

                // If all parameters have been found for this file, skip the rest of the messages
                if (parameters_i.size() == 0) break;
            }

            if (parameters_i.size() != 0) {
                stringstream msg;
                msg << parameters_i.size() << " out of " << grib_parameters.size() << " parameters are not found";
                throw runtime_error(msg.str());
            }

            // Clean up
            fclose(in);

        } catch (exception & e) {
            cerr << "Errored when reading " << file << ": " << e.what() << endl;
            failed_files++;
        }
    }

    if (failed_files != 0) {
        if (verbose_ >= Verbose::Warning) {
            cerr << failed_files << " out of " << read_files
                    << " files failed during the reading process" << endl;
        }

        if (failed_files == read_files) throw runtime_error("All files failed to read");
    }

    return;
}

void
AnEnReadGrib::readForecastsMeta_(Forecasts & forecasts,
        const vector<ParameterGrib> & grib_parameters,
        const vector<string> & files,
        const string & regex_str,
        size_t flt_unit_in_seconds, bool delimited,
        vector<int> stations_index) const {

    /*
     * Parse files for forecast times and forecast lead times
     */
    if (verbose_ >= Verbose::Progress) cout << "Parsing files names for times and forecast lead times ..." << endl;
    Times times, flts;
    FunctionsIO::parseFilenames(times, flts, files, regex_str, flt_unit_in_seconds, delimited);

    if (times.size() == 0) throw runtime_error("No day information extracted. Check filenames and regular expressions");
    if (flts.size() == 0) throw runtime_error("No lead time information extracted. Check filenames and regular expressions");

    // If stations index are not sorted, it will affect how I'm extracting coordinates.
    if (!is_sorted(stations_index.begin(), stations_index.end())) throw runtime_error("Stations index should be sorted in ascension order");

    /*
     * Read the first message from the first file and retrieve station coordinates
     */
    if (verbose_ >= Verbose::Progress) cout << "Read stations from the first file ..." << endl;
    Stations stations;
    readStations_(stations, files[0], stations_index);

    /*
     * Set up parameters
     */
    if (verbose_ >= Verbose::Progress) cout << "Insert forecast parameters ..." << endl;
    Parameters parameters;
    for (auto it = grib_parameters.begin(); it != grib_parameters.end(); ++it) {
        parameters.push_back(*it);
    }

    if (verbose_ >= Verbose::Debug) {
        cout << "(readForecasts) " << parameters
            << "(readForecasts)" << stations
            << "(readForecasts)" << times
            << "(readForecasts)" << flts;
    }

    if (parameters.size() != grib_parameters.size()) {
        stringstream msg;
        msg << grib_parameters.size() << " parameters specified but only "
            << parameters.size() << " parameters inserted. Do you have duplicated names?";
        throw runtime_error(msg.str());
    }

    // Allocate memory for forecasts
    forecasts.setDimensions(parameters, stations, times, flts);

    return;
}

void
AnEnReadGrib::readStations_(Stations & stations, const string & file,
        const vector<int> & stations_index) const {

    int err;
    FILE *in = fopen(file.c_str(), "r");
    codes_handle *h = codes_handle_new_from_file(0, in, PRODUCT_GRIB, &err);
    CODES_CHECK(err, 0);

    // Set a missing value
    CODES_CHECK(codes_set_double(h, "missingValue", NAN), 0);

    // Create an iterator to loop through all coordinates
    codes_iterator *iter = codes_grib_iterator_new(h, 0, &err);
    CODES_CHECK(err, 0);

    // Start with a clean repository
    stations.clear();

    double x, y, val;
    int counter = 0, station_index = 0;

    while (codes_grib_iterator_next(iter, &x, &y, &val)) {

        if (stations_index.size() != 0) {
            // If we are reading a subset of the stations

            // No need to read more stations if all subset stations have been read
            if (station_index == stations_index.size()) break;

            if (stations_index[station_index] != counter) {
                // If the current station is not what we want to read
                counter++;
                continue;
            }
        }

        // The current station is what we want to read
        Station station(x, y);
        stations.push_back(station);
        counter++;
        station_index++;
    }

    codes_grib_iterator_delete(iter);
    codes_handle_delete(h);
    fclose(in);

    return;
}
