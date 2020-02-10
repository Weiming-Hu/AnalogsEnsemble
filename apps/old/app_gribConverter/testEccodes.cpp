/* 
 * File:   ParameterGrib.h
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on February 7, 2020, 5:57 PM
 */

#include <iostream>
#include "Times.h"
#include "AnEnReadGrib.h"
#include "ForecastsPointer.h"

using namespace std;

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

    string file("/home/graduate/wuh20/Desktop/nam_218_20120108_0600_002.grb2");
    ForecastsPointer forecasts;

    vector<ParameterGrib> grib_parameters;
    grib_parameters.push_back(ParameterGrib("U", false, 131, 1000, "isobaricInhPa"));
    grib_parameters.push_back(ParameterGrib("V", false, 132, 1000, "isobaricInhPa"));

    vector<string> files = {file};

    string regex_day_str = ".*nam_218_(\\d{8})_\\d{4}_\\d{3}\\.grb2$";
    string regex_flt_str = ".*nam_218_\\d{8}_\\d{4}_(\\d{3})\\.grb2$";
    string regex_cycle_str = ".*nam_218_\\d{8}_(\\d{2})\\d{2}_\\d{3}\\.grb2$";

    AnEnReadGrib anen_read;
    anen_read.readForecasts(forecasts, grib_parameters, files,
            regex_day_str, regex_flt_str, regex_cycle_str,
            3600, false, {0, 3, 5});

    cout << forecasts << endl;

    return 0;
}
