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
#include "AnEnIS.h"

#include <Rcpp.h>

namespace FunctionsR {
    void createParameters(Parameters & parameters, size_t total);
    void createStations(Stations & stations, size_t total);
    void toParameters(const SEXP & sx_weights, const SEXP & sx_circulars, Parameters & parameters);
    void toTimes(const SEXP & sx_times, Times & times);
    void setElement(Rcpp::List & list, const std::string & name, const Array4D & arr);
    
    enum class ConfigMode {
        DATA_IN_MEMORY
    };
    
    ConfigMode checkConfig(const SEXP & sx_config);
    void checkType(const Rcpp::List & config, std::vector<std::string> names,
            Rboolean (typeFunc)(SEXP), const std::string type_name);
}

#endif /* FUNCTIONSR_H */

