/* 
 * File:   RcppFunctions.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on January 22, 2020, 3:53 PM
 */
#include <cmath>
#include <sstream>

#include "RcppFunctions.h"
#include "boost/numeric/conversion/cast.hpp"

using namespace Rcpp;
using namespace boost;

void
FunctionsR::createParameters(const SEXP & sx_circulars, Parameters & parameters, size_t total) {

    bool has_circulars = false;
    if (!Rf_isNull(sx_circulars)) {
        if (Rf_isNumeric(sx_circulars)) has_circulars = true;
        else throw std::runtime_error("Circulars should be numeric when parameter names are not provided");
    }

    // Create pointer wrapper
    NumericVector circulars;
    if (has_circulars) circulars = sx_circulars;

    parameters.clear();

    for (size_t i = 0; i < total; ++i) {

        Parameter parameter(std::to_string(i));

        if (has_circulars) {
            // Note the i + 1 to convert from C index to R index
            auto it = std::find(circulars.begin(), circulars.end(), i + 1);
            if (it != circulars.end()) parameter.setCircular(true);
        }

        parameters.push_back(parameter);
    }

    return;
}

void
FunctionsR::createStations(const SEXP & sx_names, Stations& stations, size_t total) {

    bool has_name = false;
    if (Rf_isString(sx_names)) has_name = true;

    StringVector names;
    if (has_name) {

        names = sx_names;

        if (names.size() != total) {
            throw std::runtime_error("The number of station names does not match the requested data.");
        }
    }

    stations.clear();
    std::string name = Config::_NAME;

    for (size_t i = 0; i < total; ++i) {
        if (has_name) {
            name = names(i);
        }

        stations.push_back(Station(i, i, name));
    }

    return;
}

void
FunctionsR::toParameters(const SEXP & sx_names, const SEXP & sx_circulars, Parameters & parameters) {

    // Type check
    if (!Rf_isString(sx_names)) throw std::runtime_error("Parameter names should be strings");

    bool has_circulars = false;
    if (!Rf_isNull(sx_circulars)) {
        if (Rf_isString(sx_circulars)) has_circulars = true;
        else throw std::runtime_error("Circulars should be strings when parameter names are provided");
    }

    // Create pointer wrapper
    StringVector names = sx_names;

    StringVector circulars;
    if (has_circulars) circulars = sx_circulars;

    // Clean parameters
    parameters.clear();

    auto num_parameters = names.size();

    for (R_xlen_t i = 0; i < num_parameters; ++i) {
        std::string parameter_name = as<std::string>(names(i));
        Parameter parameter(parameter_name);

        if (has_circulars) {
            auto it = std::find(circulars.begin(), circulars.end(), parameter_name);
            if (it != circulars.end()) parameter.setCircular(true);
        }

        parameters.push_back(parameter);
    }

    return;
}

void
FunctionsR::toStations(const SEXP & sx_xs, const SEXP & sx_ys, const SEXP & sx_names, Stations & stations) {

    bool has_name = false;
    if (Rf_isString(sx_names)) has_name = true;

    if (!Rf_isNumeric(sx_xs)) throw std::runtime_error("Xs should be numeric");
    if (!Rf_isNumeric(sx_ys)) throw std::runtime_error("Ys should be numeric");

    // Create pointer wrapper
    NumericVector xs = sx_xs;
    NumericVector ys = sx_ys;

    StringVector names;
    if (has_name) names = sx_names;

    if (ys.size() != xs.size()) {
        throw std::runtime_error("The number of x does not match the number of y");
    }

    if (has_name) {
        if (names.size() != xs.size()) {
            throw std::runtime_error("The number of stations names does not match the number of coordinates.");
        }
    }

    // Clean stations
    stations.clear();

    auto num_stations = xs.size();
    std::string name = Config::_NAME;

    for (R_xlen_t i = 0; i < num_stations; ++i) {

        if (has_name) {
            name = names(i);
        }

        Station station(xs[i], ys[i], name);
        stations.push_back(station);
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

    // Copy values
    for (R_xlen_t i = 0; i < num_times; ++i) {
        if (nv_times[i] < 0) throw std::runtime_error("Timestamps cannot be negative!");

        if (numeric_cast<size_t>(nv_times[i]) != nv_times[i]) {
            std::stringstream msg;
            msg.precision(18);
            msg << "Decimal numbers not allowed for times. I got " << nv_times[i];
            throw std::runtime_error(msg.str());
        }

        times.push_back(nv_times[i]);
    }

    // Check uniqueness
    if (numeric_cast<R_xlen_t>(times.size()) != num_times) {
        std::stringstream ss;
        ss << "Duplicated times found: #input (" << num_times
            << ") #unique (" << times.size() << ")";
        throw std::runtime_error(ss.str());
    }

    return;
}

void
FunctionsR::setElement(Rcpp::List & list, const std::string & name,
        const Array4D & arr, bool index_conversion) {

    using namespace boost;

    // The input is a 4-dimensional array
    size_t num_dims = 4;

    // Create dimension vector
    IntegerVector arr_dims(num_dims);
    for (size_t i = 0; i < num_dims; ++i) {
        arr_dims[i] = numeric_cast<int>(arr.shape()[i]);
    }

    // Value copy
    NumericVector nv_arr(arr.getValuesPtr(), arr.getValuesPtr() + arr.num_elements());
    nv_arr.attr("dim") = arr_dims;

    // If the matrix stores indices, we need to add 1 to convert from C indices to R indices
    if (index_conversion) nv_arr = nv_arr + 1;

    list[name] = nv_arr;
    return;
}

void
FunctionsR::setElement(Rcpp::List & list, const std::string & name,
        const Functions::Matrix & mat, bool index_conversion) {

    // Create a numeric matrix for R
    NumericMatrix nm_mat(mat.size1(), mat.size2());

    // Initialize with NA
    std::fill(nm_mat.begin(), nm_mat.end(), NAN);

    // Assign values
    // 
    // I copy ans assign values with nested for loops rather than pointer copy because
    // 
    // - matrices are usually not as big as multi-dimensional array, so the slowdown in performance
    // should be negligible.
    // 
    // - I can avoid worrying about whether the internal storage of matrix is by column or by row.
    // 
    for (size_t row_i = 0; row_i < mat.size1(); ++row_i) {
        for (size_t col_i = 0; col_i < mat.size2(); ++col_i) {

            // If the matrix stores indices, we need to add 1 to convert from C indices to R indices
            if (index_conversion) nm_mat(row_i, col_i) = mat(row_i, col_i) + 1;
            else nm_mat(row_i, col_i) = mat(row_i, col_i);

        }
    }

    list[name] = nm_mat;
    return;
}

void
FunctionsR::checkType(const List & config, std::vector<std::string> names,
        Rboolean (typeFunc)(SEXP), const std::string type_name) {

    std::stringstream msg;

    for (const auto & name : names) {
        if (!typeFunc(config[name])) {
            msg << name << " is not " << type_name << std::endl;
        }
    }

    std::string msg_str = msg.str();
    if (!msg_str.empty()) throw std::runtime_error(msg_str);

    return;
}
