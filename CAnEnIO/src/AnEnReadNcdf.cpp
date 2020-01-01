/* 
 * File:   AnEnReadNcdf.cpp
 * Author: wuh20
 * 
 * Created on December 30, 2019, 11:10 AM
 */

#include "AnEnReadNcdf.h"
#include "ReadNcdf.h"

#include <array>

#if defined(_OPENMP)
#include <omp.h>
#endif

using namespace ReadNcdf;
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
    if (this != &orig) verbose_ = orig.verbose_;
}

AnEnReadNcdf::~AnEnReadNcdf() {
}

void
AnEnReadNcdf::readForecasts(const string & file_path,
        Forecasts_array & forecasts) const {
    // Call the handling function
    readForecasts(file_path, forecasts, {}, {});
    return;
}

void
AnEnReadNcdf::readForecasts(const string & file_path,
        Forecasts_array & forecasts, vector<size_t> start,
        vector<size_t> count) const {

    if (verbose_ >= Verbose::Progress) {
        cout << "Reading forecast file (" << file_path << ") ..." << endl;
    }

    // Check whether we are reading partial or the entire variable
    bool entire = (start.size() == 0 || count.size() == 0);

    if (!entire) {
        if (start.size() != 4 || count.size() != 4) {
            ostringstream msg;
            msg << BOLDRED << "#start (" << start.size() <<
                    ") and #count (" << count.size() <<
                    ") should both be 4 for Forecasts." << RESET;
            throw runtime_error(msg.str());
        }
    }

    checkPath(file_path);

    NcFile nc(file_path, NcFile::FileMode::read);
    checkFileType_(nc, FileType::Forecasts);

    // Initialization for meta information
    anenPar::Parameters parameters;
    anenSta::Stations stations;
    anenTime::Times times;
    anenTime::FLTs flts;

    // Read meta information
    if (entire) {
        read_(nc, parameters);
        read_(nc, stations);
        read_(nc, times);
        read_(nc, flts);
    } else {
        read_(nc, parameters, start[0], count[0]);
        read_(nc, stations, start[1], count[1]);
        read_(nc, times, start[2], count[2]);
        read_(nc, flts, start[3], count[3]);
    }

    forecasts.setParameters(parameters);
    forecasts.setStations(stations);
    forecasts.setTimes(times);
    forecasts.setFlts(flts);

    // Read data values
    if (verbose_ >= Verbose::Detail) cout << "Updating dimensions ..." << endl;
    forecasts.updateDataDims();
    read_(nc, forecasts.data().data(), "Data", start, count);
    nc.close();

    return;
}

void
AnEnReadNcdf::readObservations(const string & file_path,
        Observations_array & observations) const {
    readObservations(file_path, observations, {}, {});
    return;
}

void
AnEnReadNcdf::readObservations(const std::string & file_path,
        Observations_array & observations,
        vector<size_t> start, vector<size_t> count) const {

    if (verbose_ >= Verbose::Progress) {
        cout << "Reading observation file (" << file_path << ") ..." << endl;
    }

    // Check whether we are reading partial or the entire variable
    bool entire = (start.size() == 0 || count.size() == 0);

    if (!entire) {
        if (start.size() != 3 || count.size() != 3) {
            ostringstream msg;
            msg << BOLDRED << "#start (" << start.size() <<
                    ") and #count (" << count.size() <<
                    ") should both be 4 for Forecasts." << RESET;
            throw runtime_error(msg.str());
        }
    }

    checkPath(file_path);

    NcFile nc(file_path, NcFile::FileMode::read);
    checkFileType_(nc, FileType::Observations);

    // Initialization for meta information
    anenPar::Parameters parameters;
    anenSta::Stations stations;
    anenTime::Times times;

    // Read meta information
    if (entire) {
        read_(nc, parameters);
        read_(nc, stations);
        read_(nc, times);
    } else {
        read_(nc, parameters, start[0], count[0]);
        read_(nc, stations, start[1], count[1]);
        read_(nc, times, start[2], count[2]);
    }

    observations.setParameters(parameters);
    observations.setStations(stations);
    observations.setTimes(times);
    
    // Read data values
    if (verbose_ >= Verbose::Detail) cout << "Updating dimensions ..." << endl;
    observations.updateDataDims();
    read_(nc, observations.data().data(), "Data", start, count);
    nc.close();

    return;
}

void
AnEnReadNcdf::readSimilarityMatrices(
        const string & file_path, SimilarityMatrices & sims,
        vector<size_t> start, vector<size_t> count) const {
    
    if (verbose_ >= Verbose::Progress) {
        cout << "Reading similarity metric file (" <<
                file_path << ") ..." << endl;
    }

    // Check whether we are reading partial or the entire variable
    bool entire = (start.size() == 0 || count.size() == 0);

    if (!entire) {
        if (start.size() != 5 || count.size() != 5) {
            ostringstream msg;
            msg << BOLDRED << "#start (" << start.size() <<
                    ") and #count (" << count.size() <<
                    ") should both be 5 for SimilarityMatrices." << RESET;
            throw runtime_error(msg.str());
        }
    }
    
    // Resize similarity to clear any leftover values
    sims.resize(0, 0, 0);

    if (verbose_ >= Verbose::Detail) cout << "Updating dimensions ..." << endl;
    NcFile nc(file_path, NcFile::FileMode::read);
    
    if (entire) {
        sims.setMaxEntries(nc.getDim("num_entries").getSize());
        sims.resize(nc.getDim("num_stations").getSize(),
                nc.getDim("num_times").getSize(),
                nc.getDim("num_flts").getSize());
    } else {
        sims.setMaxEntries(count[3]);
        sims.resize(count[0], count[1], count[2]);
        
    }

    read_(nc, sims.data(), "SimilarityMatrices", start, count);
    nc.close();
    return;
}

void
AnEnReadNcdf::readAnalogs(
        const string & file_path, Analogs & analogs,
        vector<size_t> start, vector<size_t> count) const {

    if (verbose_ >= Verbose::Progress) {
        cout << "Reading analog file (" << file_path << ") ..." << endl;
    }

    // Check whether we are reading partial or the entire variable
    bool entire = (start.size() == 0 || count.size() == 0);

    if (!entire) {
        if (start.size() != 5 || count.size() != 5) {
            ostringstream msg;
            msg << BOLDRED << "#start (" << start.size() <<
                    ") and #count (" << count.size() <<
                    ") should both be 5 for Analogs." << RESET;
            throw runtime_error(msg.str());
        }
    }
    
    if (verbose_ >= Verbose::Detail) cout << "Updating dimensions ..." << endl;
    NcFile nc(file_path, NcFile::FileMode::read);
    
    if (entire) {
        analogs.resize(boost::extents
                [nc.getDim("num_stations").getSize()]
                [nc.getDim("num_times").getSize()]
                [nc.getDim("num_flts").getSize()]
                [nc.getDim("num_members").getSize()]
                [nc.getDim("num_cols").getSize()]);
    } else {
        analogs.resize(boost::extents
                [count[0]][count[1]][count[2]][count[3]][count[4]]);
    }
    
    read_(nc, analogs.data(), "Analogs", start, count);
    nc.close();
    return;
}

void
AnEnReadNcdf::readStandardDeviation(
        const string & file_path, StandardDeviation & sds,
        vector<size_t> start, vector<size_t> count) const {
    
    if (verbose_ >= Verbose::Progress) {
        cout << "Reading standard deviation file ("
                << file_path << ") ..." << endl;
    }

    // Check whether we are reading partial or the entire variable
    bool entire = (start.size() == 0 || count.size() == 0);

    if (!entire) {
        if (start.size() != 3 || count.size() != 3) {
            ostringstream msg;
            msg << BOLDRED << "#start (" << start.size() <<
                    ") and #count (" << count.size() <<
                    ") should both be 3 for StandardDeviation." << RESET;
            throw runtime_error(msg.str());
        }
    }
    
    if (verbose_ >= Verbose::Detail) cout << "Updating dimensions ..." << endl;
    NcFile nc(file_path, NcFile::FileMode::read);

    if (entire) {
        sds.resize(boost::extents
                [nc.getDim("num_parameters").getSize()]
                [nc.getDim("num_stations").getSize()]
                [nc.getDim("num_flts").getSize()]);
    } else {
        sds.resize(boost::extents
                [count[0]][count[1]][count[2]]);
    }

    read_(nc, sds.data(), "StandardDeviation", start, count);
    nc.close();
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

    if (!entire) checkIndex(start, count, dim_len);

    readStringVector(nc, "ParameterNames", names, start, count);

    if (varExists(nc, "ParameterCirculars")) {
        readStringVector(nc, "ParameterCirculars",
                circulars, start, count);
    }

    if (varExists(nc, "ParameterWeights")) {
        if (entire) readVector(nc, "ParameterWeights", weights);
        else readVector(nc, "ParameterWeights", weights, {start}, {count});
    }

    fastInsert_(parameters, names.size(), names, circulars, weights);
    return;
}

void
AnEnReadNcdf::read_(const NcFile & nc, anenSta::Stations & stations,
        size_t start, size_t count,
        const string & dim_name_prefix,
        const string & var_name_prefix) const {

    if (verbose_ >= Verbose::Detail) cout << "Reading stations ..." << endl;

    size_t dim_len = nc.getDim(dim_name_prefix + "num_stations").getSize();
    vector<string> names;
    vector<double> xs, ys;

    if (start == 0 || count == 0) {
        readVector(nc, var_name_prefix + "Xs", xs);
        readVector(nc, var_name_prefix + "Ys", ys);
    } else {
        checkIndex(start, count, dim_len);
        readVector(nc, var_name_prefix + "Xs", xs, {start}, {count});
        readVector(nc, var_name_prefix + "Ys", ys, {start}, {count});
    }

    if (varExists(nc, var_name_prefix + "StationNames")) {
        readStringVector(nc, var_name_prefix + "StationNames",
                names, start, count);
    }

    fastInsert_(stations, xs.size(), names, xs, ys);
    return;
}

void
AnEnReadNcdf::read_(const netCDF::NcFile & nc, anenTime::Times & times,
            size_t start, size_t count, const string & var_name) const {
    
    if (verbose_ >= Verbose::Detail) cout << "Reading times ..." << endl;
    vector<double> vec;
    
    if (start == 0 || count == 0) {
        readVector(nc, var_name, vec);
    } else {
        // If it is partial reading, check the indices
        const auto & dims = nc.getVar(var_name).getDims();
        
        if (dims.size() != 1) {
            ostringstream msg;
            msg << BOLDRED << "Times should be 1-dimensional." << RESET;
            throw runtime_error(msg.str());
        }
        
        checkIndex(start, count, dims[0].getSize());
        readVector(nc, var_name, vec, {start}, {count});
    }

    times.insert(times.end(), vec.begin(), vec.end());
    
    if (vec.size() != times.size()) {
        ostringstream msg;
        msg << BOLDRED << "Times have duplicates! Total: " << vec.size() <<
                " Unique: " << times.size() << RESET;
        throw runtime_error(msg.str());
    }
    
    return;
}

void
AnEnReadNcdf::read_(const netCDF::NcFile & nc, anenTime::FLTs & flts,
        size_t start, size_t count) const {

    if (verbose_ >= Verbose::Detail) cout << "Reading lead times ..." << endl;
    vector<double> vec;

    if (start == 0 || count == 0) {
        readVector(nc, "FLTs", vec);
    } else{
        // If it is partial reading, check the indices
        const auto & dims = nc.getVar("FLTs").getDims();

        if (dims.size() != 1) {
            ostringstream msg;
            msg << BOLDRED << "FLTs should be 1-dimensional." << RESET;
            throw runtime_error(msg.str());
        }

        checkIndex(start, count, dims[0].getSize());
        readVector(nc, "FLTs", vec, {start}, {count});
    }

    flts.insert(flts.end(), vec.begin(), vec.end());

    if (vec.size() != flts.size()) {
        ostringstream msg;
        msg << BOLDRED << "FLTs have duplicates! Total: " << vec.size() <<
                " Unique: " << flts.size() << RESET;
        throw runtime_error(msg.str());
    }

    return;
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

    checkDims(nc, dim_names);
    checkVars(nc, var_names);

    return;
}

void
AnEnReadNcdf::fastInsert_(anenPar::Parameters & parameters, size_t dim_len,
        const vector<string> & names, vector<string> & circulars,
        const vector<double> & weights) const {
    
    if (dim_len != names.size()) {
        ostringstream msg;
        msg << BOLDRED << "#ParameterNames (" << names.size() <<
                ") should be " << dim_len << "." << RESET;
        throw runtime_error(msg.str());
    }
    
    if (dim_len < circulars.size()) {
        ostringstream msg;
        msg << BOLDRED << "#CircularParameters (" << circulars.size() <<
                ") should be no more than " << dim_len << "." << RESET;
        throw runtime_error(msg.str());
    }
    
    if (dim_len != weights.size() || 0 != weights.size()) {
        ostringstream msg;
        msg << BOLDRED << "#ParameterWeights (" << weights.size() <<
                ") should be either 0 or " << dim_len << "." << RESET;
        throw runtime_error(msg.str());    
    }

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

#pragma omp parallel for default(none) schedule(static) num_threads(num_cores) \
    shared(dim_len, vec_parameters, names, circulars, weights)
#endif
    for (size_t i = 0; i < dim_len; i++) {
        auto & parameter = vec_parameters[i];
        string name = names.at(i);
        parameter.setName(name);
        auto it_circular = find(circulars.begin(), circulars.end(), name);
        if (it_circular != circulars.end()) parameter.setCircular(true);
        if (!(weights.empty())) parameter.setWeight(weights.at(i));
    }

    // Insert
    parameters.insert(parameters.end(),
            vec_parameters.begin(), vec_parameters.end());

    // Check for duplicates
    if (parameters.size() != vec_parameters.size()) {
        ostringstream msg;
        msg << BOLDRED << "Parameters have duplicates! Input: " <<
                vec_parameters.size() << " Unique: " <<
                parameters.size() << RESET;
        throw runtime_error(msg.str());
    }

    return;
}

void 
AnEnReadNcdf::fastInsert_(anenSta::Stations & stations, size_t dim_len,
        const vector<string> & names, const vector<double> & xs,
        const vector<double> & ys) const {

    if (dim_len != names.size()) {
        ostringstream msg;
        msg << BOLDRED << "#StationNames (" << names.size() <<
                ") should be " << dim_len << "." << RESET;
        throw runtime_error(msg.str());
    }

    if (dim_len < xs.size()) {
        ostringstream msg;
        msg << BOLDRED << "#Xs (" << xs.size() << ") should be " <<
                dim_len << "." << RESET;
        throw runtime_error(msg.str());
    }

    if (dim_len < ys.size()) {
        ostringstream msg;
        msg << BOLDRED << "#Ys (" << ys.size() << ") should be " <<
                dim_len << "." << RESET;
        throw runtime_error(msg.str());
    }

    // Construct Station object
    vector<anenSta::Station> vec_stations(dim_len);

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
    shared(dim_len, vec_stations, xs, ys, names) num_threads(num_cores)
#endif
    for (size_t i = 0; i < dim_len; i++) {
        auto & station = vec_stations[i];
        if (names.size() != 0) station.setName(names.at(i));
        station.setX(xs.at(i));
        station.setY(ys.at(i));
    }

    // Insert
    stations.insert(stations.end(), vec_stations.begin(), vec_stations.end());

    if (stations.size() != vec_stations.size()) {
        ostringstream msg;
        msg << BOLDRED << "Stations have duplicates! Total: "
                    << vec_stations.size() << " Unique: " << stations.size()
                    << RESET;
        throw runtime_error(msg.str());
    }
    
    return;
}