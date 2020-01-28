/* 
 * File:   ForecastsR.cpp
 * Author: Weiming Hu <cervone@psu.edu>
 * 
 * Created on January 21, 2020, 3:36 PM
 */

#include "ForecastsR.h"
#include "FunctionsR.h"
#include "boost/numeric/conversion/cast.hpp"

#include <sstream>
#include <stdexcept>

using namespace Rcpp;
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

    if (numeric_cast<int>(times_.size()) != data_dims[2]) {
        std::ostringstream msg;
        msg << "Third dimension of forecasts (" << data_dims[2]
                << ") != #forecast times (" << times_.size() << ")";
        throw std::runtime_error(msg.str());
    }

    if (numeric_cast<int>(flts_.size()) != data_dims[3]) {
        std::ostringstream msg;
        msg << "Fourth dimension of forecasts (" << data_dims[3]
                << ") != #unique flts (" << flts_.size() << ")";
        throw std::runtime_error(msg.str());
    }

    // Assign data
    data_ = REAL(data);
    memcpy(dims_, data_dims.begin(), 3 * sizeof(int));
    allocated_ = false;
}

ForecastsR::~ForecastsR() {
}
