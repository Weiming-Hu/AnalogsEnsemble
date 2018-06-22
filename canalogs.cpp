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
    string file(argv[argc - 1]);
    
    AnEnIO io(file);
    io.setVerbose(3);
    io.setFileType("Observations");

    Observations_array observations;
    io.handleError(io.readObservations(observations));
    
    cout << observations;

    return (0);
}