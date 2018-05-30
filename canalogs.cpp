/* 
 * File:   canalogs.cpp
 * Author: guido
 *
 * Created on April 17, 2018, 11:10 PM
 */

#include <cstdlib>
#include <string>

#include "Stations.h"
#include "Parameters.h"
#include "Times.h"
#include "Forecasts.h"
#include "AnEnIO.h"

using namespace std;

int main(int argc, char** argv) {
    string file(argv[argc-1]);
    AnEnIO io(file);
    io.setVerbose(3);
    io.setRequiredVariables({"Data", "ParameterNames", "Times"});
    io.setOptionalVariables({"Xs", "Ys", "CircularParameters"});
    io.setRequiredDimensions({"num_chars", "num_parameters",
        "num_stations", "num_times"});
    io.handleError(io.checkDimensions());
    io.handleError(io.checkVariables());

    Parameters parameters;
    io.handleError(io.readParameters(parameters));

    Stations stations;
    io.handleError(io.readStations(stations));

    Times times;
    io.handleError(io.readTimes(times));

    Observations_array observations(parameters, stations, times);
    io.setFileType("Observations");
    io.handleError(io.readData(observations));

    return (0);
}