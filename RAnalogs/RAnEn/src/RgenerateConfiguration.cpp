/*
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on August 15, 2018
 *
 * RgenerateConfiguration.cpp exports all default values and configuration
 * names into R environment.
 */

// [[Rcpp::plugins(cpp11)]]

#include <Rcpp.h>

#include "ConfigNames.h"
#include "AnEnDefaults.h"

using namespace Rcpp;

//' @export
// [[Rcpp::export]]

SEXP generateConfiguration() {

    List list;
    list[ConfigNames::_OBS] = R_NilValue;
    list[ConfigNames::_OBS_TIMES] = R_NilValue;
    list[ConfigNames::_WEIGHTS] = NumericVector();
    list[ConfigNames::_CIRCULARS] = NumericVector();
    list[ConfigNames::_FCST_TIMES] = R_NilValue;
    list[ConfigNames::_FLTS] = R_NilValue;
    list[ConfigNames::_FCSTS] = R_NilValue;
    list[ConfigNames::_TEST_TIMES] = R_NilValue;
    list[ConfigNames::_SEARCH_TIMES] = R_NilValue;
    list[ConfigNames::_VERBOSE] = AnEnDefaults::_VERBOSE_INT;
    list[ConfigNames::_OBS_ID] = AnEnDefaults::_OBS_VAR_INDEX + 1;
    list[ConfigNames::_NUM_MEMBERS] = AnEnDefaults::_NUM_ANALOGS;
    list[ConfigNames::_NUM_SIMS] = AnEnDefaults::_NUM_SIMS;
    list[ConfigNames::_NUM_PAR_NA] = AnEnDefaults::_MAX_PAR_NAN;
    list[ConfigNames::_NUM_FLT_NA] = AnEnDefaults::_MAX_FLT_NAN;
    list[ConfigNames::_FLT_RADIUS] = AnEnDefaults::_FLT_RADIUS;
    list[ConfigNames::_SAVE_SIMS] = AnEnDefaults::_SAVE_SIMS;
    list[ConfigNames::_SAVE_SIMS_IND] = AnEnDefaults::_SAVE_SIMS_INDEX;
    list[ConfigNames::_SAVE_ANALOGS_IND] = AnEnDefaults::_SAVE_ANALOGS_INDEX;
    list[ConfigNames::_OPERATION] = AnEnDefaults::_OPERATIONAL;
    list[ConfigNames::_PREVENT_SEARCH_FUTURE] = AnEnDefaults::_PREVENT_SEARCH_FUTURE;
    list[ConfigNames::_QUICK] = AnEnDefaults::_QUICK_SORT;

    list.attr("class") = "Configuration";
    return (list);
}
