/* 
 * File:   ForecastsR.cpp
 * Author: Weiming Hu <cervone@psu.edu>
 * 
 * Created on January 21, 2020, 3:36 PM
 */

#include "ForecastsR.h"
#include "FunctionsR.h"
#include <sstream>

using namespace Rcpp;

ForecastsR::ForecastsR() {
}

ForecastsR::ForecastsR(const ForecastsR& orig) : Forecasts(orig) {
    offset_ = orig.offset_;
    data_ = orig.data_;
}

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
        FunctionsR::toParameters(sx_weights, sx_circulars, parameters_);

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
    
    if (parameters_.size() != data_dims[0]) {
        std::ostringstream msg;
        msg << "First dimension of forecasts (" << data_dims[0]
                << ") != #weights (" << parameters_.size() << ")";
        throw std::runtime_error(msg.str());
    }

    if (times_.size() != data_dims[2]) {
        std::ostringstream msg;
        msg << "Third dimension of forecasts (" << data_dims[2]
                << ") != #forecast times (" << times_.size() << ")";
        throw std::runtime_error(msg.str());
    }
    
    if (flts_.size() != data_dims[3]) {
        std::ostringstream msg;
        msg << "Fourth dimension of forecasts (" << data_dims[3]
                << ") != #unique flts (" << flts_.size() << ")";
        throw std::runtime_error(msg.str());
    }

    // Assign data
    data_ = data;
    offset_ = Offset(data_dims);
}

ForecastsR::~ForecastsR() {
}

size_t ForecastsR::num_elements() const {
    return data_.size();
}

const double* ForecastsR::getValuesPtr() const {
    return REAL(data_);
}

double* ForecastsR::getValuesPtr() {
    return REAL(data_);
}

void ForecastsR::setDimensions(
        const Parameters& parameters,
        const Stations& stations,
        const Times& times, const Times& flts) {

    // Set members in the parent class
    parameters_ = parameters;
    stations_ = stations;
    times_ = times;
    flts_ = flts;

    // Allocate memory for underlying data structure
    // using the C++ idioms
    //
    size_t size = parameters_.size() * stations_.size() *
            times_.size() * flts_.size();

    // NumericVector has no resize method. Therefore, 
    // create a new vector and re-assign.
    //
    NumericVector tmp(size);
    data_ = tmp;

    return;
}

double ForecastsR::getValue(
        size_t parameter_index, size_t station_index,
        size_t time_index, size_t flt_index) const {

    // Type cast from size_t to int for IntegerVector
    IntegerVector indices{
        (int) parameter_index, (int) station_index,
        (int) time_index, (int) flt_index};

    return data_[offset_(indices)];
}

void ForecastsR::setValue(double val,
        size_t parameter_index, size_t station_index,
        size_t time_index, size_t flt_index) {
    
    // Type cast from size_t to int for IntegerVector
    IntegerVector indices{
        (int) parameter_index, (int) station_index,
        (int) time_index, (int) flt_index};

    data_[offset_(indices)] = val;
    return;
}
