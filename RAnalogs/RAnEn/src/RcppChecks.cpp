/*
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on August 15, 2018
 *
 */

// [[Rcpp::plugins(cpp11)]]
// [[Rcpp::plugins(openmp)]]

#include <Rcpp.h>
#include "RcppFunctions.h"

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

