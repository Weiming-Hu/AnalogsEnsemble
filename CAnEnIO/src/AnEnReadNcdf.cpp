/* 
 * File:   AnEnReadNcdf.cpp
 * Author: Weiming Hu (weiming@psu.edu)
 * 
 * Created on December 30, 2019, 11:10 AM
 */

#include "AnEnReadNcdf.h"
#include "AnEnNames.h"
#include "Ncdf.h"

#if defined(_OPENMP)
#include <omp.h>
#endif

using namespace std;
using namespace Ncdf;
using namespace netCDF;
using namespace AnEnNames;
using namespace AnEnDefaults;

static const size_t _OBSERVATIONS_DIMENSIONS = 3;
static const size_t _FORECASTS_DIMENSIONS = 4;
static const size_t _ANALOGS_DIMENSIONS = 4;

AnEnReadNcdf::AnEnReadNcdf() {
    verbose_ = AnEnDefaults::_VERBOSE;
}

AnEnReadNcdf::AnEnReadNcdf(Verbose verbose) :
verbose_(verbose) {
}

AnEnReadNcdf::AnEnReadNcdf(const AnEnReadNcdf & orig) : AnEnRead(orig) {
    if (this != &orig) verbose_ = orig.verbose_;
}

AnEnReadNcdf::~AnEnReadNcdf() {
}

void
AnEnReadNcdf::readForecasts(const string & file_path,
        Forecasts & forecasts) const {
    // Call the handling function
    readForecasts(file_path, forecasts, {}, {});
    return;
}

void
AnEnReadNcdf::readForecasts(const string & file_path,
        Forecasts & forecasts, vector<size_t> start,
        vector<size_t> count) const {

    if (verbose_ >= Verbose::Progress) {
        cout << "Reading forecast file (" << file_path << ") ..." << endl;
    }

    // Check whether we are reading partial or the entire variable
    bool entire = (start.size() == 0 || count.size() == 0);

    if (!entire) {
        if (start.size() != _FORECASTS_DIMENSIONS ||
                count.size() != _FORECASTS_DIMENSIONS) {
            ostringstream msg;
            msg << BOLDRED << "#start (" << start.size() <<
                    ") and #count (" << count.size() <<
                    ") should both be " << _FORECASTS_DIMENSIONS <<
                    " for Forecasts." << RESET;
            throw runtime_error(msg.str());
        }
    }

    checkPath(file_path, Ncdf::Mode::Read);

    NcFile nc(file_path, NcFile::FileMode::read);
    checkFileType_(nc, FileType::Forecasts);

    // Initialization for meta information
    Parameters parameters;
    Stations stations;
    Times times;
    Times flts;

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

    if (verbose_ >= Verbose::Detail) cout << "Updating dimensions ..." << endl;
    forecasts.setDimensions(parameters, stations, times, flts);

    // Read data values
    read_(nc, forecasts.getValuesPtr(), VAR_DATA, start, count);
    nc.close();

    return;
}

void
AnEnReadNcdf::readObservations(const string & file_path,
        Observations & observations) const {
    readObservations(file_path, observations, {}, {});
    return;
}

void
AnEnReadNcdf::readObservations(const std::string & file_path,
        Observations& observations,
        vector<size_t> start, vector<size_t> count) const {

    if (verbose_ >= Verbose::Progress) {
        cout << "Reading observation file (" << file_path << ") ..." << endl;
    }

    // Check whether we are reading partial or the entire variable
    bool entire = (start.size() == 0 || count.size() == 0);

    if (!entire) {
        if (start.size() != _OBSERVATIONS_DIMENSIONS ||
                count.size() != _OBSERVATIONS_DIMENSIONS) {
            ostringstream msg;
            msg << BOLDRED << "#start (" << start.size() <<
                    ") and #count (" << count.size() <<
                    ") should both be " << _OBSERVATIONS_DIMENSIONS <<
                    " for Observations." << RESET;
            throw runtime_error(msg.str());
        }
    }

    checkPath(file_path, Ncdf::Mode::Read);

    NcFile nc(file_path, NcFile::FileMode::read);
    checkFileType_(nc, FileType::Observations);

    // Initialization for meta information
    Parameters parameters;
    Stations stations;
    Times times;

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
    
    // Read data values
    if (verbose_ >= Verbose::Detail) cout << "Updating dimensions ..." << endl;
    observations.setDimensions(parameters, stations, times);
    read_(nc, observations.getValuesPtr(), VAR_DATA, start, count);  
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
        if (start.size() != _ANALOGS_DIMENSIONS ||
                count.size() != _ANALOGS_DIMENSIONS) {
            ostringstream msg;
            msg << BOLDRED << "#start (" << start.size() <<
                    ") and #count (" << count.size() <<
                    ") should both be " << _ANALOGS_DIMENSIONS <<
                    " for Analogs." << RESET;
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
AnEnReadNcdf::read_(const NcFile & nc, Parameters & parameters,
        size_t start, size_t count) const {

    size_t size_ori = parameters.size();
    if (verbose_ >= Verbose::Detail) {
        if (size_ori == 0) cout << "Reading parameters ..." << endl;
        else cout << "Appending parameters ..." << endl;
    }

    // Check whether we are reading partial or the entire variable
    bool entire = (start == 0 || count == 0);

    size_t dim_len = nc.getDim(DIM_PARS).getSize();
    vector<string> names, circulars;
    vector<double> weights;

    if (!entire) checkIndex(start, count, dim_len);

    // Read the NetCDF variables as several vectors
    readStringVector(nc, VAR_PARNAMES, names, start, count);

    if (varExists(nc, VAR_CIRCULARS)) {
        readStringVector(nc, VAR_CIRCULARS, circulars, start, count);

        if (names.size() < circulars.size()) {
            ostringstream msg;
            msg << BOLDRED << "#" << VAR_CIRCULARS << " (" << circulars.size() <<
                    ") should be no more than " << names.size() << RESET;
            throw runtime_error(msg.str());
        }
    }

    if (varExists(nc, VAR_PARWEIGHTS)) {
        if (entire) readVector(nc, VAR_PARWEIGHTS, weights);
        else readVector(nc, VAR_PARWEIGHTS, weights, {start}, {count});
        
        if (names.size() != weights.size() && 0 != weights.size()) {
            ostringstream msg;
            msg << BOLDRED << "#" << VAR_PARWEIGHTS << " (" << weights.size() <<
                    ") should be either undefined or " << names.size() << RESET;
            throw runtime_error(msg.str());
        }
    }
    
    // Convert vectors to the dimension class
    for (size_t dim_i = size_ori, i = 0; i < names.size(); ++i, ++dim_i) {
        
        // Initialize the parameter with its name
        Parameter parameter(names[i]);
        
        // Set circular if the parameter is found in the list
        const auto & it = find(circulars.begin(), circulars.end(), names[i]);
        if (it != circulars.end()) parameter.setCircular(true);
        
        // Set its weight if it is provided
        if (!(weights.empty())) parameter.setWeight(weights.at(i));
        
        // Push the parameter to the dimension class with an index
        parameters.push_back(Parameters::value_type(dim_i, parameter));
    }
    
    // Check for duplicates
    if (parameters.size() - size_ori != names.size()) {
        ostringstream msg;
        msg << BOLDRED << "Only " << parameters.size() - size_ori << " out of " <<
                names.size() << " have been inserted due to duplicates in " <<
                "parameters!" << RESET;
        throw runtime_error(msg.str());
    }
    
    return;
}

void
AnEnReadNcdf::read_(const NcFile & nc, Stations & stations,
        size_t start, size_t count,
        const string & dim_name_prefix,
        const string & var_name_prefix) const {

    size_t size_ori = stations.size();
    if (verbose_ >= Verbose::Detail) {
        if (size_ori == 0) cout << "Reading stations ..." << endl;
        else cout << "Appending stations ..." << endl;
    }

    // Read the NetCDF variables as several vectors
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
    
    if (xs.size() != ys.size()) {
        ostringstream msg;
        msg << BOLDRED << "#" << VAR_XS << " (" << xs.size() <<
                ") should be the same as #" << VAR_YS << 
                " (" << ys.size() << ")" << RESET;
        throw runtime_error(msg.str());
    }

    if (varExists(nc, var_name_prefix + VAR_STATIONNAMES)) {
        readStringVector(nc, var_name_prefix + VAR_STATIONNAMES,
                names, start, count);
        
        if (xs.size() != names.size()) {
            ostringstream msg;
            msg << BOLDRED << "#" << VAR_STATIONNAMES << " (" << names.size() <<
                    ") should be " << xs.size() << "." << RESET;
            throw runtime_error(msg.str());
        }
    }

    // Convert vectors to the dimension class
    for (size_t dim_i = size_ori, i = 0; i < xs.size(); ++dim_i, ++i) {
        
        // Determine whether the station has a name
        string station_name = AnEnDefaults::_NAME;
        if (names.size() != 0) station_name = names[i];
        
        // Create and push the station to the dimension class
        stations.push_back(Stations::value_type(
                dim_i, Station(xs[i], ys[i], station_name)));
    }
    
    if (stations.size() - size_ori != xs.size()) {
        ostringstream msg;
        msg << BOLDRED << "Only " << stations.size() - size_ori << " out of " <<
                xs.size() << " have been inserted due to duplicates in " <<
                "stations!" << RESET;
        throw runtime_error(msg.str());
    }
    
    return;
}

void
AnEnReadNcdf::read_(const netCDF::NcFile & nc, Times & times,
        const string & var_name, size_t start, size_t count) const {
    
    size_t size_ori = times.size();
    if (verbose_ >= Verbose::Detail) {
        if (size_ori == 0) cout << "Reading times ..." << endl;
        else cout << "Appending times ..." << endl;
    }
    
    // Read the NetCDF variable as a vector
    vector<unsigned int> vec;
    
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
    
    // Create the variable to ensure the timestamps are sorted
    size_t last_timestamp = 0;

    // Convert this vector to the dimension class
    for (size_t i = 0, dim_i = size_ori; i < vec.size(); ++i, ++dim_i) {
        
        if (last_timestamp > vec[i]) {
            ostringstream msg;
            msg << BOLDRED << "Times should be in ascension order!" << RESET;
            throw runtime_error(msg.str());
        }
        
        times.push_back(Times::value_type(dim_i, Time(vec[i])));
        last_timestamp = vec[i];
    }
    
    // Check for duplicates
    if (times.size() - size_ori != vec.size()) {
        ostringstream msg;
        msg << BOLDRED << "Only " << times.size() - size_ori << " out of " <<
                vec.size() << " have been inserted due to duplicates in " <<
                var_name << "!" << RESET;
        throw runtime_error(msg.str());
    }

    return;
}

void
AnEnReadNcdf::checkFileType_(const NcFile & nc, FileType file_type) const {

    vector<string> dim_names, var_names;
    string var_name;
            
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
            dim_names = {DIM_PARS, DIM_STATIONS, DIM_TIMES, DIM_CHARS};
            var_names = {VAR_DATA, VAR_TIMES, VAR_PARNAMES, VAR_XS, VAR_YS};
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

    // Make sure dimensions and variables exist in the NetCDF file
    checkDims(nc, dim_names);
    checkVars(nc, var_names);
    
    // Make sure the data variable has the correct shape
    dim_names.clear();
    switch (file_type) {
        case FileType::Forecasts:
            var_name = VAR_DATA;
            dim_names = {DIM_PARS, DIM_STATIONS, DIM_TIMES, DIM_FLTS};
            break;
        case FileType::Observations:
            var_name = VAR_DATA;
            dim_names = {DIM_PARS, DIM_STATIONS, DIM_TIMES};
            break;
        case FileType::Similarity:
            var_name = VAR_SIMS;
            dim_names = {DIM_STATIONS, DIM_TIMES, DIM_FLTS,
                DIM_MEMBERS, DIM_COLS};
            break;
        case FileType::Analogs:
            var_name = VAR_ANALOGS;
            dim_names = {DIM_STATIONS, DIM_TIMES, DIM_FLTS,
                DIM_MEMBERS, DIM_COLS};
            break;
        case FileType::StandardDeviation:
            var_name = VAR_STD;
            dim_names = {DIM_PARS, DIM_STATIONS, DIM_FLTS};
    }
    
    checkVarShape(nc, var_name, dim_names);

    return;
}