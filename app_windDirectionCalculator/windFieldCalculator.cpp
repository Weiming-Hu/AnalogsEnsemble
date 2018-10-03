/* 
 * File:   windFieldCalculator.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on October 2, 2018, 9:20 PM
 */

/** @file */

#include "AnEnIO.h"
#include "CommonExeFunctions.h"

#include "boost/program_options.hpp"
#include "boost/multi_array.hpp"
#include "boost/filesystem.hpp"
#include "boost/exception/all.hpp"

#include <functional>
#include <iostream>
#include <sstream>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846  /* pi */
#endif

using namespace std;

void runWindFieldCalculator(
        const string & file_in, const string & file_type,
        const string & file_out,
        const string & var_U_name, const string & var_V_name,
        const string & var_dir_name, const string & var_speed_name,
        int verbose) {

    AnEnIO io("Read", file_in, file_type, verbose);

    if (file_type == "Forecasts") {

        Forecasts_array forecasts;
        io.handleError(io.readForecasts(forecasts));

        // calculation

        io.setMode("Write", file_out);
        io.handleError(io.writeForecasts(forecasts));

    } else if (file_type == "Observations") {

        Observations_array observations;
        io.handleError(io.readObservations(observations));

        // calculation

        io.setMode("Write", file_out);
        io.handleError(io.writeObservations(observations));

    } else {

    }

    return;
}

int main(int argc, char** argv) {
    cout << "Wind Field Calculator ...";
    return 0;
}
