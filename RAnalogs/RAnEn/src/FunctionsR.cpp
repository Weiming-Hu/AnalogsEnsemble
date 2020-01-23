/* 
 * File:   FunctionsR.cpp
 * Author: Weiming Hu <cervone@psu.edu>
 *
 * Created on January 22, 2020, 3:53 PM
 */

#include "FunctionsR.h"
#include "boost/numeric/conversion/cast.hpp"

using namespace Rcpp;

void
FunctionsR::createParameters(Parameters & parameters, size_t total) {

    parameters.clear();
    parameters.resize(total);

    for (size_t i = 0; i < total; ++i) {
        parameters.push_back(Parameters::value_type(i, Parameter(std::to_string(i))));
    }

    return;
}

void
FunctionsR::createStations(Stations& stations, size_t total) {

    stations.clear();
    stations.reserve(total);

    for (size_t i = 0; i < total; ++i) {
        stations.push_back(Stations::value_type(i, Station(i, i)));
    }
    
    return;
}

void
FunctionsR::toParameters(const SEXP & sx_weights,
        const SEXP & sx_circulars, Parameters & parameters) {

    // Type check
    if (!Rf_isNumeric(sx_weights)) throw std::runtime_error("Weights should be numeric");
    if (!Rf_isNumeric(sx_circulars)) throw std::runtime_error("Circulars should be numeric");

    // Create pointer wrapper
    NumericVector weights = sx_weights;
    IntegerVector circulars = sx_circulars;
    
    auto num_parameters = weights.size();

    // Allocate memory
    parameters.clear();
    parameters.reserve(num_parameters);

    const auto & it_begin = circulars.begin();
    const auto & it_end = circulars.end();

    for (R_xlen_t i = 0; i < num_parameters; ++i) {
        Parameter parameter(std::to_string(i), weights[i]);

        // NOTICE the increment on the index to convert C index to R index
        auto it = std::find(it_begin, it_end, i + 1);
        if (it != it_end) parameter.setCircular(true);

        parameters.push_back(Parameters::value_type(i, parameter));
    }
    
    return;
}

void
FunctionsR::toTimes(const SEXP & sx_times, Times & times) {

    // Type check
    if (!Rf_isNumeric(sx_times)) throw std::runtime_error("Times should be numeric");

    // Create pointer wrapper
    NumericVector nv_times = sx_times;

    auto num_times = nv_times.size();

    // Allocate memory
    times.clear();
    times.reserve(num_times);

    // Copy values
    for (R_xlen_t i = 0; i < num_times; ++i) {
        if (nv_times[i] < 0) throw std::runtime_error("Timestamps cannot be negative!");
        times.push_back(Times::value_type(i, Time(nv_times[i])));
    }

    // Check uniqueness
    if (times.size() != num_times) throw std::runtime_error("Duplicated times found");

    return;
}

void
FunctionsR::setElement(Rcpp::List list, const std::string & name, const Array4D & arr) {
    
    // TODO: Check the ordering of the array
    
    // TODO: Any other better solution to avoid value copyting?
    
    using namespace boost;
    
    size_t num_dims = arr.num_dimensions();
    
    // Create dimension vector
    IntegerVector arr_dims(num_dims);
    for (size_t i = 0; i < num_dims; ++i) {
        arr_dims[i] = numeric_cast<int>(arr.shape()[i]);
    }

    // Value copy
    NumericVector nv_arr(arr.data(), arr.data() + arr.num_elements());
    nv_arr.attr("dim") = arr_dims;

    list[name] = nv_arr;
    return;
}