/* 
 * File:   RcppFunctions.h
 * Author: Weiming Hu <weiming@psu.edu>
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

// [[Rcpp::plugins(cpp11)]]

namespace FunctionsR {
    void createParameters(Parameters & parameters, size_t total);
    void createStations(Stations & stations, size_t total);
    void createTimes(Times & times, size_t total);

    void toParameters(const SEXP & sx_name, const SEXP & sx_circulars, Parameters & parameters);
    void toStations(const SEXP & sx_xs, const SEXP & sx_ys, const SEXP & sx_names, Stations & stations);
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
