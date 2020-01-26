/* 
 * File:   ConfigNames.h
 * Author: Weiming Hu <cervone@psu.edu>
 *
 * Created on January 23, 2020, 6:49 PM
 */

#ifndef CONFIGNAMES_H
#define CONFIGNAMES_H

#include <string>

/**
 * ConfigNames controls the names of the configuration list elements to
 * be recognized.
 */
namespace ConfigNames {
    
    // Input names
    static const std::string _OBS = "observations";
    static const std::string _OBS_TIMES = "observation_times";
    static const std::string _WEIGHTS = "weights";
    static const std::string _CIRCULARS = "circulars";
    static const std::string _FCST_TIMES = "forecast_times";
    static const std::string _FLTS = "flts";
    static const std::string _FCSTS = "forecasts";
    static const std::string _TEST_TIMES = "test_times";
    static const std::string _SEARCH_TIMES = "search_times";
    static const std::string _VERBOSE = "verbose";
    static const std::string _OBS_ID = "observation_id";
    static const std::string _NUM_MEMBERS = "num_members";
    static const std::string _NUM_SIMS = "max_num_sims";
    static const std::string _NUM_PAR_NA = "max_par_nan";
    static const std::string _NUM_FLT_NA = "max_flt_nan";
    static const std::string _FLT_RADIUS = "FLT_radius";
    static const std::string _SAVE_SIMS = "preserve_similarity";
    static const std::string _SAVE_SIMS_IND = "preserve_similarity_index";
    static const std::string _SAVE_ANALOGS_IND = "preserve_analogs_index";
    static const std::string _OPERATION = "operational";
    static const std::string _PREVENT_SEARCH_FUTURE = "prevent_search_future";
    static const std::string _QUICK = "quick";
    static const std::string _VERSION = "version";
    
    // Output names
    static const std::string _ANALOGS = "analogs";
    static const std::string _ANALOGS_IND = "analogs_index";
    static const std::string _SIMS = "similarity";
    static const std::string _SIMS_IND = "similarity_index";
}

#endif /* CONFIGNAMES_H */

