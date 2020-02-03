/* 
 * File:   ForecastsR.cpp
 * Author: Weiming Hu <cervone@psu.edu>
 * 
 * Created on January 21, 2020, 3:36 PM
 */

#include "ForecastsR.h"
#include "FunctionsR.h"
#include "Functions.h"
#include "boost/numeric/conversion/cast.hpp"

#include <sstream>
#include <stdexcept>

using namespace boost;

ForecastsR::ForecastsR(SEXP sx_weights, SEXP sx_circulars,
        SEXP sx_times, SEXP sx_flts, SEXP sx_data) {

    // Type checks
    if (!Rf_isNumeric(sx_data)) throw std::runtime_error("Forecasts should be numeric");

    // Create wrappers around SEXP types
    NumericVector data = sx_data;

    // Get data dimensions
    if (!data.hasAttribute("dim")) throw std::runtime_error("Dimensions missing for forecasts");

    IntegerVector data_dims = data.attr("dim");

    // Check dimension length
    if (data_dims.size() != 4) throw std::runtime_error(
            "Forecasts should be 4-dimensional [Parameters, Stations, Times, FLTs]");

    try {

        // Create parameters
        FunctionsR::toParameters(sx_weights, sx_circulars, parameters_, data_dims[0]);

        // Create stations
        FunctionsR::createStations(stations_, data_dims[1]);

        // Create times
        FunctionsR::toTimes(sx_times, times_);

        // Create flts
        FunctionsR::toTimes(sx_flts, flts_);

    } catch (std::exception & ex) {
        std::string msg = std::string("ForecastsR -> ") + ex.what();
        throw std::runtime_error(msg);
    }

    if (times_.size() != numeric_cast<size_t>(data_dims[2])) {
        std::ostringstream msg;
        msg << "Third dimension of forecasts (" << data_dims[2]
                << ") != #forecast times (" << times_.size() << ")";
        throw std::runtime_error(msg.str());
    }

    if (flts_.size() != numeric_cast<size_t>(data_dims[3])) {
        std::ostringstream msg;
        msg << "Fourth dimension of forecasts (" << data_dims[3]
                << ") != #unique flts (" << flts_.size() << ")";
        throw std::runtime_error(msg.str());
    }

    // Assign data
    data_ = REAL(data);

    // Assign dimensions
    dims_[_DIM_PARAMETER] = numeric_cast<size_t>(data_dims[_DIM_PARAMETER]);
    dims_[_DIM_STATION] = numeric_cast<size_t>(data_dims[_DIM_STATION]);
    dims_[_DIM_TIME] = numeric_cast<size_t>(data_dims[_DIM_TIME]);
    dims_[_DIM_FLT] = numeric_cast<size_t>(data_dims[_DIM_FLT]);

    allocated_ = false;
}

ForecastsR::~ForecastsR() {
}

void
ForecastsR::print(std::ostream & os) const {

    os << "[Forecasts] size: [" <<
            parameters_.size() << ", " <<
            stations_.size() << ", " <<
            times_.size() << ", " <<
            flts_.size() << "]" << std::endl;

    // Print out all parameters to display circular variables and weights
    os << parameters_;

    return;
}
