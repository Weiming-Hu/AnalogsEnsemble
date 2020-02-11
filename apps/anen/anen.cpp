/* 
 * File:   anen.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on February, 2020, 11:10 AM
 */

/** @file */

#include "boost/program_options.hpp"
#include "boost/filesystem.hpp"

#include "AnEnIS.h"
#include "AnEnSSE.h"
#include "AnEnReadGrib.h"
#include "ForecastsPointer.h"
#include "ObservationsPointer.h"

using namespace std;

int main() {
    
    // Input
    vector<string> forecast_files, analysis_files;
    Verbose verbose;
    vector<ParameterGrib> grib_parameters;
    Config config;
    vector<size_t> test_times, search_times;
    
    string regex_day_str, regex_flt_str, regex_cycle_str;
    double unit_in_seconds;
    bool delimited;
    vector<int> stations_index;
    
    /*
     * Read forecasts from files
     */
    AnEnReadGrib anenio_grib(verbose);
    ForecastsPointer forecasts;
    anenio_grib.readForecasts(forecasts, grib_parameters, forecast_files,
            regex_day_str, regex_flt_str, regex_cycle_str,
            unit_in_seconds, delimited, stations_index);
    
    /*
     * Read forecast analysis from files and convert them to observations
     */
    ForecastsPointer analysis;
    anenio_grib.readForecasts(forecasts, grib_parameters, analysis_files,
            regex_day_str, regex_flt_str, regex_cycle_str,
            unit_in_seconds, delimited, stations_index);
    
    // Convert analysis to observations
    ObservationsPointer observations;
    FunctionsIO::collapseLeadTimes(observations, analysis);
    
    /*
     * Generate analogs
     */
    AnEnIS anen(config);
    anen.compute(forecasts, observations, test_times, search_times);
    
    const Array4DPointer & analogs = anen.analogs_value();
    
    
    
    return 0;
}