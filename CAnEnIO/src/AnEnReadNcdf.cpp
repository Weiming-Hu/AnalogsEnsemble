/* 
 * File:   AnEnReadNcdf.cpp
 * Author: wuh20
 * 
 * Created on December 30, 2019, 11:10 AM
 */

#include "colorTexts.h"
#include "AnEnReadNcdf.h"
#include "boost/filesystem.hpp"

#include <stdexcept>

namespace filesys = boost::filesystem;
using namespace netCDF;
using namespace std;

const string AnEnReadNcdf::_MEMBER_DIM_PREFIX_ = "member_";
const string AnEnReadNcdf::_MEMBER_VAR_PREFIX_ = "Member";
const string AnEnReadNcdf::_SEARCH_DIM_PREFIX_ = "search_";
const string AnEnReadNcdf::_SEARCH_VAR_PREFIX_ = "Search";

AnEnReadNcdf::AnEnReadNcdf() {
    verbose_ = Verbose::Progress;
}

AnEnReadNcdf::AnEnReadNcdf(Verbose verbose) :
verbose_(verbose) {
}

AnEnReadNcdf::AnEnReadNcdf(const AnEnReadNcdf& orig) {
    if (this != &orig) {
        verbose_ = orig.verbose_;
    }
}

AnEnReadNcdf::~AnEnReadNcdf() {
}

void
AnEnReadNcdf::readForecasts(
        const string & file_path,
        Forecasts_array & forecasts) const {

    if (verbose_ >= Verbose::Progress) {
        cout << "Reading forecast file (" << file_path << ") ..." << endl;
    }

    checkPath_(file_path);

    NcFile nc(file_path, NcFile::FileMode::read);
    checkFileType_(nc, FileType::Forecasts);
    
    anenPar::Parameters parameters;
    anenSta::Stations stations;
    anenTime::Times times;
    anenTime::FLTs flts;

    // Read meta information
    readParameters_(nc, parameters);
    readStations_(nc, stations);
    readTimes_(nc, times);
    readFLTs_(nc, flts);

    // Prepare the meta information in forecasts first
    forecasts.setParameters(parameters);
    forecasts.setStations(stations);
    forecasts.setTimes(times);
    forecasts.setFlts(flts);

    if (verbose_ >= Verbose::Progress) {
        cout << "Update dimensions of forecasts ..." << endl;
    }
    
    forecasts.updateDataDims();

    // Read data
    if (verbose_ >= Verbose::Progress) {
        cout << "Reading forecast values from file ("
            << file_path << ") ..." << endl;
    }
    
    readForecastsArrayData_(forecasts);

    return;
}

void
AnEnReadNcdf::checkPath_(const string & file_path) const {

    if (verbose_ >= Verbose::Detail) {
        cout << "Checking file (" << file_path << ") ..." << endl;
    }

    // Check whether the file exists
    bool file_exists = filesys::exists(file_path);
    if (!file_exists) {
        string msg = "File not found " + file_path;
        if (verbose_ >= Verbose::Error) cerr << BOLDRED << msg << RESET << endl;
        throw invalid_argument(msg);
    }

    filesys::path boost_path(file_path);

    // Succeed if the file path has .nc as its extension
    if (boost_path.has_extension()) {
        if (boost_path.extension().string() == ".nc") return;        
    }

    // Fail if the file path does not have a .nc extension
    string msg = "Unknown file extension " + file_path;
    if (verbose_ >= Verbose::Error) cerr << BOLDRED << msg << RESET << endl;
    throw invalid_argument(msg);
}

void
AnEnReadNcdf::checkFileType_(
        const NcFile & nc, FileType file_type) const {

    // Define fixed length array for names because they are hard-coded.
    array<string, 9> dim_names;
    array<string, 11> var_names;
    
    // Define the required dimension and variable names to be checked.
    switch(file_type) {
        case FileType::Forecasts:
            dim_names = {{
                "num_parameters", "num_stations",
                "num_times", "num_flts", "num_chars"}};
            var_names = {{
                "Data", "FLTs", "Times", "ParameterNames", "Xs", "Ys"}};
            break;
        case FileType::Observations:
            dim_names = {{
                "num_parameters", "num_stations", "num_times", "num_chars"}};
            var_names = {{
                "Data", "Times", "ParameterNames", "Xs", "Ys"}};
            break;
        case FileType::Similarity:
            dim_names = {{
                "num_stations", "num_times", "num_flts", "num_entries",
                "num_cols", "num_parameters", "num_chars",
                _SEARCH_DIM_PREFIX_ + "num_stations",
                _SEARCH_DIM_PREFIX_ + "num_times"}};
            var_names = {{
                "SimilarityMatrices", "ParameterNames", "StationNames",
                "Xs", "Ys", "Times", "FLTs",
                _SEARCH_VAR_PREFIX_ + "Times",
                _SEARCH_VAR_PREFIX_ + "StationNames",
                _SEARCH_VAR_PREFIX_ + "Xs",
                _SEARCH_VAR_PREFIX_ + "Ys"}};
            break;
        case FileType::Analogs:
            dim_names = {{
                "num_stations", "num_times", "num_flts", "num_members",
                "num_cols", "num_chars",
                _MEMBER_DIM_PREFIX_ + "num_stations",
                _MEMBER_DIM_PREFIX_ + "num_times"}};
            var_names = {{
                "Analogs", "StationNames", "Xs", "Ys", "Times", "FLTs",
                _MEMBER_VAR_PREFIX_ + "Times",
                _MEMBER_VAR_PREFIX_ + "StationNames",
                _MEMBER_VAR_PREFIX_ + "Xs",
                _MEMBER_VAR_PREFIX_ + "Ys"}};
            break;
        case FileType::StandardDeviation:
            dim_names = {{"num_parameters", "num_stations", "num_flts"}};
            var_names = {{"StandardDeviation", "FLTs", "StationNames",
                "ParameterNames", "Xs", "Ys"}};
    }

    for (const auto & name : dim_names) {
        checkDim_(nc, name);
    }

    for (const auto & name : var_names) {
        checkVar_(nc, name);
    }

    return;
}

void
AnEnReadNcdf::checkDim_(const NcFile & nc, const string & name) const {
    
    NcDim dim = nc.getDim(name);
    
    if (dim.isNull()) {
        string msg = "Dimension (" + name + ") is missing!";
        if (verbose_ >= Verbose::Error) cerr << BOLDRED << msg << RESET << endl;
        throw invalid_argument(msg);
    }
    
    return;
}

void
AnEnReadNcdf::checkVar_(const NcFile & nc, const string & name) const {

    NcVar var = nc.getVar(name);

    if (var.isNull()) {
        string msg = "Variable (" + name + ") is missing!";
        if (verbose_ >= Verbose::Error) cerr << BOLDRED << msg << RESET << endl;
        throw invalid_argument(msg);
    }

    return;
}

