/*
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on August 15, 2018
 *
 */

// [[Rcpp::plugins(cpp11)]]
// [[Rcpp::plugins(openmp)]]

#include <Rcpp.h>
#include "FunctionsR.h"

using namespace Rcpp;

#if defined(_OPENMP)
#include <omp.h>
#endif

//' @export
// [[Rcpp::export]]
bool checkOpenMP() {
#ifndef _OPENMP
    return false;
#else
    return true;
#endif
}

//' @export
// [[Rcpp::export]]
void validateConfiguration(const SEXP & sx_config) {
    FunctionsR::checkConfig(sx_config);
    return;
}