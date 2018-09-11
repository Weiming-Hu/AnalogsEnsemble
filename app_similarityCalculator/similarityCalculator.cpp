/* 
 * File:   similarityCalculator.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on September 11, 2018, 5:40 PM
 */

#include "AnEnIO.h"
#include "AnEn.h"

#include <iostream>

using namespace std;

// Program options:
// https://theboostcpplibraries.com/boost.program_options
// https://www.boost.org/doc/libs/1_58_0/doc/html/program_options/tutorial.html#idp337627504

void runSimilarityCalculator() {
    
    // I need the following arguments
    
    // AnEnIO
    string file_test_forecasts, file_search_forecasts, file_observations;
    int verbose;
    
    // Read input data
    string mode = "Read", file_type = "Forecasts";
    Forecasts_array test_forecasts, search_forecasts;
    Observations_array observations;
    
    AnEnIO io(mode, file_test_forecasts, file_type, verbose);
    
}

int main(int argc, char** argv) {
    cout << "Calculates similarity metrics." << endl;
    
    
    return 0;
}

