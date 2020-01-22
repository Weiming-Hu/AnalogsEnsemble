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

#include <Rcpp.h>

namespace FunctionsR {
    void createParameters(Parameters & parameters, size_t total);
    void createStations(Stations & stations, size_t total);
    void toParameters(const SEXP & sx_weights, const SEXP & sx_circulars, Parameters & parameters);
    void toTimes(const SEXP & sx_times, Times & times);
}

#endif /* FUNCTIONSR_H */

