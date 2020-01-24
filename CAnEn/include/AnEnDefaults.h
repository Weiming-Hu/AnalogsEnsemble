/* 
 * File:   Default.h
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <cervone@psu.edu>
 *
 * Created on January 5, 2020, 7:27 AM
 */

#ifndef ANENDEFAULTS_H
#define ANENDEFAULTS_H

#include <string>
#include <limits>

/**
 * The AnEnDefault namespace includes default values used throughout the project.
 */
namespace AnEnDefaults {
    static const std::string _NAME = "UNDEFINED";
    static const double _WEIGHT = 1;
    static const bool _CIRCULAR = false;
    static const double _X = 0.0;
    static const double _Y = 0.0;
    
    static const std::size_t _TIME = 0;
    static const std::string _UNIT = "seconds";
    static const std::string _ORIGIN = "1970-01-01";
    
    static const bool _OPERATIONAL = false;
    static const bool _CHECK_SEARCH_FUTURE = true;

    static const std::size_t _NUM_ANALOGS = 1;
    static const std::size_t _NUM_SIMS = _NUM_ANALOGS;
    
    static const bool _SAVE_SIMS = false;
    static const bool _SAVE_SIMS_INDEX = false;
    static const bool _SAVE_ANALOGS_INDEX = false;
    
    static const std::size_t _OBS_VAR_INDEX = 0;
    static const bool _QUICK_SORT = true;
    static const std::size_t _MAX_PAR_NAN = 0;
    static const std::size_t _MAX_FLT_NAN = 0;
    static const std::size_t _FLT_RADIUS = 1;

    static const std::size_t _PREVIEW_COUNT = 10;
    static const std::size_t _MAX_SIZE_T = std::numeric_limits<std::size_t>::max();

    /**
     * A higher level will contain all messages from the lower levels. For
     * example, The progress level will contain errors and warnings. Errors,
     * if any, must be printed.
     */
    enum class Verbose {
        Error = 0, Warning = 1, Progress = 2,
        Detail = 3, Debug = 4
    };
    
    static const Verbose _VERBOSE = Verbose::Progress;
}

#endif /* ANENDEFAULTS_H */

