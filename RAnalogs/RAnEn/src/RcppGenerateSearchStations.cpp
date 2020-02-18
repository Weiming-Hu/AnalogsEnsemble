/*
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on August 15, 2018
 */

// [[Rcpp::plugins(cpp11)]]
// [[Rcpp::plugins(openmp)]]

#include <cmath>
#include "Config.h"
#include "RcppFunctions.h"
#include "Functions.h"

using namespace Rcpp;

//' RAnEn::generateSearchStations
//' 
//' RAnEn::generateSearchStations calculates the nearest coordinates for all input coordinates.
//' 
//' @param sx_xs X coordinates
//' @param sx_ys Y coordinates
//' @param num_neighbors The number of neighbors to find
//' @param distance The distance threshold when finding nearest neighbors
//' @return A matrix for nearest search station index. Each row is a set of search station index
//' 
//' @md
//' @export
// [[Rcpp::export]]

NumericMatrix generateSearchStations(SEXP sx_xs, SEXP sx_ys,
        int num_neighbors, SEXP sx_distance = R_NilValue) {

    // Convert distance value
    double distance;
    if (Rf_isNull(sx_distance)) {
        Config config;
        distance = config.distance;
    } else {
        distance = as<double>(sx_distance);
    }

    // Convert coordinates to stations
    Stations stations;
    FunctionsR::toStations(sx_xs, sx_ys, R_NilValue, stations);

    // Create a matrix
    Functions::Matrix c_mat(stations.size(), num_neighbors);

    // Initialize the values to NAN
    auto & storage = c_mat.data();
    fill_n(storage.begin(), storage.size(), NAN);

    // Call the C++ routine to find nearest stations
    Functions::setSearchStations(stations, c_mat, distance);

    // Copy values from C matrix to R matrix
    NumericMatrix r_mat(c_mat.size1(), c_mat.size2());

    for (size_t row_i = 0; row_i < c_mat.size1(); ++row_i) {
        for (size_t col_i = 0; col_i < c_mat.size2(); ++col_i) {
            // Notice the plus 1 because we need to convert from C index (starting at 0)
            // to R index (starting at 1)
            //
            r_mat(row_i, col_i) = c_mat(row_i, col_i) + 1;
        }
    }

    return r_mat;
}
