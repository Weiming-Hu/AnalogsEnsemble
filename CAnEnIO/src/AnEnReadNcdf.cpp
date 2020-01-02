/* 
 * File:   AnEnReadNcdf.cpp
 * Author: wuh20
 * 
 * Created on December 30, 2019, 11:10 AM
 */

#include "AnEnReadNcdf.h"
#include "Ncdf.h"

#include <array>

#if defined(_OPENMP)
#include <omp.h>
#endif

using namespace Ncdf;
using namespace netCDF;
using namespace std;

// The name prefix for member dimensions in file type Analogs
const string MEMBER_DIM_PREFIX = "member_";

// The name prefix for member variables in file type Analogs
const string MEMBER_VAR_PREFIX = "Member";

// The name prefix for search dimensions in file type SimilarityMatrices
const string SEARCH_DIM_PREFIX = "search_";

// The name prefix for search variables in file type SimilarityMatrices
const string SEARCH_VAR_PREFIX = "Search";

// If data have a length longer than this limit, I/O will be parallelized.
const size_t SERIAL_LENGTH_LIMIT = 1000;

// Define the names for dimensions in NetCDF files
const string DIM_ENTRIES = "num_entries";
const string DIM_STATIONS = "num_stations";
const string DIM_TIMES = "num_times";
const string DIM_FLTS = "num_flts";
const string DIM_MEMBERS = "num_members";
const string DIM_COLS = "num_cols";
const string DIM_PARS = "num_parameters";
const string DIM_CHARS = "num_chars";

// Define the names for variables in NetCDF files
const string VAR_DATA = "Data";
const string VAR_SIMS = "SimilarityMatrices";
const string VAR_ANALOGS = "Analogs";
const string VAR_STD = "StandardDeviation";
const string VAR_PARNAMES = "ParameterNames";
const string VAR_CIRCULARS = "ParameterCirculars";
const string VAR_PARWEIGHTS = "ParameterWeights";
const string VAR_XS = "Xs";
const string VAR_YS = "Ys";
const string VAR_STATIONNAMES = "StationNames";
const string VAR_TIMES = "Times";
const string VAR_FLTS = "FLTs";


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

    checkPath(file_path, Ncdf::Mode::Read);

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
        read_(nc, times, VAR_TIMES);
        read_(nc, flts, VAR_FLTS);
    } else {
        read_(nc, parameters, start[0], count[0]);
        read_(nc, stations, start[1], count[1]);
        read_(nc, times, VAR_TIMES, start[2], count[2]);
        read_(nc, flts, VAR_FLTS, start[3], count[3]);
    }

    forecasts.setParameters(parameters);
    forecasts.setStations(stations);
    forecasts.setTimes(times);
    forecasts.setFlts(flts);

    // Read data values
    if (verbose_ >= Verbose::Detail) cout << "Updating dimensions ..." << endl;
    forecasts.updateDataDims();
    read_(nc, forecasts.data().data(), VAR_DATA, start, count);
    
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

    checkPath(file_path, Ncdf::Mode::Read);

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
        read_(nc, times, VAR_TIMES);
    } else {
        read_(nc, parameters, start[0], count[0]);
        read_(nc, stations, start[1], count[1]);
        read_(nc, times, VAR_TIMES, start[2], count[2]);
    }

    observations.setParameters(parameters);
    observations.setStations(stations);
    observations.setTimes(times);
    
    // Read data values
    if (verbose_ >= Verbose::Detail) cout << "Updating dimensions ..." << endl;
    observations.updateDataDims();
    read_(nc, observations.data().data(), VAR_DATA, start, count);
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
        sims.setMaxEntries(nc.getDim(DIM_ENTRIES).getSize());
        sims.resize(nc.getDim(DIM_STATIONS).getSize(),
                nc.getDim(DIM_TIMES).getSize(),
                nc.getDim(DIM_FLTS).getSize());
    } else {
        sims.setMaxEntries(count[3]);
        sims.resize(count[0], count[1], count[2]);
        
    }

    read_(nc, sims.data(), VAR_SIMS, start, count);
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
                [nc.getDim(DIM_STATIONS).getSize()]
                [nc.getDim(DIM_TIMES).getSize()]
                [nc.getDim(DIM_FLTS).getSize()]
                [nc.getDim(DIM_MEMBERS).getSize()]
                [nc.getDim(DIM_COLS).getSize()]);
    } else {
        analogs.resize(boost::extents
                [count[0]][count[1]][count[2]][count[3]][count[4]]);
    }
    
    read_(nc, analogs.data(), VAR_ANALOGS, start, count);
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
                [nc.getDim(DIM_PARS).getSize()]
                [nc.getDim(DIM_STATIONS).getSize()]
                [nc.getDim(DIM_FLTS).getSize()]);
    } else {
        sds.resize(boost::extents
                [count[0]][count[1]][count[2]]);
    }

    read_(nc, sds.data(), VAR_STD, start, count);
    nc.close();
    return;
}

void
AnEnReadNcdf::read_(const NcFile & nc, anenPar::Parameters & parameters,
        size_t start, size_t count) const {

    if (verbose_ >= Verbose::Detail) cout << "Reading parameters ..." << endl;

    // Check whether we are reading partial or the entire variable
    bool entire = (start == 0 || count == 0);

    size_t dim_len = nc.getDim(DIM_PARS).getSize();
    vector<string> names, circulars;
    vector<double> weights;

    if (!entire) checkIndex(start, count, dim_len);

    readStringVector(nc, VAR_PARNAMES, names, start, count);

    if (varExists(nc, VAR_CIRCULARS)) {
        readStringVector(nc, VAR_CIRCULARS,
                circulars, start, count);
    }

    if (varExists(nc, VAR_PARWEIGHTS)) {
        if (entire) readVector(nc, VAR_PARWEIGHTS, weights);
        else readVector(nc, VAR_PARWEIGHTS, weights, {start}, {count});
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

    size_t dim_len = nc.getDim(dim_name_prefix + DIM_STATIONS).getSize();
    vector<string> names;
    vector<double> xs, ys;

    if (start == 0 || count == 0) {
        readVector(nc, var_name_prefix + VAR_XS, xs);
        readVector(nc, var_name_prefix + VAR_YS, ys);
    } else {
        checkIndex(start, count, dim_len);
        readVector(nc, var_name_prefix + VAR_XS, xs, {start}, {count});
        readVector(nc, var_name_prefix + VAR_YS, ys, {start}, {count});
    }

    if (varExists(nc, var_name_prefix + VAR_STATIONNAMES)) {
        readStringVector(nc, var_name_prefix + VAR_STATIONNAMES,
                names, start, count);
    }

    fastInsert_(stations, xs.size(), names, xs, ys);
    return;
}

void
AnEnReadNcdf::read_(const netCDF::NcFile & nc, anenTime::Times & times,
        const string & var_name, size_t start, size_t count) const {
    
    if (verbose_ >= Verbose::Detail) cout << "Reading times ..." << endl;
    vector<double> vec;
    
    if (start == 0 || count == 0) {
        readVector(nc, var_name, vec);
    } else {
        // If it is partial reading, check the indices
        const auto & dims = nc.getVar(var_name).getDims();
        
        if (dims.size() != 1) {
            ostringstream msg;
            msg << BOLDRED << var_name << " should be 1-dimensional." << RESET;
            throw runtime_error(msg.str());
        }
        
        checkIndex(start, count, dims[0].getSize());
        readVector(nc, var_name, vec, {start}, {count});
    }

    times.insert(times.end(), vec.begin(), vec.end());
    
    if (vec.size() != times.size()) {
        ostringstream msg;
        msg << BOLDRED << var_name << " have duplicates! Total: " <<
                vec.size() << " Unique: " << times.size() << RESET;
        throw runtime_error(msg.str());
    }
    
    return;
}

void
AnEnReadNcdf::read_(const netCDF::NcFile & nc, anenTime::FLTs & flts,
        const string & var_name, size_t start, size_t count) const {

    if (verbose_ >= Verbose::Detail) cout << "Reading lead times ..." << endl;
    vector<double> vec;

    if (start == 0 || count == 0) {
        readVector(nc, var_name, vec);
    } else{
        // If it is partial reading, check the indices
        const auto & dims = nc.getVar(var_name).getDims();

        if (dims.size() != 1) {
            ostringstream msg;
            msg << BOLDRED << var_name << " should be 1-dimensional." << RESET;
            throw runtime_error(msg.str());
        }

        checkIndex(start, count, dims[0].getSize());
        readVector(nc, var_name, vec, {start}, {count});
    }

    flts.insert(flts.end(), vec.begin(), vec.end());

    if (vec.size() != flts.size()) {
        ostringstream msg;
        msg << BOLDRED << var_name << " have duplicates! Total: " <<
                vec.size() << " Unique: " << flts.size() << RESET;
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
                DIM_PARS, DIM_STATIONS,
                DIM_TIMES, DIM_FLTS, DIM_CHARS
            };
            var_names = {
                VAR_DATA, VAR_FLTS, VAR_TIMES, VAR_PARNAMES, VAR_XS, VAR_YS
            };
            break;
        case FileType::Observations:
            dim_names = {
                DIM_PARS, DIM_STATIONS, DIM_TIMES, DIM_CHARS
            };
            var_names = {

                VAR_DATA, VAR_TIMES, VAR_PARNAMES, VAR_XS, VAR_YS
            };
            break;
        case FileType::Similarity:
            dim_names = {
                DIM_STATIONS, DIM_TIMES, DIM_FLTS, DIM_ENTRIES,
                DIM_COLS, DIM_PARS, DIM_CHARS,
                SEARCH_DIM_PREFIX + DIM_STATIONS,
                SEARCH_DIM_PREFIX + DIM_TIMES
            };
            var_names = {
                VAR_SIMS, VAR_PARNAMES, VAR_STATIONNAMES,
                VAR_XS, VAR_YS, VAR_TIMES, VAR_FLTS,
                SEARCH_VAR_PREFIX + VAR_TIMES,
                SEARCH_VAR_PREFIX + VAR_STATIONNAMES,
                SEARCH_VAR_PREFIX + VAR_XS,
                SEARCH_VAR_PREFIX + VAR_YS
            };
            break;
        case FileType::Analogs:
            dim_names = {
                DIM_STATIONS, DIM_TIMES, DIM_FLTS, DIM_MEMBERS,
                DIM_COLS, DIM_CHARS,
                MEMBER_DIM_PREFIX + DIM_STATIONS,
                MEMBER_DIM_PREFIX + DIM_TIMES
            };
            var_names = {
                VAR_ANALOGS, VAR_STATIONNAMES,
                VAR_XS, VAR_YS, VAR_TIMES, VAR_FLTS,
                MEMBER_VAR_PREFIX + VAR_TIMES,
                MEMBER_VAR_PREFIX + VAR_STATIONNAMES,
                MEMBER_VAR_PREFIX + VAR_XS,
                MEMBER_VAR_PREFIX + VAR_YS
            };
            break;
        case FileType::StandardDeviation:
            dim_names = {DIM_PARS, DIM_STATIONS, DIM_FLTS};
            var_names = {
                VAR_STD, VAR_FLTS, VAR_STATIONNAMES,
                VAR_PARNAMES, VAR_XS, VAR_YS
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
        msg << BOLDRED << "#" << VAR_PARNAMES << " (" << names.size() <<
                ") should be " << dim_len << "." << RESET;
        throw runtime_error(msg.str());
    }
    
    if (dim_len < circulars.size()) {
        ostringstream msg;
        msg << BOLDRED << "#" << VAR_CIRCULARS << " (" << circulars.size() <<
                ") should be no more than " << dim_len << "." << RESET;
        throw runtime_error(msg.str());
    }
    
    if (dim_len != weights.size() || 0 != weights.size()) {
        ostringstream msg;
        msg << BOLDRED << "#" << VAR_PARWEIGHTS << " (" << weights.size() <<
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
        msg << BOLDRED << "#" << VAR_STATIONNAMES << " (" << names.size() <<
                ") should be " << dim_len << "." << RESET;
        throw runtime_error(msg.str());
    }

    if (dim_len < xs.size()) {
        ostringstream msg;
        msg << BOLDRED << "#" << VAR_XS << " (" << xs.size() <<
                ") should be " << dim_len << "." << RESET;
        throw runtime_error(msg.str());
    }

    if (dim_len < ys.size()) {
        ostringstream msg;
        msg << BOLDRED << "#" << VAR_YS << " (" << ys.size() <<
                ") should be " << dim_len << "." << RESET;
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