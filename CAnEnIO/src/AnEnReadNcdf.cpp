/* 
 * File:   AnEnReadNcdf.cpp
 * Author: wuh20
 * 
 * Created on December 30, 2019, 11:10 AM
 */

#include "AnEnReadNcdf.h"
#include "ReadNcdf.h"

#if defined(_OPENMP)
#include <omp.h>
#endif

using namespace netCDF;
using namespace std;

// The name prefix for member dimensions in file type Analogs
const string _MEMBER_DIM_PREFIX_ = "member_";

// The name prefix for member variables in file type Analogs
const string _MEMBER_VAR_PREFIX_ = "Member";

// The name prefix for search dimensions in file type SimilarityMatrices
const string _SEARCH_DIM_PREFIX_ = "search_";

// The name prefix for search variables in file type SimilarityMatrices
const string _SEARCH_VAR_PREFIX_ = "Search";

// If data have a length longer than this limit, I/O will be parallelized.
const size_t SERIAL_LENGTH_LIMIT = 1000;

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
AnEnReadNcdf::readForecasts(const string & file_path,
        Forecasts_array & forecasts) const {

    if (verbose_ >= Verbose::Progress) {
        cout << "Reading forecast file (" << file_path << ") ..." << endl;
    }

    ReadNcdf::checkPath(file_path, (bool) verbose_);

    NcFile nc(file_path, NcFile::FileMode::read);
    checkFileType_(nc, FileType::Forecasts);

    anenPar::Parameters parameters;
    anenSta::Stations stations;
    anenTime::Times times;
    anenTime::FLTs flts;

    // Read meta information
    read_(nc, parameters);
    read_(nc, stations);
    read_(nc, times);
    read_(nc, flts);

    // Prepare the meta information in forecasts first
    forecasts.setParameters(parameters);
    forecasts.setStations(stations);
    forecasts.setTimes(times);
    forecasts.setFlts(flts);

    if (verbose_ >= Verbose::Progress) {
        cout << "Updating dimensions of forecasts ..." << endl;
    }

    forecasts.updateDataDims();

    // Read data
    if (verbose_ >= Verbose::Progress) {
        cout << "Reading forecast values from file ("
                << file_path << ") ..." << endl;
    }

    read_(forecasts);

    return;
}

void
AnEnReadNcdf::read_(const NcFile & nc, anenPar::Parameters & parameters,
        size_t start, size_t count) const {

    if (verbose_ >= Verbose::Detail) cout << "Reading parameters ..." << endl;
    
    // Check whether we are reading partial or the entire variable
    bool entire = (start == 0 || count == 0);
    
    size_t dim_len = nc.getDim("num_parameters").getSize();
    vector<string> names, circulars;
    vector<double> weights;

    if (!entire) ReadNcdf::checkIndex(start, count, dim_len, (bool) verbose_);
    
    ReadNcdf::readStringVector(nc, "ParameterNames",
            names, (bool) verbose_, start, count);
    
    if (ReadNcdf::varExists(nc, "ParameterCirculars")) {
        ReadNcdf::readStringVector(nc, "ParameterCirculars",
                circulars, (bool) verbose_, start, count);
    }

    if (ReadNcdf::varExists(nc, "ParameterWeights")) {
        ReadNcdf::readVector(nc, "ParameterWeights",
                weights, (bool) verbose_, {start}, {count});
    }
    
    if (entire) {
        if (names.size() != dim_len) {
            ostringstream msg;
            msg << "There number of parameter names (" << names.size()
                    << ") does not match the length of the parameter " <<
                    "dimension (" << dim_len << ").";
            
            if (verbose_ >= Verbose::Error) {
                cerr << BOLDRED << msg.str() << RESET << endl;
            }
            
            throw length_error(msg.str());
        }
    }

    fastInsert_(parameters, names.size(), names, circulars, weights);
    return;
}

void
AnEnReadNcdf::read_(const NcFile & nc, anenSta::Stations * stations,
        size_t start, size_t count,
        const string & dim_name_prefix,
        const string & var_name_prefix) {

    if (verbose_ >= Verbose::Detail) cout << "Reading stations ..." << endl;

    // Check whether we are reading partial or the entire variable
    bool entire = (start == 0 || count == 0);
    
    size_t dim_len = nc.getDim(dim_name_prefix + "num_stations").getSize();
    vector<string> names;
    vector<double> xs, ys;
    
    if (ReadNcdf::varExists())
    
}

void
AnEnReadNcdf::checkFileType_(const NcFile & nc, FileType file_type) const {

    // Define fixed length array for names because they are hard-coded.
    array<string, 9> dim_names;
    array<string, 11> var_names;

    // Define the required dimension and variable names to be checked.
    switch (file_type) {
        case FileType::Forecasts:
            dim_names = {
                "num_parameters", "num_stations",
                "num_times", "num_flts", "num_chars"
            };
            var_names = {
                "Data", "FLTs", "Times", "ParameterNames", "Xs", "Ys"
            };
            break;
        case FileType::Observations:
            dim_names = {
                "num_parameters", "num_stations", "num_times", "num_chars"
            };
            var_names = {

                "Data", "Times", "ParameterNames", "Xs", "Ys"
            };
            break;
        case FileType::Similarity:
            dim_names = {
                "num_stations", "num_times", "num_flts", "num_entries",
                "num_cols", "num_parameters", "num_chars",
                _SEARCH_DIM_PREFIX_ + "num_stations",
                _SEARCH_DIM_PREFIX_ + "num_times"
            };
            var_names = {
                "SimilarityMatrices", "ParameterNames", "StationNames",
                "Xs", "Ys", "Times", "FLTs",
                _SEARCH_VAR_PREFIX_ + "Times",
                _SEARCH_VAR_PREFIX_ + "StationNames",
                _SEARCH_VAR_PREFIX_ + "Xs",
                _SEARCH_VAR_PREFIX_ + "Ys"
            };
            break;
        case FileType::Analogs:
            dim_names = {
                "num_stations", "num_times", "num_flts", "num_members",
                "num_cols", "num_chars",
                _MEMBER_DIM_PREFIX_ + "num_stations",
                _MEMBER_DIM_PREFIX_ + "num_times"
            };
            var_names = {
                "Analogs", "StationNames", "Xs", "Ys", "Times", "FLTs",
                _MEMBER_VAR_PREFIX_ + "Times",
                _MEMBER_VAR_PREFIX_ + "StationNames",
                _MEMBER_VAR_PREFIX_ + "Xs",
                _MEMBER_VAR_PREFIX_ + "Ys"
            };
            break;
        case FileType::StandardDeviation:
            dim_names = {"num_parameters", "num_stations", "num_flts"};
            var_names = {
                "StandardDeviation", "FLTs", "StationNames",
                "ParameterNames", "Xs", "Ys"
            };
    }

    ReadNcdf::checkDims(nc, dim_names, (bool) verbose_);
    ReadNcdf::checkVars(nc, var_names, (bool) verbose_);

    return;
}

void
AnEnReadNcdf::fastInsert_(anenPar::Parameters & parameters, size_t dim_len,
        const vector<string> & names, vector<string> & circulars,
        const vector<double> & weights) const {

    // Construct anenPar::Parameter objects and
    // prepare them for insertion into anenPar::Parameters
    //
    vector<anenPar::Parameter> vec_parameters(dim_len);

#if defined(_OPENMP)
    int num_cores = 1;
    if (dim_len <= SERIAL_LENGTH_LIMIT) {
        num_cores = 1;
    } else {
        char *num_procs_str = getenv("OMP_NUM_THREADS");
        if (num_procs_str) sscanf(num_procs_str, "%d", &num_cores);
        else num_cores = omp_get_num_procs();
    }

#pragma omp parallel for default(none) schedule(static) \
    shared(dim_len, vec_parameters, names, circulars, weights) \
    num_threads(num_cores)
#endif
    for (size_t i = 0; i < dim_len; i++) {
        auto & parameter = vec_parameters[i];

        string name = names.at(i);
        parameter.setName(name);

        // Set circular if name is present
        auto it_circular = find(circulars.begin(), circulars.end(), name);
        if (it_circular != circulars.end()) parameter.setCircular(true);

        if (!(weights.empty()))
            parameter.setWeight(weights.at(i));
    }

    // Insert
    parameters.insert(parameters.end(),
            vec_parameters.begin(), vec_parameters.end());

    // Check for duplicates
    if (parameters.size() != vec_parameters.size()) {
        ostringstream msg;
        msg << "Parameters have duplicates! Input: " <<
                vec_parameters.size() << " Unique: " << parameters.size();
        
        if (verbose_ >= Verbose::Error) {
            cerr << BOLDRED << msg.str() << RESET << endl;
        }
        
        throw runtime_error(msg.str());
    }

    return;
}