/* 
 * File:   Config.h
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <cervone@psu.edu>
 *
 * Created on January 31, 2020, 2:05 PM
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <vector>
#include <iostream>

/**
 * A higher level will contain all messages from the lower levels. For
 * example, The progress level will contain errors and warnings. Errors,
 * if any, must be printed.
 */
enum class Verbose {
    Error = 0,
    Warning = 1,
    Progress = 2,
    Detail = 3,
    Debug = 4
};

//' @export
class Config {
public:
    Config();
    Config(const Config& orig);
    virtual ~Config();
    
    void print(std::ostream & os);
    
    int getVerbose();
    void setVerbose(int);

    void reset();

    /*************************************************************************
     *                      Configurable Parameters                          *
     *************************************************************************/

    std::size_t num_analogs;
    std::size_t num_sims;
    std::size_t obs_var_index;
    std::size_t max_par_nan;
    std::size_t max_flt_nan;
    std::size_t flt_radius;
    std::size_t num_nearest;
    std::size_t extend_obs;

    double distance;
    
    std::vector<double> weights;

    bool operation;
    bool prevent_search_future;
    bool save_analogs;
    bool save_analogs_day_index;
    bool save_sims;
    bool save_sims_day_index;
    bool save_sims_station_index;
    bool save_sds;
    bool save_obs_time_index_table;
    bool save_search_stations_index;
    bool quick_sort;

    Verbose verbose;
    
    /*************************************************************************
     *                    Predefined Static Parameters                       *
     *************************************************************************/
    // These parameters are not allowed to change. Usually, they are used to
    // configure the default values for Parameters, Times and Stations.
    //
    static const std::string _NAME;
    static const bool _CIRCULAR;
    static const double _X;
    static const double _Y;
    static const std::size_t _TIME;
    static const std::string _UNIT;
    static const std::string _ORIGIN;

    /*************************************************************************
     *                    Predefined Parameter Names                         *
     *************************************************************************/
    // This defines how each parameter is called in R
    //
    // Input parameter names
    static const std::string _NUM_ANALOGS;
    static const std::string _NUM_SIMS;
    static const std::string _OBS_ID;
    static const std::string _NUM_PAR_NA;
    static const std::string _NUM_FLT_NA;
    static const std::string _FLT_RADIUS;
    static const std::string _NUM_NEAREST;
    static const std::string _EXTEND_OBS;
    static const std::string _DISTANCE;
    static const std::string _OPERATION;
    static const std::string _PREVENT_SEARCH_FUTURE;
    static const std::string _SAVE_ANALOGS;
    static const std::string _SAVE_ANALOGS_TIME_IND;
    static const std::string _SAVE_SIMS;
    static const std::string _SAVE_SIMS_TIME_IND;
    static const std::string _SAVE_SIMS_STATION_IND;
    static const std::string _SAVE_SDS;
    static const std::string _SAVE_OBS_TIME_IND_TABLE;
    static const std::string _SAVE_SEARCH_STATIONS_IND;
    static const std::string _QUICK;
    static const std::string _VERBOSE;
    static const std::string _TEST_TIMES;
    static const std::string _SEARCH_TIMES;
    static const std::string _WEIGHTS;

    // Output member names
    static const std::string _ANALOGS;
    static const std::string _ANALOGS_TIME_IND;
    static const std::string _SIMS;
    static const std::string _SIMS_TIME_IND;
    static const std::string _SIMS_STATION_IND;
};

#endif /* CONFIG_H */

