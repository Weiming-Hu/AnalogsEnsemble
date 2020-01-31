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
        save_sims = rhs.save_sims;
        save_sims_day_index = rhs.save_sims_day_index;
        save_analogs_day_index = rhs.save_analogs_day_index;
        quick_sort = rhs.quick_sort;
        verbose = rhs.verbose;
    }

    return *this;
}
