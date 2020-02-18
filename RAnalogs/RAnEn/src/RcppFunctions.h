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
#include "Functions.h"
#include "AnEnIS.h"

#include <Rcpp.h>

// [[Rcpp::depends(BH)]]    
// [[Rcpp::plugins(cpp11)]]

namespace FunctionsR {
    void createParameters(const SEXP & sx_circulars, Parameters & parameters, int total);
    void createStations(const SEXP & sx_names, Stations & stations, int total);

    void toParameters(const SEXP & sx_name, const SEXP & sx_circulars, Parameters & parameters);
    void toStations(const SEXP & sx_xs, const SEXP & sx_ys, const SEXP & sx_names, Stations & stations);
    void toTimes(const SEXP & sx_times, Times & times);

    void setElement(Rcpp::List & list, const std::string & name,
                    const Array4D & arr, bool index_conversion = false);
    void setElement(Rcpp::List & list, const std::string & name,
                    const Functions::Matrix & mat, bool index_conversion = false);
    
    enum class ConfigMode {
        DATA_IN_MEMORY
    };
    
    ConfigMode checkConfig(const SEXP & sx_config);
    void checkType(const Rcpp::List & config, std::vector<std::string> names,
            Rboolean (typeFunc)(SEXP), const std::string type_name);
}

#endif /* FUNCTIONSR_H */

