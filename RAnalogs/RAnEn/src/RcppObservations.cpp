/* 
 * File:   RcppObservations.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 * 
 * Created on January 21, 2020, 3:37 PM
 */

#include "RcppObservations.h"
#include "RcppFunctions.h"
#include "boost/numeric/conversion/cast.hpp"

#include <sstream>
#include <stdexcept>

using namespace Rcpp;
using namespace boost;

ObservationsR::ObservationsR(SEXP sx_data, SEXP sx_names, SEXP sx_times) {

    // Type checks
    if (!Rf_isNumeric(sx_data)) throw std::runtime_error("Observation data should be numeric");

    // Create wrappers around SEXP types
    NumericVector data = sx_data;

    // Get data dimensions
    if (!data.hasAttribute("dim")) throw std::runtime_error("Dimensions missing for search observations");

    IntegerVector data_dims = data.attr("dim");

    // Check dimension length
    if (data_dims.size() != 3) throw std::runtime_error(
            "Observations data array should be 3-dimensional [Parameters, Stations, Times]");

    try {
        // Create parameters
        if (Rf_isNull(sx_names)) {
            FunctionsR::createParameters(R_NilValue, parameters_, data_dims[0]);
        } else {
            FunctionsR::toParameters(sx_names, R_NilValue, parameters_);
        }

        // Create stations
        FunctionsR::createStations(R_NilValue, stations_, data_dims[1]);

        // Create times
        FunctionsR::toTimes(sx_times, times_);

    } catch (std::exception & ex) {
        std::string msg = std::string("Observations -> ") + ex.what();
        throw std::runtime_error(msg);
    }

    if (parameters_.size() != numeric_cast<size_t>(data_dims[0])) {
        std::ostringstream msg;
        msg << "First dimension of observation data array (" << data_dims[0]
                << ") != #parameters (" << parameters_.size() << ")";
        throw std::runtime_error(msg.str());
    }

    if (times_.size() != numeric_cast<size_t>(data_dims[2])) {
        std::ostringstream msg;
        msg << "Third dimensions of observation data array (" << data_dims[2]
                << ") != #unique observation times (" << times_.size() << ")";
        throw std::runtime_error(msg.str());
    }

    // Assign data
    data_ = REAL(data);

    // Assign dimensions
    dims_[_DIM_PARAMETER] = numeric_cast<size_t>(data_dims[_DIM_PARAMETER]);
    dims_[_DIM_STATION] = numeric_cast<size_t>(data_dims[_DIM_STATION]);
    dims_[_DIM_TIME] = numeric_cast<size_t>(data_dims[_DIM_TIME]);
    
    allocated_ = false;
}

ObservationsR::~ObservationsR() {
}

void
ObservationsR::print(std::ostream & os) const {

    os << "[Observations] shape [" <<
            parameters_.size() << ", " <<
            stations_.size() << ", " <<
            times_.size() << "]" << std::endl;
    
    // Print out all parameters to display circular variables and weights
    os << parameters_;

    return;
}

std::ostream & operator<<(std::ostream & os, const ObservationsR & obj) {
    obj.print(os);
    return os;
}
