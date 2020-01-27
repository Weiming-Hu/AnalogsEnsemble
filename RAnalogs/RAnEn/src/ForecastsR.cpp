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

using namespace Rcpp;
using namespace boost;

ForecastsR::ForecastsR() {
    data_ = nullptr;
    internal_ = false;
}

ForecastsR::ForecastsR(const ForecastsR& orig) : Forecasts(orig) {
    offset_ = orig.offset_;
    data_ = orig.data_;
    
    // Data will managed by other objects. The data pointer is not
    // internal to this object.
    //
    internal_ = false;
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
    offset_ = Offset(data_dims);
    internal_ = false;
}

ForecastsR::~ForecastsR() {
    if (internal_) delete [] data_;
}

size_t ForecastsR::num_elements() const {
    return offset_.num_elements();
}

const double* ForecastsR::getValuesPtr() const {
    return data_;
}

double* ForecastsR::getValuesPtr() {
    return data_;
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

    if (internal_) delete [] data_;
    data_ = new double [size];
    internal_ = true;

    return;
}

double ForecastsR::getValue(
        size_t parameter_index, size_t station_index,
        size_t time_index, size_t flt_index) const {

    std::vector<size_t> indices{parameter_index, station_index, time_index, flt_index};
    return data_[offset_(indices)];
}

void ForecastsR::setValue(double val,
        size_t parameter_index, size_t station_index,
        size_t time_index, size_t flt_index) {

    std::vector<size_t> indices{parameter_index, station_index, time_index, flt_index};
    data_[offset_(indices)] = val;
    return;
}

void ForecastsR::print(std::ostream & os) const {
    Forecasts::print(os);

    size_t count = offset_.num_elements();
    os << "[Data] size: " << count << std::endl;

    if (count > AnEnDefaults::_PREVIEW_COUNT) {
        os << Functions::format(data_, AnEnDefaults::_PREVIEW_COUNT, ",") << ", ...";
    } else {
        os << Functions::format(data_, count, ",");
    }
    os << std::endl;

    return;
}

std::ostream &
operator<<(std::ostream & os, const ForecastsR & obj) {
    obj.print(os);
    return os;
} 
