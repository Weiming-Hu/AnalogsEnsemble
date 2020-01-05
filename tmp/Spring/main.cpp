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
#include <iostream>
#include <cmath>

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

    //    cout << forecasts;
    //    cout << observations;
    //    
    
    
    cout << forecasts.getTimes() << endl;
    return 0;
}

