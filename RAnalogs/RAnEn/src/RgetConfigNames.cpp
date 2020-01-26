/*
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on August 15, 2018
 *
 * RprintConfig.cpp prints a preview of the configuration list.
 */

// [[Rcpp::plugins(cpp11)]]

#include <Rcpp.h>
#include "ConfigNames.h"

using namespace Rcpp;

//' @export
// [[Rcpp::export]]

SEXP getConfigNames() {
    List config;

    config[_OBS] = ConfigNames::_OBS;
    config[_OBS_TIMES] = ConfigNames::_OBS_TIMES;
    config[_WEIGHTS] = ConfigNames::_WEIGHTS;
    config[_CIRCULARS] = ConfigNames::_CIRCULARS;
    config[_FCST_TIMES] = ConfigNames::_FCST_TIMES;
    config[_FLTS] = ConfigNames::_FLTS;
    config[_FCSTS] = ConfigNames::_FCSTS;
    config[_TEST_TIMES] = ConfigNames::_TEST_TIMES;
    config[_SEARCH_TIMES] = ConfigNames::_SEARCH_TIMES;
    config[_VERBOSE] = ConfigNames::_VERBOSE;
    config[_OBS_ID] = ConfigNames::_OBS_ID;
    config[_NUM_MEMBERS] = ConfigNames::_NUM_MEMBERS;
    config[_NUM_SIMS] = ConfigNames::_NUM_SIMS;
    config[_NUM_PAR_NA] = ConfigNames::_NUM_PAR_NA;
    config[_NUM_FLT_NA] = ConfigNames::_NUM_FLT_NA;
    config[_FLT_RADIUS] = ConfigNames::_FLT_RADIUS;
    config[_SAVE_SIMS] = ConfigNames::_SAVE_SIMS;
    config[_SAVE_SIMS_IND] = ConfigNames::_SAVE_SIMS_IND;
    config[_SAVE_ANALOGS_IND] = ConfigNames::_SAVE_ANALOGS_IND;
    config[_OPERATION] = ConfigNames::_OPERATION;
    config[_PREVENT_SEARCH_FUTURE] = ConfigNames::_PREVENT_SEARCH_FUTURE;
    config[_QUICK] = ConfigNames::_QUICK;
    config[_VERSION] = ConfigNames::_VERSION;

    return config;
}
