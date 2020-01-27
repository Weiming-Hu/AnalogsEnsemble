/* 
 * File:   ObservationsR.cpp
 * Author: Weiming Hu <cervone@psu.edu>
 * 
 * Created on January 21, 2020, 3:37 PM
 */

#include "ObservationsR.h"
#include "FunctionsR.h"
#include "Functions.h"
#include "AnEnDefaults.h"
#include "boost/numeric/conversion/cast.hpp"

#include <sstream>
#include <stdexcept>

using namespace Rcpp;
using namespace boost;


ObservationsR::ObservationsR() {
}

ObservationsR::ObservationsR(SEXP sx_times, SEXP sx_data) {

    // Type checks
    if (!Rf_isNumeric(sx_data)) throw std::runtime_error("Observation data should be numeric");

    // Create wrappers around SEXP types
    NumericVector data = sx_data;

    // Get data dimensions
    if (!data.hasAttribute("dim")) throw std::runtime_error("Dimensions missing for search observations");

    IntegerVector data_dims = data.attr("dim");

    // Check dimension length
    if (data_dims.size() != 3) throw std::runtime_error(
            "Observations should be 3-dimensional [Parameters, Stations, Times]");

    try {
        // Create parameters
        FunctionsR::createParameters(parameters_, data_dims[0]);

        // Create stations
        FunctionsR::createStations(stations_, data_dims[1]);

        // Create times
        FunctionsR::toTimes(sx_times, times_);

    } catch (std::exception & ex) {
        std::string msg = std::string("ObservationsR -> ") + ex.what();
        throw std::runtime_error(msg);
    }

    if (numeric_cast<int>(times_.size()) != data_dims[2]) {
        std::ostringstream msg;
        msg << "Third dimensions of observations (" << data_dims[2]
                << ") != #unique observation times (" << times_.size() << ")";
        throw std::runtime_error(msg.str());
    }

    // Assign data
    data_ = REAL(data);
    memcpy(dims_, data_dims.begin(), 3 * sizeof(int));
//    dims_[0] = data_dims[0];
//    dims_[1] = data_dims[1];
//    dims_[2] = data_dims[2];
    
    allocated_ = false;
}

ObservationsR::~ObservationsR() {
}
