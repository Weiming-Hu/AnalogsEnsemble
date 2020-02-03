/* 
 * File:   RcppFunctions.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on January 22, 2020, 3:53 PM
 */

#include "RcppFunctions.h"
#include "boost/numeric/conversion/cast.hpp"

#include <sstream>

using namespace Rcpp;
using namespace boost;

void
FunctionsR::createParameters(Parameters & parameters, size_t total) {

    parameters.clear();

    for (size_t i = 0; i < total; ++i) {
        parameters.push_back(Parameters::value_type(i, Parameter(std::to_string(i))));
    }

    return;
}

void
FunctionsR::createStations(Stations& stations, size_t total) {

    stations.clear();

    for (size_t i = 0; i < total; ++i) {
        stations.push_back(Stations::value_type(i, Station(i, i)));
    }

    return;
}

void
FunctionsR::createTimes(Times& times, size_t total) {
    
    times.clear();
    
    for (size_t i = 0; i < total; ++i) {
        times.push_back(Times::value_type(i, Time(i)));
    }
    
    return;
}

void
FunctionsR::toParameters(const SEXP & sx_names, const SEXP & sx_circulars, Parameters & parameters) {

    // Type check
    if (!Rf_isString(sx_names)) throw std::runtime_error("Parameter names should be strings");
    if (!Rf_isNumeric(sx_circulars)) throw std::runtime_error("Circulars should be numeric");

    // Create pointer wrapper
    IntegerVector circulars = sx_circulars;
    StringVector names = sx_names;
    
    // Clean parameters
    parameters.clear();

    const auto & it_begin = circulars.begin();
    const auto & it_end = circulars.end();
    auto num_parameters = names.size();

    for (R_xlen_t i = 0; i < num_parameters; ++i) {
        Parameter parameter(as<std::string>(names(i)));

        // NOTICE the increment on the index to convert C index to R index
        auto it = std::find(it_begin, it_end, i + 1);
        if (it != it_end) parameter.setCircular(true);

        parameters.push_back(Parameters::value_type(i, parameter));
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
        stations.push_back(Stations::value_type(i, station));
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
        if (numeric_cast<size_t>(nv_times[i]) != nv_times[i]) throw std::runtime_error("Decimal numbers not allowed for times");
        times.push_back(Times::value_type(i, Time(nv_times[i])));
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
FunctionsR::setElement(Rcpp::List & list, const std::string & name, const Array4D & arr) {

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

    list[name] = nv_arr;
    return;
}

FunctionsR::ConfigMode
FunctionsR::checkConfig(const SEXP & sx_config) {

    using namespace std;

//    if (!Rf_isNewList(sx_config)) throw std::runtime_error("Configuration should be a list");
//    
//    List config = sx_config;
//
//    // Members that should be numeric
//    vector<string> members_numeric{
//        _OBS_TIMES, _WEIGHTS, _CIRCULARS, _FCST_TIMES, _FLTS, _TEST_TIMES,
//        _SEARCH_TIMES, _VERBOSE, _OBS_ID, _NUM_MEMBERS, _NUM_SIMS,
//        _NUM_PAR_NA, _NUM_FLT_NA, _FLT_RADIUS};
//
//    checkType(config, members_numeric, Rf_isNumeric, "numeric");
//
//    // Members that should be logic
//    vector<string> members_logic{
//        _SAVE_SIMS, _SAVE_SIMS_IND, _SAVE_ANALOGS_IND,
//        _OPERATION, _PREVENT_SEARCH_FUTURE, _QUICK};
//
//    checkType(config, members_logic, Rf_isLogical, "logic");
//
//    // Members that are used to determine the ConfigMode
//    if (Rf_isNumeric(config[_OBS]) && Rf_isNumeric(config[_FCSTS])) {
//        // Forecasts and observations are numeric vectors that
//        // have already been read into memory
//        //
//        return ConfigMode::DATA_IN_MEMORY;
//    }
//
//    throw runtime_error("Configuration is not recognized");
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
