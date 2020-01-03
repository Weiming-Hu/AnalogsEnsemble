/* 
 * File:   analogGenerator.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *         Guido Cervone <cervone@psu.edu>
 *
 * Created on April 17, 2018, 11:10 PM
 */

/** @file */

#include "AnEn.h"
#include "AnEnReadNcdf.h"

#include <boost/multi_array.hpp>
#include <algorithm>
#include <iostream>
#include <cstring>

using namespace std;

int main(int argc, char** argv) {

    string forecast_file = "/home/graduate/wuh20/github/AnalogsEnsemble/tests/Data/forecasts.nc";
    string observation_file = "/home/graduate/wuh20/github/AnalogsEnsemble/tests/Data/observations.nc";

    
    Forecasts_array forecasts;
    Observations_array observations;

    AnEnReadNcdf read_nc;
    read_nc.readForecasts(forecast_file, forecasts);
    read_nc.readObservations(observation_file, observations);

    // TODO: Print a slice of forecasts.
    //    using boost::indices;
    //    using boost::multi_array_types::index_range;
    //    auto view = forecasts.data()[ indices[0][0][index_range()][index_range()] ];

    cout << forecasts;
    cout << observations;

    return 0;
}
