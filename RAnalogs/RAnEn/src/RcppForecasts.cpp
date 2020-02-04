/* 
 * File:   RcppForecasts.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 * 
 * Created on January 21, 2020, 3:36 PM
 */

#include "RcppForecasts.h"
#include "RcppFunctions.h"
#include "Functions.h"
#include "boost/numeric/conversion/cast.hpp"

#include <sstream>
#include <stdexcept>

using namespace boost;
using namespace Rcpp;

ForecastsR::ForecastsR(
        SEXP sx_data, SEXP sx_parameters_name, SEXP sx_circulars,
        SEXP sx_xs, SEXP sx_ys, SEXP sx_stations_name, SEXP sx_times, SEXP sx_flts) {

    // Type checks
    if (!Rf_isNumeric(sx_data)) throw std::runtime_error("Forecasts data should be numeric");

    // Create wrappers around SEXP types
    NumericVector data = sx_data;

    // Get data dimensions
    if (!data.hasAttribute("dim")) throw std::runtime_error("Dimensions missing for forecasts");

    IntegerVector data_dims = data.attr("dim");

    // Check dimension length
    if (data_dims.size() != 4) throw std::runtime_error(
            "Forecasts data array should be 4-dimensional [Parameters, Stations, Times, FLTs]");

    try {

        // Create parameters
        FunctionsR::toParameters(sx_parameters_name, sx_circulars, parameters_);

        // Create stations
        FunctionsR::toStations(sx_xs, sx_ys, sx_stations_name, stations_);

        // Create times
        FunctionsR::toTimes(sx_times, times_);

        // Create flts
        FunctionsR::toTimes(sx_flts, flts_);

    } catch (std::exception & ex) {
        std::string msg = std::string("Forecasts -> ") + ex.what();
        throw std::runtime_error(msg);
    }

    if (parameters_.size() != numeric_cast<size_t>(data_dims[0])) {
        std::ostringstream msg;
        msg << "First dimension of forecast data array (" << data_dims[0]
                << ") != #parameters (" << parameters_.size() << ")";
        throw std::runtime_error(msg.str());
    }

    if (stations_.size() != numeric_cast<size_t>(data_dims[1])) {
        std::ostringstream msg;
        msg << "Second dimension of forecast data array (" << data_dims[1]
                << ") != #stations (" << stations_.size() << ")";
        throw std::runtime_error(msg.str());
    }

    if (times_.size() != numeric_cast<size_t>(data_dims[2])) {
        std::ostringstream msg;
        msg << "Third dimension of forecast data array (" << data_dims[2]
                << ") != #forecast times (" << times_.size() << ")";
        throw std::runtime_error(msg.str());
    }

    if (flts_.size() != numeric_cast<size_t>(data_dims[3])) {
        std::ostringstream msg;
        msg << "Fourth dimension of forecast data array (" << data_dims[3]
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
ForecastsR::show() const {

    Rcout << "[Forecasts] size: [" <<
            parameters_.size() << ", " <<
            stations_.size() << ", " <<
            times_.size() << ", " <<
            flts_.size() << "]" << std::endl;

    // Print out all parameters
    Rcout << parameters_;

    return;
}
