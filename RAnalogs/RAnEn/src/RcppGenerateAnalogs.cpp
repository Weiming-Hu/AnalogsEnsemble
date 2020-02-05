/*
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on August 15, 2018
 *
 * RcomputeAnEnIS.cpp is the source file with the interface definition for
 * R functions to call AnEnIS class.
 */

// [[Rcpp::plugins(cpp11)]]
// [[Rcpp::plugins(openmp)]]

#include <Rcpp.h>
#include <stdexcept>

#include "AnEnIS.h"
#include "AnEnSSE.h"
#include "RcppForecasts.h"
#include "RcppObservations.h"
#include "RcppFunctions.h"
#include "boost/numeric/conversion/cast.hpp"

using namespace Rcpp;
using namespace boost;

RCPP_EXPOSED_CLASS(Config)

//' RAnEn::generateAnalogs
//' 
//' RAnEn::generateAnalogs generates analog ensemble.
//' 
//' @param sx_forecasts Forecasts. You can generate this by typing
//' `forecasts <- generateForecastsTemplate()`.
//' @param sx_observations Observations. You can generate this by typing
//' `observations <- generateObservationsTemplate()`
//' @param sx_test_times Test times
//' @param sx_search_times Search times
//' @param sx_config A Config. You can generate this by typing `config <- new(Config)`
//' @param sx_algorithm `IS` for independent search or `SSE` for search
//' space extension
//' @return A list of results with class AnEn. You can type `print(AnEn)`
//' to see a summary.
//' 
//' @md
//' @export
// [[Rcpp::export(generateAnalogs.default)]]

List generateAnalogs(SEXP sx_forecasts, SEXP sx_observations,
        SEXP sx_test_times, SEXP sx_search_times,
        SEXP sx_config = R_NilValue, SEXP sx_algorithm = R_NilValue) {

    /**********************************************************************
     *                           Type Conversion                          *
     **********************************************************************/

    // Type checks
    if (!Rf_isNewList(sx_forecasts)) throw std::runtime_error("Forecasts should be a list");
    if (!Rf_isNewList(sx_observations)) throw std::runtime_error("Observations should be a list");
    if (!Rf_isNumeric(sx_test_times)) throw std::runtime_error("test times should be numeric");
    if (!Rf_isNumeric(sx_search_times)) throw std::runtime_error("search times should be numeric");

    // Convert Forecasts
    List list_forecasts = sx_forecasts;
    ForecastsR forecasts(
            list_forecasts[Config::_DATA],
            list_forecasts[Config::_PAR_NAMES],
            list_forecasts[Config::_CIRCULARS],
            list_forecasts[Config::_XS],
            list_forecasts[Config::_YS],
            list_forecasts[Config::_STATION_NAMES],
            list_forecasts[Config::_TIMES],
            list_forecasts[Config::_FLTS]);

    // Convert Observations
    List list_observations = sx_observations;
    ObservationsR observations(
            list_observations[Config::_DATA],
            list_observations[Config::_PAR_NAMES],
            list_observations[Config::_TIMES]);

    // Convert test and search times
    Times test_times, search_times;

    try {
        FunctionsR::toTimes(sx_test_times, test_times);
    } catch (std::exception & ex) {
        std::string msg = std::string("toTimes(test_times) -> ") + ex.what();
        throw std::runtime_error(msg);
    }

    try {
        FunctionsR::toTimes(sx_search_times, search_times);
    } catch (std::exception & ex) {
        std::string msg = std::string("toTimes(search_times) -> ") + ex.what();
        throw std::runtime_error(msg);
    }

    // Convert Config
    Config config;
    if (Rf_inherits(sx_config, "Rcpp_Config")) {
      config = as<Config>(sx_config);
    } else if (Rf_isNull(sx_config)) {
      config = Config();
    } else {
      throw std::runtime_error("Config class object is required");
    }
    
    // Take care of the index change from R to C
    if (config.obs_var_index > 0) config.obs_var_index -= 1;
    
    // Check algorithm
    std::string algorithm;
    if (Rf_isString(sx_algorithm)) {
      algorithm = as<std::string>(sx_algorithm);
    } else if (Rf_isNull(sx_algorithm)) {
      algorithm = "IS";
    } else {
      throw std::runtime_error("algorithm should be a string");
    }

    /***********************************************************************
     *                           AnEn Computation                          *
     ***********************************************************************/

    // Call routines
    AnEn* anen = nullptr;

    if (algorithm == "IS") {
        anen = new AnEnIS(config);
    } else if (algorithm == "SSE") {
        anen = new AnEnSSE(config);
    } else {
        throw std::runtime_error("algorithm not supported");
    }

    anen->compute(forecasts, observations, test_times, search_times);

    /**********************************************************************
     *                           Wrap Up Results                          *
     **********************************************************************/
    List ret;

    if (config.save_sims_station_index && algorithm == "SSE") {
        AnEnSSE* anen_sse = dynamic_cast<AnEnSSE *> (anen);
        FunctionsR::setElement(ret, Config::_SIMS_STATION_IND, anen_sse->getSimsStationIndex(), true);
    }

    if (config.save_sims_day_index) {
        AnEnIS* anen_is = dynamic_cast<AnEnIS *> (anen);
        FunctionsR::setElement(ret, Config::_SIMS_TIME_IND, anen_is->getSimsTimeIndex(), true);
    }

    if (config.save_sims) {
        AnEnIS* anen_is = dynamic_cast<AnEnIS *> (anen);
        FunctionsR::setElement(ret, Config::_SIMS, anen_is->getSimsValue(), false);
    }

    if (config.save_analogs_day_index) {
        AnEnIS* anen_is = dynamic_cast<AnEnIS *> (anen);
        FunctionsR::setElement(ret, Config::_ANALOGS_TIME_IND, anen_is->getAnalogsTimeIndex(), true);
    }

    if (config.save_analogs) {
        AnEnIS* anen_is = dynamic_cast<AnEnIS *> (anen);
        FunctionsR::setElement(ret, Config::_ANALOGS, anen_is->getAnalogsValue(), false);
    }

    if (config.save_sds) {
        AnEnIS* anen_is = dynamic_cast<AnEnIS *> (anen);
        FunctionsR::setElement(ret, Config::_SDS, anen_is->getSds(), false);
    }

    if (config.save_obs_time_index_table) {
      AnEnIS* anen_is = dynamic_cast<AnEnIS *> (anen);
                FunctionsR::setElement(ret, Config::_TIME_MAPPING, anen_is->getObsTimeIndexTable(), true);
    }

    if (config.save_search_stations_index && algorithm == "SSE") {
      AnEnSSE* anen_sse = dynamic_cast<AnEnSSE *> (anen);
      FunctionsR::setElement(ret, Config::_SEARCH_STATIONS_IND, anen_sse->getSearchStationsIndex(), true);
    }

    ret.attr("class") = "AnEn";
    delete anen;

    return ret;
}