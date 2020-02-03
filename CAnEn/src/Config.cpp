/* 
 * File:   Config.cpp
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <cervone@psu.edu>
 * 
 * Created on January 31, 2020, 2:05 PM
 */

#include "Config.h"

#include <limits>
#include <cmath>

using namespace std;

const std::string Config::_NAME = "UNDEFINED";
const double Config::_WEIGHT = 1;
const bool Config::_CIRCULAR = false;
const double Config::_X = 0.0;
const double Config::_Y = 0.0;
const std::size_t Config::_TIME = 0;
const std::string Config::_UNIT = "seconds";
const std::string Config::_ORIGIN = "1970-01-01";

const string Config::_NUM_ANALOGS = "num_analogs";
const string Config::_NUM_SIMS = "num_similarity";
const string Config::_OBS_ID = "observation_id";
const string Config::_NUM_PAR_NA = "max_par_nan";
const string Config::_NUM_FLT_NA = "max_flt_nan";
const string Config::_FLT_RADIUS = "flt_radius";
const string Config::_NUM_NEAREST = "num_nearest";
const string Config::_EXTEND_OBS = "extend_observation";
const string Config::_DISTANCE = "distance";
const string Config::_OPERATION = "operation";
const string Config::_PREVENT_SEARCH_FUTURE = "prevent_search_future";
const string Config::_SAVE_ANALOGS = "save_analogs";
const string Config::_SAVE_ANALOGS_TIME_IND = "save_analogs_day_index";
const string Config::_SAVE_SIMS = "save_similarity";
const string Config::_SAVE_SIMS_TIME_IND = "save_similarity_day_index";
const string Config::_SAVE_SIMS_STATION_IND = "save_similarity_station_index";
const string Config::_SAVE_SDS = "save_sds";
const string Config::_SAVE_OBS_TIME_IND_TABLE = "save_obs_time_index_table";
const string Config::_SAVE_SEARCH_STATIONS_IND = "save_search_stations_index";
const string Config::_QUICK = "quick";
const string Config::_VERBOSE = "verbose";
const string Config::_TEST_TIMES = "test_times";
const string Config::_SEARCH_TIMES = "search_times";
const string Config::_ANALOGS = "analogs";
const string Config::_ANALOGS_TIME_IND = "analogs_time_index";
const string Config::_SIMS = "similarity";
const string Config::_SIMS_TIME_IND = "similarity_time_index";
const string Config::_SIMS_STATION_IND = "station_index";

Config::Config() {
    reset();
}

Config::Config(const Config & orig) {
    *this = orig;
}

Config::~Config() {
}

void
Config::reset() {
    
    // This is the default configuration setting.
    
    num_analogs = 1;
    num_sims = 1;
    obs_var_index = 0;
    quick_sort = true;
    max_par_nan = 0;
    max_flt_nan = 0;
    flt_radius = 1;
    num_nearest = 1;
    extend_obs = true;
    preview_count = 5;
    distance = NAN;
    operation = false;
    prevent_search_future = true;
    save_analogs = true;
    save_analogs_day_index = false;
    save_sims = false;
    save_sims_day_index = false;
    save_sims_station_index = false;
    save_sds = false;
    save_obs_time_index_table = false;
    save_search_stations_index = false;
    verbose = Verbose::Warning;
    
    return;
}

Config& Config::operator=(const Config & rhs) {

    if (this != &rhs) {
        num_analogs = rhs.num_analogs;
        num_sims = rhs.num_sims;
        obs_var_index = rhs.obs_var_index;
        max_par_nan = rhs.max_par_nan;
        max_flt_nan = rhs.max_flt_nan;
        flt_radius = rhs.flt_radius;
        num_nearest = rhs.num_nearest;
        extend_obs = rhs.extend_obs;
        preview_count = rhs.preview_count;
        distance = rhs.distance;
        operation = rhs.operation;
        prevent_search_future = rhs.prevent_search_future;
        save_analogs = rhs.save_analogs;
        save_analogs_day_index = rhs.save_analogs_day_index;
        save_sims = rhs.save_sims;
        save_sims_day_index = rhs.save_sims_day_index;
        save_sims_station_index = rhs.save_sims_station_index;
        quick_sort = rhs.quick_sort;
        verbose = rhs.verbose;
    }

    return *this;
}
