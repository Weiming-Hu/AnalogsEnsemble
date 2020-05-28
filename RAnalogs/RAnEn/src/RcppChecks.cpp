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

//' @export
// [[Rcpp::export]]
void setNumThreads(int threads) {
#ifndef _OPENMP
    Rcpp::Rcout << "Nothing happened because OpenMP is not supported." << std::endl;
#else
    omp_set_num_threads(threads);
#endif
  return;
}

//' @export
// [[Rcpp::export]]
int getNumThreads() {
#ifndef _OPENMP
    Rcpp::Rcout << "Only using a single thread because OpenMP is not supported." << std::endl;
    return 1;
#else
    return omp_get_num_procs();
#endif
}

