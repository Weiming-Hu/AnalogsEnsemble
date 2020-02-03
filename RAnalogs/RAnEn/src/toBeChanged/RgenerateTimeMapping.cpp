/*
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on August 15, 2018
 *
 * RcomputeAnEnIS.cpp is the source file with the interface definition for
 * R functions to call AnEnIS class.
 */

// [[Rcpp::plugins(cpp11)]]
// [[Rcpp::plugins(openmp)]]

#include <Rcpp.h>
#include <vector>
#include <numeric>
#include <stdexcept>

#include "FunctionsR.h"
#include "Functions.h"
#include "Times.h"

using namespace Rcpp;

//' @export
// [[Rcpp::export]]

NumericMatrix generateTimeMapping(SEXP sx_fcst_times, SEXP sx_fcst_flts, SEXP sx_obs_times) {

    // Type checks
    if (!Rf_isNumeric(sx_fcst_times)) throw std::runtime_error("Forecast times should be numeric");
    if (!Rf_isNumeric(sx_fcst_flts)) throw std::runtime_error("Forecast lead times should be numeric");
    if (!Rf_isNumeric(sx_obs_times)) throw std::runtime_error("Observation times should be numeric");

    // Convert SEXP to Times
    Times fcst_times, fcst_flts, obs_times;
    FunctionsR::toTimes(sx_fcst_times, fcst_times);
    FunctionsR::toTimes(sx_fcst_flts, fcst_flts);
    FunctionsR::toTimes(sx_obs_times, obs_times);

    // Specify what the search times are. In this particular case, all times
    // in forecast times are search times.
    //
    std::vector<std::size_t> fcsts_search_index(fcst_times.size());
    std::iota(fcsts_search_index.begin(), fcsts_search_index.end(), 0);

    // Calculate the mapping from forecasts times to observation times
    Functions::Matrix obs_index_table = Functions::Matrix(
            fcst_times.size(), fcst_flts.size(), NAN);
    Functions::updateTimeTable(fcst_times,
            fcsts_search_index, fcst_flts, obs_times, obs_index_table);

    // Assign values
    //
    // Notice the swap of dimensions because Functions::Matrix is by rows and
    // NumericMatrix is by columns. So swap the dimensions here so that the
    // NumericMatrix has [FLTs, forecast times] when Functions::Matrix has
    // [forecast times, FLTs].
    //
    NumericMatrix matrix(obs_index_table.size2(), obs_index_table.size1());
    matrix.assign(obs_index_table.data().begin(), obs_index_table.data().end());
    
    // Convert from C index (counting from 0) to R index (counting from 1) 
    matrix = matrix + 1;

    return matrix;
}
