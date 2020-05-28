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
  omp_set_num_threads(threads);
#else
  Rcpp::Rcout << "Nothing happened because OpenMP is not supported." << std::endl;
#endif
  return;
}

//' @export
// [[Rcpp::export]]
int getNumThreads() {
#ifndef _OPENMP
  return omp_get_num_procs();
#else
  Rcpp::Rcout << "Only using a single thread because OpenMP is not supported." << std::endl;
  return 1;
#endif
}

