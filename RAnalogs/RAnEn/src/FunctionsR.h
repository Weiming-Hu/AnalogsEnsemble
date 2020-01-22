/* 
 * File:   FunctionsR.h
 * Author: Weiming Hu <cervone@psu.edu>
 *
 * Created on January 22, 2020, 3:53 PM
 */

#ifndef FUNCTIONSR_H
#define FUNCTIONSR_H

#include "Times.h"
#include "Stations.h"
#include "Parameters.h"

#include <Rcpp>

namespace FunctionsR {
    void createStations(Stations & stations, size_t total);
    void toParameters(Parameters & parameters, SEXP);
    void toTimes(Times & times, SEXP);
}

#endif /* FUNCTIONSR_H */

