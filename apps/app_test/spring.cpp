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
#include "ForecastsArray.h"
#include "ObservationsArray.h"

#include <iterator>

using namespace std;

int main(int argc, char** argv) {

    string forecast_file = "test_forecasts.nc";
    string observation_file = "test_observations.nc";

    ForecastsArray forecasts;
    ObservationsArray observations;

    AnEnReadNcdf read_nc;
    read_nc.readForecasts(forecast_file, forecasts);
    read_nc.readObservations(observation_file, observations);
    
    size_t search_start = 1, search_end = 6, test_start = 8, test_end = 9;

    Times test_times, search_times;
    const auto & it = forecasts.getTimes().begin();
    test_times.assign(it + test_start, it + test_end + 1);
    search_times.assign(it + search_start, it + search_end + 1);

    size_t num_members = 2;
    bool operational = true, check_time_overlap = true, save_sims = true;
    
    AnEnIS anen(num_members, operational, check_time_overlap, save_sims,
            AnEnDefaults::Verbose::Debug, 0, false, true, true);
    
    anen.compute(forecasts, observations, test_times, search_times);
    
    

    return 0;
}
