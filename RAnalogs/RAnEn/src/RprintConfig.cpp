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
#include "FunctionsR.h"

using namespace Rcpp;

//' @export
// [[Rcpp::export(print.Configuration)]]

void printConfig(SEXP sx_config) {

    // Type checks
    FunctionsR::checkConfig(sx_config);

    // Create list wrapper
    List config = sx_config;

    Rcout << "----- AnEn Configuration Summary -----"
            // General arguments
            << ConfigNames::_OBS_ID << ": " << config[ConfigNames::_OBS_ID]
            << ConfigNames::_NUM_MEMBERS << ": " << config[ConfigNames::_NUM_MEMBERS]
            << ConfigNames::_TEST_TIMES << ": " << config[ConfigNames::_TEST_TIMES]
            << ConfigNames::_SEARCH_TIMES << ": " << config[ConfigNames::_SEARCH_TIMES]
            << ConfigNames::_QUICK << ": " << config[ConfigNames::_QUICK]
            << ConfigNames::_VERBOSE << ": " << config[ConfigNames::_VERBOSE]

            // Forecasts
            << ConfigNames::_WEIGHTS << ": " << config[ConfigNames::_WEIGHTS]
            << ConfigNames::_CIRCULARS << ": " << config[ConfigNames::_CIRCULARS]
            << ConfigNames::_FCST_TIMES << ": " << config[ConfigNames::_FCST_TIMES]
            << ConfigNames::_FLTS << ": " << config[ConfigNames::_FLTS]
            << ConfigNames::_FCSTS << ": " << config[ConfigNames::_FCSTS]

            // Observations
            << ConfigNames::_OBS << ": " << config[ConfigNames::_OBS]
            << ConfigNames::_OBS_TIMES << ": " << config[ConfigNames::_OBS_TIMES]

            // Extra arguments
            << ConfigNames::_NUM_SIMS << ": " << config[ConfigNames::_NUM_SIMS]
            << ConfigNames::_NUM_PAR_NA << ": " << config[ConfigNames::_NUM_PAR_NA]
            << ConfigNames::_NUM_FLT_NA << ": " << config[ConfigNames::_NUM_FLT_NA]
            << ConfigNames::_FLT_RADIUS << ": " << config[ConfigNames::_FLT_RADIUS]
            << ConfigNames::_SAVE_SIMS << ": " << config[ConfigNames::_SAVE_SIMS]
            << ConfigNames::_SAVE_SIMS_IND << ": " << config[ConfigNames::_SAVE_SIMS_IND]
            << ConfigNames::_SAVE_ANALOGS_IND << ": " << config[ConfigNames::_SAVE_ANALOGS_IND]
            << ConfigNames::_OPERATION << ": " << config[ConfigNames::_OPERATION]
            << ConfigNames::_CHECK_SEARCH_FUTURE << ": " << config[ConfigNames::_CHECK_SEARCH_FUTURE]
            << std::endl;

    return;
}
