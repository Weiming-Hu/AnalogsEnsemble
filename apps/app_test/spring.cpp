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

#include "ForecastsArray.h"
#include "ObservationsArray.h"

using namespace std;

int main(int argc, char** argv) {

    string forecast_file = "forecasts.nc";
    string observation_file = "observations.nc";

    ForecastsArray forecasts;
    ObservationsArray observations;

    AnEnReadNcdf read_nc;
    read_nc.readForecasts(forecast_file, forecasts);
    read_nc.readObservations(observation_file, observations);

    cout << forecasts;
    cout << observations;

    return 0;
}
