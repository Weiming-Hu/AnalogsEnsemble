/* 
 * File:   canalogs.cpp
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <weiming@psu.edu>
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
//    string file(argv[argc - 1]);
    string file("/Users/wuh20/Desktop/ObservationsWind.nc");
    
    AnEnIO io("Read", file, "Observations", 3);

    Observations_array observations;
    io.handleError(io.readObservations(observations));
    
    io.setMode("Write", "/Users/wuh20/Desktop/test.nc");
    io.handleError(io.writeObservations(observations));
    
    return (0);
}