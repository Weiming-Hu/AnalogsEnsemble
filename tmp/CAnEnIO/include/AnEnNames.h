/* 
 * File:   AnEnNames.h
 * Author: Weiming Hu (weiming@psu.edu)
 *
 * Created on January 1, 2020, 10:45 PM
 */

#ifndef ANENNAMES_H
#define ANENNAMES_H

/**
 * Namespace AnEnNames difines a series of names of variables and dimensions
 * to be used during file I/O. The naming convention can be found at:
 * 
 * https://weiming-hu.github.io/AnalogsEnsemble/2019/01/16/NetCDF-File-Types.html
 */

#include <string>

namespace AnEnNames {
    // The name prefix for member dimensions in file type Analogs
    const std::string MEMBER_DIM_PREFIX = "member_";

    // The name prefix for member variables in file type Analogs
    const std::string MEMBER_VAR_PREFIX = "Member";

    // The name prefix for search dimensions in file type SimilarityMatrices
    const std::string SEARCH_DIM_PREFIX = "search_";

    // The name prefix for search variables in file type SimilarityMatrices
    const std::string SEARCH_VAR_PREFIX = "Search";

    // Define the names for dimensions in NetCDF files
    const std::string DIM_ENTRIES = "num_entries";
    const std::string DIM_STATIONS = "num_stations";
    const std::string DIM_TIMES = "num_times";
    const std::string DIM_FLTS = "num_flts";
    const std::string DIM_MEMBERS = "num_members";
    const std::string DIM_COLS = "num_cols";
    const std::string DIM_PARS = "num_parameters";
    const std::string DIM_CHARS = "num_chars";

    // Define the names for variables in NetCDF files
    const std::string VAR_DATA = "Data";
    const std::string VAR_SIMS = "SimilarityMatrices";
    const std::string VAR_ANALOGS = "Analogs";
    const std::string VAR_STD = "StandardDeviation";
    const std::string VAR_PARNAMES = "ParameterNames";
    const std::string VAR_CIRCULARS = "ParameterCirculars";
    const std::string VAR_PARWEIGHTS = "ParameterWeights";
    const std::string VAR_XS = "Xs";
    const std::string VAR_YS = "Ys";
    const std::string VAR_STATIONNAMES = "StationNames";
    const std::string VAR_TIMES = "Times";
    const std::string VAR_FLTS = "FLTs";
}

#endif /* ANENNAMES_H */

