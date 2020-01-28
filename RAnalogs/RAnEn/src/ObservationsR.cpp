/* 
 * File:   ObservationsR.cpp
 * Author: Weiming Hu <cervone@psu.edu>
 * 
 * Created on January 21, 2020, 3:37 PM
 */

#include "ObservationsR.h"
#include "FunctionsR.h"
#include "boost/numeric/conversion/cast.hpp"

#include <sstream>
#include <stdexcept>

using namespace Rcpp;
using namespace boost;

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

    if (times_.size() != numeric_cast<size_t>(data_dims[2])) {
        std::ostringstream msg;
        msg << "Third dimensions of observations (" << data_dims[2]
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

    os << "[Observations] size: [" <<
            parameters_.size() << ", " <<
            stations_.size() << ", " <<
            times_.size() << "]" << std::endl;
    
    // Print out all parameters to display circular variables and weights
    os << parameters_;

    // Preview data
    size_t num_print = (num_elements() < AnEnDefaults::_PREVIEW_COUNT ?
            num_elements() : AnEnDefaults::_PREVIEW_COUNT);
    os << "[Data] size: " << num_elements() << std::endl;
    Functions::format(data_, num_print);

    return;
}

std::ostream & operator<<(std::ostream & os, const ObservationsR & obj) {
    obj.print(os);
    return os;
}
