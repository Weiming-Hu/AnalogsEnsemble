/* 
 * File:   AnEnReadGrib.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 * 
 * Created on February 7, 2020, 1:02 PM
 */

#include <sstream>
#include <stdexcept>

#include "eccodes.h"
#include "Functions.h"
#include "FunctionsIO.h"
#include "AnEnReadGrib.h"

using namespace std;
using namespace boost::gregorian;

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
        const string & regex_day_str,
        const string & regex_flt_str,
        const string & regex_cycle_str,
        size_t flt_unit_in_seconds, bool delimited,
        vector<int> stations_index) const {

    /*
     * Parse files for forecast times and forecast lead times
     */
    if (verbose_ >= Verbose::Progress) cout << "Parsing files names for times and forecast lead times ..." << endl;
    Times times, flts;
    FunctionsIO::parseFilenames(times, flts, files,
            regex_day_str, regex_flt_str, regex_cycle_str,
            flt_unit_in_seconds, delimited);

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

    /*
     * Read forecasts
     */
    if (verbose_ >= Verbose::Progress) cout << "Reading forecast ..." << endl;
    bool ret;
    int err = 0;
    double* p_data = nullptr;
    Time file_time, file_flt;

    size_t data_len;
    size_t time_i, flt_i;
    size_t num_stations = stations.size();

    // This variable counts the number of failures when reading files
    size_t failed_files = 0;

    // These regular expressions can be reused
    regex regex_day = regex(regex_day_str);
    regex regex_flt = regex(regex_flt_str);
    regex regex_cycle = regex(regex_cycle_str);

    // Prepare the keys to be filtered when reading a file
    ostringstream index_keys_ss;
    index_keys_ss << ParameterGrib::_key_id << ","
            << ParameterGrib::_key_level << ","
            << ParameterGrib::_key_level_type;
    string index_keys = index_keys_ss.str();

    // This is the start time
    date start_day(from_string(Time::_origin));

    // Allocate memory for forecasts
    forecasts.setDimensions(parameters, stations, times, flts);

    // Turn on support for multi fields messages
    codes_grib_multi_support_on(0);

    for (const auto & file : files) {

        // Determine the time and flt index for this file
        if (regex_cycle_str.empty()) ret = FunctionsIO::parseFilename(
                file_time, file_flt, file, start_day, regex_day, regex_flt,
                flt_unit_in_seconds, delimited);
        else ret = FunctionsIO::parseFilename(
                file_time, file_flt, file, start_day, regex_day, regex_flt,
                regex_cycle, flt_unit_in_seconds, delimited);

        // Skip this file if the file is not recognized
        if (ret) {
            if (verbose_ >= Verbose::Detail) cout << "Reading " << file << endl;
        } else {
            if (verbose_ >= Verbose::Detail) cout << "Skipped " << file << endl;
            continue;
        }

        // Get index
        time_i = times.getIndex(file_time);
        flt_i = flts.getIndex(file_flt);

        try {

            // Prepare reading from this file
            codes_index *p_index = codes_index_new_from_file(
                    0, const_cast<char*> (file.c_str()),
                    index_keys.c_str(), &err);

            if (err) {
                ostringstream msg;
                msg << "Failed when opening file " << file;
                throw runtime_error(msg.str());
            }

            for (size_t parameter_i = 0; parameter_i < parameters.size(); ++parameter_i) {
                const ParameterGrib & parameter = grib_parameters[parameter_i];

                // Make the query
                codes_index_select_long(p_index, ParameterGrib::_key_id.c_str(), parameter.getId());
                codes_index_select_long(p_index, ParameterGrib::_key_level.c_str(), parameter.getLevel());
                codes_index_select_string(p_index, ParameterGrib::_key_level_type.c_str(),
                        const_cast<char*> (parameter.getLevelType().c_str()));

                // Create a handle to the index
                codes_handle* h = codes_handle_new_from_index(p_index, &err);
                CODES_CHECK(err, 0);

                // Read data from the GRIB file
                if (stations_index.empty()) {
                    CODES_CHECK(codes_get_size(
                                h, ParameterGrib::_key_values.c_str(),
                                &data_len), 0);

                    p_data = new double [data_len];

                    CODES_CHECK(codes_get_double_array(
                                h, ParameterGrib::_key_values.c_str(),
                                p_data, &data_len), 0);
                } else {
                    data_len = stations_index.size();

                    p_data = new double [data_len];

                    CODES_CHECK(codes_get_double_elements(
                                h, ParameterGrib::_key_values.c_str(),
                                stations_index.data(), data_len, p_data), 0);
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
                codes_handle_delete(h);
            }

            // Clean up
            codes_index_delete(p_index);

        } catch (...) {
            cerr << "Errorred when reading " << file << endl;
            failed_files++;
        }
    }

    if (verbose_ >= Verbose::Progress) cout << "Forecast reading complete" << endl;

    if (failed_files != 0)
        if (verbose_ >= Verbose::Warning)
            cerr << failed_files << " out of " << files.size() << " files failed during the reading process" << endl;

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
