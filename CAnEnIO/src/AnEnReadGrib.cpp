/* 
 * File:   AnEnReadGrib.cpp
 * Author: wuh20
 * 
 * Created on February 7, 2020, 1:02 PM
 */

#include "FunctionsIO.h"
#include "AnEnReadGrib.h"

using namespace std;

AnEnReadGrib::AnEnReadGrib() {
}

AnEnReadGrib::AnEnReadGrib(const AnEnReadGrib& orig) {
}

AnEnReadGrib::~AnEnReadGrib() {
}

void
AnEnReadGrib::readForecasts(vector<string> files, Forecasts & forecasts) const {
    
    /*
     * Parse files for forecast times and forecast lead times
     */
    Times times, flts;
//    FunctionsIO::parseFiles(times, flts, files,
//            regex_time_str, regex_flt_str, regex_cycle_str,
//            flt_unit_in_seconds, delimited);
    
}

void
AnEnReadGrib::readObservations(vector<string> files, Observations& observations) const {

}
