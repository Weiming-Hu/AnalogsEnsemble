/* 
 * File:   Config.cpp
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <cervone@psu.edu>
 * 
 * Created on January 31, 2020, 2:05 PM
 */

#include "Config.h"
#include "Functions.h"

#include <limits>
#include <cmath>

using namespace std;

const bool Config::_CIRCULAR = false;
const double Config::_X = 0.0;
const double Config::_Y = 0.0;
const size_t Config::_TIME = 0;
const string Config::_NAME = "UNDEFINED";

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
const string Config::_WEIGHTS = "weights";
const string Config::_PREVENT_SEARCH_FUTURE = "prevent_search_future";
const string Config::_SAVE_ANALOGS = "save_analogs";
const string Config::_SAVE_ANALOGS_TIME_IND = "save_analogs_time_index";
const string Config::_SAVE_SIMS = "save_similarity";
const string Config::_SAVE_SIMS_TIME_IND = "save_similarity_time_index";
const string Config::_SAVE_SIMS_STATION_IND = "save_similarity_station_index";
const string Config::_SAVE_SDS = "save_sds";
const string Config::_SAVE_OBS_TIME_IND_TABLE = "save_obs_time_index_table";
const string Config::_SAVE_SEARCH_STATIONS_IND = "save_search_stations_index";
const string Config::_QUICK = "quick";
const string Config::_VERBOSE = "verbose";

const string Config::_DATA = "Data";
const string Config::_PAR_NAMES = "ParameterNames";
const string Config::_CIRCULARS = "ParameterCirculars";
const string Config::_XS = "Xs";
const string Config::_YS = "Ys";
const string Config::_STATION_NAMES = "StationNames";
const string Config::_TIMES = "Times";
const string Config::_FLTS = "FLTs";

const string Config::_DIM_STATIONS = "num_stations";
const string Config::_DIM_TIMES = "num_times";
const string Config::_DIM_TEST_TIMES = "num_test_times";
const string Config::_DIM_SEARCH_TIMES = "num_search_times";
const string Config::_DIM_FLTS = "num_flts";
const string Config::_DIM_PARS = "num_parameters";
const string Config::_DIM_CHARS = "num_chars";
const string Config::_DIM_ANALOGS = "num_analogs";
const string Config::_DIM_SIMS = "num_similarity";

const string Config::_ANALOGS = "analogs";
const string Config::_ANALOGS_TIME_IND = "analogs_time_index";
const string Config::_SIMS = "similarity";
const string Config::_SIMS_TIME_IND = "similarity_time_index";
const string Config::_SIMS_STATION_IND = "similarity_station_index";
const string Config::_SDS = "sds";
const string Config::_TIME_MAPPING = "mapping";
const string Config::_SEARCH_STATIONS_IND = "search_stations";
const string Config::_TEST_TIMES = "test_times";
const string Config::_SEARCH_TIMES = "search_times";

Config::Config() {
    reset();
}

Config::Config(const Config & orig) {
    *this = orig;
}

Config::~Config() {
}

void
Config::print(ostream & os) {
    os << "Configuration details:" << endl
            << "num_analogs: " << num_analogs << endl
            << "num_sims: " << num_sims << endl
            << "obs_var_index: " << obs_var_index << endl
            << "quick_sort: " << (quick_sort ? "true" : "false") << endl
            << "max_par_nan: " << max_par_nan << endl
            << "max_flt_nan: " << max_flt_nan << endl
            << "flt_radius: " << flt_radius << endl
            << "num_nearest: " << num_nearest << endl
            << "distance: " << distance << endl
            << "extend_obs: " << (extend_obs ? "true" : "false") << endl
            << "operation: " << (operation ? "true" : "false") << endl
            << "prevent_search_future: " << (prevent_search_future ? "true" : "false") << endl
            << "save_analogs: " << (save_analogs ? "true" : "false") << endl
            << "save_analogs_time_index: " << (save_analogs_time_index ? "true" : "false") << endl
            << "save_sims: " << (save_sims ? "true" : "false") << endl
            << "save_sims_time_index: " << (save_sims_time_index ? "true" : "false") << endl
            << "save_sims_station_index: " << (save_sims_station_index ? "true" : "false") << endl
            << "save_sds: " << (save_sds ? "true" : "false") << endl
            << "save_obs_time_index_table: " << (save_obs_time_index_table ? "true" : "false") << endl
            << "save_search_stations_index: " << (save_search_stations_index ? "true" : "false") << endl
            << "weights: " << (weights.size() > 0 ? Functions::format(weights) : "[equally weighted with 1s]") << endl
            << "verbose: " << Functions::vtoi(verbose) << " (" << Functions::vtos(verbose) << ")" << endl;
    return;
}

int
Config::getVerbose() {
    return Functions::vtoi(verbose);
}

void
Config::setVerbose(int i) {
    verbose = Functions::itov(i);
    return;
}

void
Config::reset() {

    // This is the default configuration setting.
    //
    // weights are automatically initialized to empty vector
    //
    num_analogs = 1;
    num_sims = 1;
    obs_var_index = 0;
    quick_sort = true;
    max_par_nan = 0;
    max_flt_nan = 0;
    flt_radius = 1;
    num_nearest = 1;
    distance = NAN;
    extend_obs = true;
    operation = false;
    prevent_search_future = true;
    save_analogs = true;
    save_analogs_time_index = false;
    save_sims = false;
    save_sims_time_index = false;
    save_sims_station_index = false;
    save_sds = false;
    save_obs_time_index_table = false;
    save_search_stations_index = false;
    verbose = Verbose::Warning;

    return;
}
