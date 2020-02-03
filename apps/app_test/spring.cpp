/* 
 * File:   analogGenerator.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *         Guido Cervone <cervone@psu.edu>
 *
 * Created on April 17, 2018, 11:10 PM
 */

/** @file */


#include "AnEnIS.h"
#include "AnEnReadNcdf.h"
#include "ForecastsPointer.h"
#include "ObservationsPointer.h"
#include <algorithm>

#include <map>

#include <ctime>

using namespace std;

int main(int argc, char** argv) {

    string forecast_file = "forecasts_211.nc";
    string observation_file = "observations_211.nc";

    ForecastsPointer forecasts;
    ObservationsPointer observations;

    AnEnReadNcdf read_nc;
    read_nc.readForecasts(forecast_file, forecasts);
    read_nc.readObservations(observation_file, observations);

    cout << observations << endl;

    size_t num_search_fcsts = 51;
    size_t search_start = 0;
    size_t num_test_fcsts = 2;
    size_t test_start = 55;

    vector<size_t> fcsts_test_index(num_test_fcsts);
    vector<size_t> fcsts_search_index(num_search_fcsts);

    iota(fcsts_search_index.begin(), fcsts_search_index.end(), search_start);
    iota(fcsts_test_index.begin(), fcsts_test_index.end(), test_start);

    cout << "Test indices: " << Functions::format(fcsts_test_index);
    cout << endl << "Search indices: " << Functions::format(fcsts_search_index);
    cout << endl;
    
    Config config;
    config.operation = true;
    config.prevent_search_future = true;
    config.save_sims = true;
    config.quick_sort = false;
    config.save_sims_day_index = true;
    config.save_analogs_day_index = false;
    config.num_sims = 2;
    config.num_analogs = 5;
    config.obs_var_index = 0;
    config.max_flt_nan = 0;
    config.max_par_nan = 0;
    config.flt_radius = 1;

    AnEnIS anen(config);
    anen.compute(forecasts, observations, fcsts_test_index, fcsts_search_index);

//    const Array4DPointer & analogs = anen.getAnalogsValue();
    const Array4DPointer & sims = anen.getSimsValue();
//    const Array4DPointer & sims_index = anen.getSimsTimeIndex();

    cout << sims << endl;

    return 0;
}
