/* 
 * File:   RcppConfig.h
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on January 21, 2020, 3:36 PM
 */

#include <Rcpp.h>
#include "Config.h"

// [[Rcpp::plugins(cpp11)]]

using namespace Rcpp;

//' @name Config
//' @title Provides detailed configuration
//' @description Analog generation can be fine-tuned with configuration. This
//' class provides all the tunable parameters for analog generation. You can
//' type `RAnEn::Config` in the R console to see the description for each parameter.
//' To create an object of Config, you can type `config <- new(Config)`, and then 
//' you can directly modify the members, e.g. `config$num_analogs <- 10`. To see
//' the full list of tunable parameters, you can type `config` after you have 
//' created an object. It will print all the parameters.
//' @md
//' @export
class Config;

// Construct the wrap and as functions for the exposed class Config

RCPP_EXPOSED_CLASS(Config)

void show(Config* config_ptr) {
    config_ptr->print(Rcout);
}

RCPP_MODULE(Config) {

    class_<Config>("Config")
            .constructor("Create a configuration with default values. Usage: config <- new(Config)")
            .field(Config::_NUM_ANALOGS.c_str(), &Config::num_analogs, "The number of analog members in ensembles")
            .field(Config::_NUM_SIMS.c_str(), &Config::num_sims, "The number of similarity metrics to save")
            .field(Config::_OBS_ID.c_str(), &Config::obs_var_index, "The index for the observation variable to be predicted")
            .field(Config::_NUM_PAR_NA.c_str(), &Config::max_par_nan, "The maximum number of NA values allowed in parameters")
            .field(Config::_NUM_FLT_NA.c_str(), &Config::max_flt_nan, "The maximum number of NA values allowed in forecast lead times")
            .field(Config::_FLT_RADIUS.c_str(), &Config::flt_radius, "Half of the size of the forecast lead time comparison window")
            .field(Config::_NUM_NEAREST.c_str(), &Config::num_nearest, "The number of nearest neighbors for search stations")
            .field(Config::_EXTEND_OBS.c_str(), &Config::extend_obs, "Whether to query observations from search stations")
            .field(Config::_DISTANCE.c_str(), &Config::distance, "The distance threshold when searching for nearest neighbors. A Cartesian coordinate system is assumed.")
            .field(Config::_WEIGHTS.c_str(), &Config::weights, "The weights for each forecast parameter")
            .field(Config::_OPERATION.c_str(), &Config::operation, "Whether to run the generation in operational mode")
            .field(Config::_PREVENT_SEARCH_FUTURE.c_str(), &Config::prevent_search_future, "Whether to prevent using future observations during search")
            .field(Config::_SAVE_ANALOGS.c_str(), &Config::save_analogs, "Whether to save analogs")
            .field(Config::_SAVE_ANALOGS_TIME_IND.c_str(), &Config::save_analogs_day_index, "Whether to save analogs time index")
            .field(Config::_SAVE_SIMS.c_str(), &Config::save_sims, "Whether to save similarity metrics")
            .field(Config::_SAVE_SIMS_TIME_IND.c_str(), &Config::save_sims_day_index, "Whether to save similarity time index")
            .field(Config::_SAVE_SIMS_STATION_IND.c_str(), &Config::save_sims_station_index, "Whether to save similarity station index")
            .field(Config::_SAVE_SDS.c_str(), &Config::save_sds, "Whether to save standard deviation for forecasts standard deviation")
            .field(Config::_SAVE_OBS_TIME_IND_TABLE.c_str(), &Config::save_obs_time_index_table, "Whether to save the index mapping from forecast times to observation times")
            .field(Config::_SAVE_SEARCH_STATIONS_IND.c_str(), &Config::save_search_stations_index, "Whether to save search stations index")
            .field(Config::_QUICK.c_str(), &Config::quick_sort, "Whether to use quick sort algorithm. If FALSE, selected analogs members are sorted based on the ascending order of similarity metrics.")
            .method("reset", &Config::reset, "Reset the configuration to its default values")
            .method("show", &show, "Print the detailed configuration")
            .property(Config::_VERBOSE.c_str(), &Config::getVerbose, &Config::setVerbose);
}
