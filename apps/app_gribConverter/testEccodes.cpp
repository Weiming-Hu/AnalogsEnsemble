/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <iostream>

using namespace std;

#include <vector>
#include <string>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <regex>

#include "eccodes.h"
#include "Stations.h"
#include "Parameters.h"
#include "ForecastsPointer.h"

#include "boost/date_time/gregorian/gregorian.hpp"

static const double _SECONDS_IN_DAY = 24 * 60 * 60;
static const double _SECONDS_IN_HOUR = 60 * 60;

/**
 * Define a class for representing parameters from grib files
 */

class ParameterGrib : public Parameter {
public:
    ParameterGrib();
    ParameterGrib(const ParameterGrib &);
    ParameterGrib(string name, bool circular,
            long discipline, long category, long number);

    long getId() const {
        return id_;
    };

    long getLevel() const {
        return level_;
    };

    long getLevelType() const {
        return level_type_;
    };

    virtual ~ParameterGrib();

private:
    long id_;
    long level_;
    long level_type_;
};

ParameterGrib::ParameterGrib() : Parameter::Parameter() {
}

ParameterGrib::ParameterGrib(const ParameterGrib & parameter) :
Parameter::Parameter(parameter) {
}

ParameterGrib::ParameterGrib(string name, bool circular,
        long id, long level, long level_type) :
Parameter::Parameter(name, circular),
id_(id), level_(level), level_type_(level_type) {
}

ParameterGrib::~ParameterGrib() {
}

/**
 * Define a function to read a particular station and parameter from Grib files
// */
//
//void readParameter(Forecasts & forecasts, const vector<string> & files,
//        const vector<ParameterGrib> & parameters,
//        const string & regex_time_str,
//        const string & regex_flt_str,
//         const string & regex_cycle_str = '',
//        vector<int> stations_index = {},
//        bool delimited = false) {
//
//    using namespace boost::gregorian;
//
//    int err;
//
//    /*
//     * Set forecast lead times and forecast times
//     */
//    regex regex_time = regex(regex_time_str);
//    regex regex_flt = regex(regex_flt_str);
//    regex regex_cycle = regex(regex_cycle_str);
//
//    double timestamp;
//    Times times, flts;
//    bool has_time, has_flt;
//    date start_time, current_time;
//    smatch match_time, match_flt, match_cycle;
//
//    size_t counter = 0;
//    for (const auto & file : files) {
//        
//        // Apply the regular expression
//        has_time = regex_search(file.begin(), file.end(), match_time, regex_time);
//        has_flt = regex_search(file.begin(), file.end(), match_flt, regex_flt);
//
//        // Skip this iteration if no time information is found in the file name
//        if (!has_time) ;
//        if (!has_flt) continue;
//        
//        // Determine the current time
//        if (delimited) current_time = from_string(string(match_time[1]));
//        else current_time = from_undelimited_string(string(match_time[1]));
//        
//        // If this is the first iteration, this is also our start time
//        if (counter == 0) start_time = current_time;
//        
//        // Calculate the associated time stamp
//        timestamp = (current_time - start_time).days() * _SECONDS_IN_DAY;
//        
//        if (!regex_cycle_str.empty()) {
//            if (regex_search(file.begin(), file.end(), match_cycle, regex_cycle)) {
//                timestamp += stoi(match_cycle[1]) * _SECONDS_IN_HOUR;
//            } else {
//                throw runtime_error("Cannot find cycle time using the regular expression");
//            }
//        }
//        
//        // Push back
//        times.push_back(Times::value_type(counter, Times(timestamp)));
//        flts.push_back(Times::value_type(counter, ))
//    }
//
//
//    /*
//     * Set stations in forecasts using information from the first file
//     */
//
//    FILE *in = fopen(files[1].c_str(), "r");
//    codes_handle *h = codes_handle_new_from_file(0, in, PRODUCT_GRIB, &err);
//    CODES_CHECK(err, 0);
//
//    // Set a missing value
//    CODES_CHECK(codes_set_double(h, "missingValue", NAN), 0);
//
//    // Create an iterator to loop through all coordinates
//    codes_iterator *iter = codes_grib_iterator_new(h, 0, &err);
//    CODES_CHECK(err, 0);
//
//    double x, y, val;
//    Stations stations;
//    counter = 0;
//
//    while (codes_grib_iterator_next(iter, &x, &y, &val)) {
//        Station station(x, y);
//        stations.push_back(Stations::value_type(counter, station));
//        counter++;
//    }
//
//    codes_grib_iterator_delete(iter);
//    codes_handle_delete(h);
//    fclose(in);
//
//
//    // 
//
//
//    int err = 0;
//    double* p_data = nullptr;
//
//    size_t data_len;
//    size_t num_stations = forecasts.getStations().size();
//
//    // Check forecast dimensions
//    if (forecasts.getParameters().size() != parameters.size()) {
//        throw runtime_error("First forecast dimension does not match parameters");
//    }
//
//    // Turn on support for multi fields messages
//    codes_grib_multi_support_on(0);
//
//    // Create an index from the file
//    codes_index *p_index = codes_index_new_from_file(
//            0, const_cast<char*> (file.c_str()),
//            "parameter,level,typeOfLevel", &err);
//
//    if (err) {
//        printf("error: %s\n", codes_get_error_message(err));
//        exit(err);
//    }
//
//    for (size_t parameter_i = 0; parameter_i < parameters.size(); ++parameter_i) {
//        const ParameterGrib & parameter = parameters[parameter_i];
//
//        // Create a handle to the index
//        codes_handle* h = codes_handle_new_from_index(p_index, &err);
//        CODES_CHECK(err, 0);
//
//        // Make the query
//        codes_index_select_long(p_index, "parameter", parameter.getId());
//        codes_index_select_long(p_index, "level", parameter.getLevel());
//        codes_index_select_string(p_index, "typeOfLevel",
//                const_cast<char*> (parameter.getLevelType().c_str()));
//
//        // Read data from the GRIB file
//        if (indices.empty()) {
//            CODES_CHECK(codes_get_size(h, "values", &data_len), 0);
//            p_data = new double [data_len];
//            CODES_CHECK(codes_get_double_array(
//                    h, "values", data.data(), &data_len), 0);
//        } else {
//            data_len = indices.size();
//            p_data = new double [data_len];
//            CODES_CHECK(codes_get_double_elements(
//                    h, "values", indices.data(), data_len, data.data()), 0);
//        }
//
//        assert(data_len == num_stations);
//
//        // Set values into the forecasts
//        for (size_t station_i = 0; station_i < data_len; ++station_i) {
//            forecasts.setValue(p_data[station_i],
//                    parameter_i, station_i, current_time_index, current_flt_index);
//        }
//
//        delete [] p_data;
//        codes_handle_delete(h);
//    }
//
//    // Clean up
//    codes_index_delete(p_index);
//    return;
//}

int main() {
//
//    string file("/home/graduate/wuh20/Desktop/nam_218_20120108_0000_011.grb2");
//
//
//
//
//    ForecastsPointer forecasts;
//    forecasts.setDimensions(parameters, stations, times, flts);
//
//    readParameter(forecasts,{file}, 132, 1000, "isobaricInhPa",
//    {
//        0, 1, 2, 3
//    });
//
//    cout << "Wind: ";
//    for (const auto & e : data) cout << e << ",";
//    cout << endl;

    return 0;
}