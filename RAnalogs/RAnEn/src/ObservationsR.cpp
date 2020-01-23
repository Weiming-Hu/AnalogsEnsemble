/* 
 * File:   ObservationsR.cpp
 * Author: Weiming Hu <cervone@psu.edu>
 * 
 * Created on January 21, 2020, 3:37 PM
 */

#include "ObservationsR.h"
#include "FunctionsR.h"

#include <sstream>
#include <stdexcept>

using namespace Rcpp;

ObservationsR::ObservationsR() {
}

ObservationsR::ObservationsR(const ObservationsR& orig) : Observations(orig) {
    offset_ = orig.offset_;
    data_ = orig.data_;
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
    
    if (times_.size() != data_dims[2]) {
        std::ostringstream msg;
        msg << "Third dimensions of observations (" << data_dims[2]
                << ") != #unique observation times (" << times_.size() << ")";
        throw std::runtime_error(msg.str());
    }

    // Assign data
    data_ = data;
    offset_ = Offset(data_dims);
}

ObservationsR::~ObservationsR() {
}

size_t ObservationsR::num_elements() const {
    return data_.size();
}

const double* ObservationsR::getValuesPtr() const {
    return REAL(data_);
}

double* ObservationsR::getValuesPtr() {
    return REAL(data_);
}

void ObservationsR::setDimensions(
        const Parameters& parameters,
        const Stations& stations,
        const Times& times) {

    // Set members in the parent class
    parameters_ = parameters;
    stations_ = stations;
    times_ = times;

    // Allocate memory for underlying data structure
    // using the C++ idioms
    //
    size_t size = parameters_.size() * stations_.size() * times_.size();

    // NumericVector has no resize method. Therefore, 
    // create a new vector and re-assign.
    //
    NumericVector tmp(size);
    data_ = tmp;

    return;
}

double ObservationsR::getValue(
        size_t parameter_index, size_t station_index, size_t time_index) const {

    // Type cast from size_t to int for IntegerVector
    IntegerVector indices{
        (int) parameter_index, (int) station_index, (int) time_index};

    return data_[offset_(indices)];
}

void ObservationsR::setValue(double val,
        size_t parameter_index, size_t station_index, size_t time_index) {

    // Type cast from size_t to int for IntegerVector
    IntegerVector indices{
        (int) parameter_index, (int) station_index, (int) time_index};

    data_[offset_(indices)] = val;
    return;
}
