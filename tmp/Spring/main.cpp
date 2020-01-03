/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: wuh20
 *
 * Created on January 2, 2020, 4:32 PM
 */

#include <cstdlib>
#include "ForecastsArray.h"
#include "ObservationsArray.h"
#include "AnEnReadNcdf.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    string forecast_file = "forecasts.nc";
    string observation_file = "observations.nc";

    
    ForecastsArray forecasts;
    ObservationsArray observations;

    AnEnReadNcdf read_nc;
    read_nc.readForecasts(forecast_file, forecasts);
    read_nc.readObservations(observation_file, observations);

    // TODO: Print a slice of forecasts.
    //    using boost::indices;
    //    using boost::multi_array_types::index_range;
    //    auto view = forecasts.data()[ indices[0][0][index_range()][index_range()] ];

//    cout << forecasts;
cout << observations;
    return 0;
}

