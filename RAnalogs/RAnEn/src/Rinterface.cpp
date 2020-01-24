/*
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on August 15, 2018
 *
 * Rinterface.cpp is the source file with the interface definition for
 * R functions to call C++ functions.
 */

// [[Rcpp::plugins(cpp11)]]
// [[Rcpp::plugins(openmp)]]

#include <Rcpp.h>
#include <stdexcept>

#include "AnEnIS.h"
#include "ConfigNames.h"
#include "ForecastsR.h"
#include "ObservationsR.h"
#include "FunctionsR.h"
#include "boost/numeric/conversion/cast.hpp"

using namespace Rcpp;
using namespace boost;

#if defined(_OPENMP)
#include <omp.h>
#endif

// [[Rcpp::export(".checkOpenMP")]]

bool checkOpenMP() {
#ifndef _OPENMP
    return false;
#else
    return true;
#endif
}

// [[Rcpp::export(".generateAnEnIS")]]

SEXP computeAnEnIS(SEXP R_config) {

    if (!Rf_isNewList(R_config)) throw std::runtime_error("A list is expected");

    // Create list wrapper
    List config = R_config;
    
    // Type checks
    FunctionsR::checkConfig(config);
    
    // Observations
    ObservationsR observations(
            config[ConfigNames::_OBS_TIMES],
            config[ConfigNames::_OBS]);

    // Forecasts
    ForecastsR forecasts(
            config[ConfigNames::_WEIGHTS],
            config[ConfigNames::_CIRCULARS],
            config[ConfigNames::_FCST_TIMES],
            config[ConfigNames::_FLTS],
            config[ConfigNames::_FCSTS]);

    // Test and search times
    Times test_times, search_times;
    
    try {
        FunctionsR::toTimes(config[ConfigNames::_TEST_TIMES], test_times);
    } catch (std::exception & ex) {
        std::string msg = std::string("toTimes(test_times) -> ") + ex.what();
        throw std::runtime_error(msg);
    }
    
    try {
        FunctionsR::toTimes(config[ConfigNames::_SEARCH_TIMES], search_times);
    } catch (std::exception & ex) {
        std::string msg = std::string("toTimes(search_times) -> ") + ex.what();
        throw std::runtime_error(msg);
    }

    // Verbose
    AnEnDefaults::Verbose verbose = Functions::itov(
            as<int>(config[ConfigNames::_VERBOSE]));

    // Observation variable index should subtract 1 to be converted from 
    // an R index to a C index
    //
    std::size_t obs_id = numeric_cast<std::size_t>(
            as<double>(config[ConfigNames::_OBS_ID]) - 1);
    
    // Safe type conversion from numeric vector to std::size_t
    std::size_t num_analogs = numeric_cast<std::size_t>(
            as<double>(config[ConfigNames::_NUM_MEMBERS]));
    std::size_t num_sims = numeric_cast<std::size_t>(
            as<double>(config[ConfigNames::_NUM_SIMS]));
    std::size_t num_par_nan = numeric_cast<std::size_t>(
            as<double>(config[ConfigNames::_NUM_PAR_NA]));
    std::size_t num_flt_nan = numeric_cast<std::size_t>(
            as<double>(config[ConfigNames::_NUM_FLT_NA]));
    std::size_t flt_radius = numeric_cast<std::size_t>(
            as<double>(config[ConfigNames::_FLT_RADIUS]));
    
    // These logical variables will be used more than once
    bool preserve_similarity = as<bool>(config[ConfigNames::_SAVE_SIMS]);
    bool preserve_similairty_index = as<bool>(config[ConfigNames::_SAVE_SIMS_IND]);
    bool preserve_analogs_index = as<bool>(config[ConfigNames::_SAVE_ANALOGS_IND]);

    // AnEnIS initialization
    AnEnIS anen(
            num_analogs, as<bool>(config[ConfigNames::_OPERATION]),
            as<bool>(config[ConfigNames::_CHECK_SEARCH_FUTURE]),
            preserve_similarity, verbose, obs_id,
            as<bool>(config[ConfigNames::_QUICK]),
            preserve_similairty_index, preserve_analogs_index,
            num_sims, num_par_nan, num_flt_nan, flt_radius);

    // Compute analogs
    anen.compute(forecasts, observations, test_times, search_times);

    /**********************************************************************
     *                           Wrap Up Results                          *
     **********************************************************************/
    List ret;
    
    if (preserve_similairty_index) {
        FunctionsR::setElement(ret, ConfigNames::_SIMS_IND, anen.getSimsIndex());
    }
    
    if (preserve_similarity) {
        FunctionsR::setElement(ret, ConfigNames::_SIMS, anen.getSimsValue());
    }
    
    if (preserve_analogs_index) {
        FunctionsR::setElement(ret, ConfigNames::_ANALOGS_IND, anen.getAnalogsIndex());
    }
    
    FunctionsR::setElement(ret, ConfigNames::_ANALOGS, anen.getAnalogsValue());

    ret.attr("class") = "AnEn";
    return (ret);
}