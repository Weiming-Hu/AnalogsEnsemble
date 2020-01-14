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

//    string forecast_file = "test_forecasts.nc";
//    string observation_file = "test_observations.nc";
//
//    ForecastsArray forecasts;
//    ObservationsArray observations;
//
//    AnEnReadNcdf read_nc;
//    read_nc.readForecasts(forecast_file, forecasts);
//    read_nc.readObservations(observation_file, observations);
//    
//    size_t search_start = 1, search_end = 6, test_start = 8, test_end = 9;
//
//    Times test_times, search_times;
//    const auto & it = forecasts.getTimes().begin();
//    test_times.assign(it + test_start, it + test_end + 1);
//    search_times.assign(it + search_start, it + search_end + 1);
//
//    size_t num_members = 2;
//    bool operational = true, check_time_overlap = true, save_sims = true;
//    
//    AnEnIS anen(num_members, operational, check_time_overlap, save_sims,
//            AnEnDefaults::Verbose::Debug, 0, false, true, true);
//    
//    anen.compute(forecasts, observations, test_times, search_times);

    vector<double> vec(20);
    for (auto & e : vec) e = rand() % 1000;
    
    cout << "Vector values: " << Functions::format(vec) << endl;
    double linear_sd_old = Functions::sdLinear(vec),
            circular_sd_old = Functions::sdCircular(vec),
            mean_pre, mean_cur;
    cout << "Original Sds (not circular): " << linear_sd_old << endl
            << "Original Sds (circular): " << circular_sd_old << endl;
    
    
    double val_1 = 0;
    cout << "Test value: " << val_1 << endl;
    
    mean_pre = Functions::mean(vec);
    mean_cur = mean_pre + (val_1 - mean_pre) / (vec.size() + 1);
    double linear_result = sqrt((pow(linear_sd_old, 2) * (vec.size() - 1) + (val_1 - mean_pre) * (val_1 - mean_cur)) / vec.size()),
            circular_result = sqrt((pow(circular_sd_old, 2) * (vec.size() - 1) +
            Functions::sdCircular({val_1, mean_pre}) * Functions::sdCircular({val_1, mean_cur})) / vec.size());
    
    cout << "New Sds (not circular): " << linear_result << endl
            << "New Sds (circular): " << circular_result << endl;
    
    vec.push_back(val_1);
    cout << "New Sds (not circular) answer: " << Functions::sdLinear(vec) << endl
            << "New Sds (circular) answer: " << Functions::sdCircular(vec) << endl;
    vec.pop_back();
    
    
    
    
    

    return 0;
}
