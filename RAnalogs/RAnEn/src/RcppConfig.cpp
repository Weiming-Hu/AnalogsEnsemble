/* 
 * File:   RcppConfig.h
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on January 21, 2020, 3:36 PM
 */

#include "Config.h"
#include <Rcpp.h>

// [[Rcpp::plugins(cpp11)]]

using namespace Rcpp;

void show( Config* config_ptr) {
  config_ptr->print(Rcout);
}

RCPP_EXPOSED_CLASS(Config)
RCPP_MODULE(Config) {

    class_<Config>("Config")
        .constructor("Create a configuration with default values")
        .field("num_analogs", &Config::num_analogs, "The number of analog members in ensembles")
        .field("num_sims", &Config::num_sims, "The number of similarity metrics to save")
        .field("obs_var_index", &Config::obs_var_index, "The index for the observation variable to be predicted")
        .field("max_par_nan", &Config::max_par_nan, "The maximum number of NA values allowed in parameters")
        .field("max_flt_nan", &Config::max_flt_nan, "The maximum number of NA values allowed in forecast lead times")
        .field("flt_radius", &Config::flt_radius, "Half of the size of the forecast lead time comparison window")
        .field("num_nearest", &Config::num_nearest, "The number of nearest neighbors for search stations")
        .field("extend_obs", &Config::extend_obs, "Whether to query observations from search stations")
        .field("distance", &Config::distance, "The distance threshold when searching for nearest neighbors. A Cartesian coordinate system is assumed.")
        .field("weights", &Config::weights, "The weights for each forecast parameter")
        .field("operation", &Config::operation, "Whether to run the generation in operational mode")
        .field("prevent_search_future", &Config::prevent_search_future, "Whether to prevent using future observations during search")
        .field("save_analogs", &Config::save_analogs, "Whether to save analogs")
        .field("save_analogs_day_index", &Config::save_analogs_day_index, "Whether to save analogs time index")
        .field("save_sims", &Config::save_sims, "Whether to save similarity metrics")
        .field("save_sims_day_index", &Config::save_sims_day_index, "Whether to save similarity time index")
        .field("save_sims_station_index", &Config::save_sims_station_index, "Whether to save similarity station index")
        .field("save_sds", &Config::save_sds, "Whether to save standard deviation for forecasts standard deviation")
        .field("save_obs_time_index_table", &Config::save_obs_time_index_table, "Whether to save the index mapping from forecast times to observation times")
        .field("save_search_stations_index", &Config::save_search_stations_index, "Whether to save search stations index")
        .field("quick_sort", &Config::quick_sort, "Whether to use quick sort algorithm. If FALSE, selected analogs members are sorted based on the ascending order of similarity metrics.")
        .method("reset", &Config::reset, "Reset the configuration to its default values")
        .method("show", &show, "Print the detailed configuration")
        .property("verbose", &Config::getVerbose, &Config::setVerbose);
}
