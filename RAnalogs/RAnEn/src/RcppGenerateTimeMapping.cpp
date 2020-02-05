/*
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on August 15, 2018
 */

// [[Rcpp::plugins(cpp11)]]
// [[Rcpp::plugins(openmp)]]

#include <Rcpp.h>
#include <vector>
#include <numeric>
#include <stdexcept>

#include "RcppFunctions.h"
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
    // I copy ans assign values with nested for loops rather than pointer copy because
    // 
    // - matrices are usually not as big as multi-dimensional array, so the slowdown in performance
    // should be negligible.
    // 
    // - I can avoid worrying about whether the internal storage of matrix is by column or by row.
    // 
    NumericMatrix mat(obs_index_table.size1(), obs_index_table.size2());
    for (size_t row_i = 0; row_i < mat.nrow(); ++row_i) {
      for (size_t col_i = 0; col_i < mat.ncol(); ++col_i) {
        mat(row_i, col_i) = obs_index_table(row_i, col_i) + 1;
      }
    }

    return mat;
}
