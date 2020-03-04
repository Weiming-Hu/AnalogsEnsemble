/* 
 * File:   AnEnReadNcdf.cpp
 * Author: Weiming Hu (weiming@psu.edu)
 * 
 * Created on December 30, 2019, 11:10 AM
 */

#include "Ncdf.h"
#include "AnEnReadNcdf.h"

using namespace std;
using namespace Ncdf;
using namespace netCDF;

static const size_t _OBSERVATIONS_DIMENSIONS = 3;
static const size_t _FORECASTS_DIMENSIONS = 4;
static const size_t _ANALOGS_DIMENSIONS = 4;

AnEnReadNcdf::AnEnReadNcdf() {
    Config config;
    verbose_ = config.verbose;
}

AnEnReadNcdf::AnEnReadNcdf(Verbose verbose) :
verbose_(verbose) {
}

AnEnReadNcdf::AnEnReadNcdf(const AnEnReadNcdf & orig) {
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
            msg << "#start (" << start.size() << ") and #count (" << count.size()
                    << ") should both be " << _FORECASTS_DIMENSIONS << " for Forecasts";
            throw runtime_error(msg.str());
        }
    }

    Ncdf::checkExists(file_path);
    Ncdf::checkExtension(file_path);

    NcFile nc(file_path, NcFile::FileMode::read);
    checkFileType_(nc, FileType::Forecasts);

    // Initialization for meta information
    Parameters parameters;
    Times times;
    Stations stations;
    Times flts;

    // Read meta information
    if (entire) {
        read(nc, parameters);
        read(nc, stations);
        read(nc, times, Config::_TIMES);
        read(nc, flts, Config::_FLTS);
    } else {
        read(nc, parameters, start[0], count[0]);
        read(nc, stations, start[1], count[1]);
        read(nc, times, Config::_TIMES, start[2], count[2]);
        read(nc, flts, Config::_FLTS, start[3], count[3]);
    }

    if (verbose_ >= Verbose::Detail) cout << "Updating dimensions ..." << endl;
    forecasts.setDimensions(parameters, stations, times, flts);

    // Read data values
    //
    // Forecasts might be in the root group or in a sub group called "Forecasts"
    //
    if (nc.getVar(Config::_DATA).isNull()) {
        
        auto nc_sub = nc.getGroup("Forecasts");
        
        if (nc_sub.isNull()) {
            throw runtime_error("Forecasts can not be found");
            
        } else {
            // Forecasts are found in the sub group
            read(nc_sub, forecasts.getValuesPtr(), Config::_DATA, start, count);
        }
        
    } else {
        // Forecasts are found in the current group
        read(nc, forecasts.getValuesPtr(), Config::_DATA, start, count);
    }

    // The file handler will automatically be closed when it is out of scope.
    // For C++ API older than 4.3.0, this function was not available.
    //
    // nc.close();

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
            msg << "#start (" << start.size() << ") and #count (" << count.size()
                    << ") should both be " << _OBSERVATIONS_DIMENSIONS << " for Observations";
            throw runtime_error(msg.str());
        }
    }

    Ncdf::checkExists(file_path);
    Ncdf::checkExtension(file_path);

    NcFile nc(file_path, NcFile::FileMode::read);
    checkFileType_(nc, FileType::Observations);

    // Initialization for meta information
    Parameters parameters;
    Stations stations;
    Times times;

    // Read meta information
    if (entire) {
        read(nc, parameters);
        read(nc, stations);
        read(nc, times, Config::_TIMES);
    } else {
        read(nc, parameters, start[0], count[0]);
        read(nc, stations, start[1], count[1]);
        read(nc, times, Config::_TIMES, start[2], count[2]);
    }

    // Read data values
    if (verbose_ >= Verbose::Detail) cout << "Updating dimensions ..." << endl;
    observations.setDimensions(parameters, stations, times);

    if (nc.getVar(Config::_DATA).isNull()) {

        auto nc_sub = nc.getGroup("Observations");

        if (nc_sub.isNull()) {
            throw runtime_error("Observations can not be found");

        } else {
            // Forecasts are found in the sub group
            read(nc_sub, observations.getValuesPtr(), Config::_DATA, start, count);
        }

    } else {
        // Forecasts are found in the current group
        read(nc, observations.getValuesPtr(), Config::_DATA, start, count);
    }
    

    // The file handler will automatically be closed when it is out of scope.
    // For C++ API older than 4.3.0, this function was not available.
    //
    // nc.close();

    return;
}

void
AnEnReadNcdf::readAnalogs(const string& file_path, Array4D& analogs,
        const string& var_name, vector<size_t> start, vector<size_t> count) {

    if (verbose_ >= Verbose::Progress) {
        cout << "Reading analog file (" << file_path << ") ..." << endl;
    }

    // Check whether we are reading partial or the entire variable
    bool entire = (start.size() == 0 || count.size() == 0);

    if (!entire) {
        if (start.size() != _ANALOGS_DIMENSIONS ||
                count.size() != _ANALOGS_DIMENSIONS) {
            ostringstream msg;
            msg << "#start (" << start.size() << ") and #count (" << count.size()
                    << ") should both be " << _ANALOGS_DIMENSIONS << " for analogs";
            throw runtime_error(msg.str());
        }
    }

    Ncdf::checkExists(file_path);
    Ncdf::checkExtension(file_path);

    NcFile nc(file_path, NcFile::FileMode::read);

    vector<string> dim_names = {
        Config::_DIM_STATIONS, Config::_DIM_TEST_TIMES,
        Config::_DIM_FLTS, Config::_DIM_ANALOGS
    };

    checkFileType_(nc, FileType::Analogs);
    checkVarShape(nc, var_name, dim_names);

    // Read meta information
    size_t num_stations, num_test_times, num_flts, num_analogs;
    
    if (entire) {
        num_stations = nc.getDim(Config::_DIM_STATIONS).getSize();
        num_test_times = nc.getDim(Config::_TIMES).getSize();
        num_flts = nc.getDim(Config::_FLTS).getSize();
        num_analogs = nc.getDim(Config::_DIM_ANALOGS).getSize();
    } else {
        num_stations = count[0];
        num_test_times = count[1];
        num_flts = count[2];
        num_analogs = count[3];
    }
    
    // Read data values
    if (verbose_ >= Verbose::Detail) cout << "Updating dimensions ..." << endl;
    analogs.resize(num_stations, num_test_times, num_flts, num_analogs);
    
    auto var = nc.getVar(var_name);
    
    if (var.isNull()) {
        throw runtime_error("Variable cannot be found");
    } else {
        read(nc, analogs.getValuesPtr(), var_name, start, count);
    }

    return;
}

void
AnEnReadNcdf::read(const NcGroup & nc, Parameters & parameters,
        size_t start, size_t count) const {

    size_t size_ori = parameters.size();
    if (verbose_ >= Verbose::Detail) {
        if (size_ori == 0) cout << "Reading parameters ..." << endl;
        else cout << "Appending parameters ..." << endl;
    }

    // Check whether we are reading partial or the entire variable
    bool entire = (start == 0 || count == 0);

    size_t dim_len = nc.getDim(Config::_DIM_PARS).getSize();
    vector<string> names, circulars;

    if (!entire) checkIndex(start, count, dim_len);

    // Read the NetCDF variables as several vectors
    readStringVector(nc, Config::_PAR_NAMES, names, start, count);

    if (varExists(nc, Config::_CIRCULARS)) {
        readStringVector(nc, Config::_CIRCULARS, circulars, start, count);

        if (names.size() < circulars.size()) {
            ostringstream msg;
            msg << "#" << Config::_CIRCULARS << " (" << circulars.size()
                    << ") should be no more than " << names.size();
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

        // Push the parameter to the dimension class with an index
        parameters.push_back(parameter);
    }

    // Check for duplicates
    if (parameters.size() - size_ori != names.size()) {
        ostringstream msg;
        msg << "Only " << parameters.size() - size_ori << " out of " << names.size()
                << " have been inserted due to duplicates in " << "parameters!";
        throw runtime_error(msg.str());
    }

    return;
}

void
AnEnReadNcdf::read(const NcGroup & nc, Stations & stations,
        size_t start, size_t count,
        const string & dim_name_prefix,
        const string & var_name_prefix) const {

    size_t size_ori = stations.size();
    if (verbose_ >= Verbose::Detail) {
        if (size_ori == 0) cout << "Reading stations ..." << endl;
        else cout << "Appending stations ..." << endl;
    }

    // Read the NetCDF variables as several vectors
    size_t dim_len = nc.getDim(dim_name_prefix + Config::_DIM_STATIONS).getSize();
    vector<string> names;
    vector<double> xs, ys;

    if (start == 0 || count == 0) {
        readVector(nc, var_name_prefix + Config::_XS, xs);
        readVector(nc, var_name_prefix + Config::_YS, ys);
    } else {
        checkIndex(start, count, dim_len);
        readVector(nc, var_name_prefix + Config::_XS, xs,{start},
        {
            count
        });
        readVector(nc, var_name_prefix + Config::_YS, ys,{start},
        {
            count
        });
    }

    if (xs.size() != ys.size()) {
        ostringstream msg;
        msg << "#" << Config::_XS << " (" << xs.size() << ") should be the same as #"
                << Config::_YS << " (" << ys.size() << ")";
        throw runtime_error(msg.str());
    }

    if (varExists(nc, var_name_prefix + Config::_STATION_NAMES)) {
        readStringVector(nc, var_name_prefix + Config::_STATION_NAMES,
                names, start, count);

        if (xs.size() != names.size()) {
            ostringstream msg;
            msg << "#" << Config::_STATION_NAMES << " (" << names.size() <<
                    ") should be " << xs.size() << ".";
            throw runtime_error(msg.str());
        }
    }

    // Convert vectors to the dimension class
    for (size_t dim_i = size_ori, i = 0; i < xs.size(); ++dim_i, ++i) {

        // Determine whether the station has a name
        string station_name = Config::_NAME;
        if (names.size() != 0) station_name = names[i];

        // Create and push the station to the dimension class
        stations.push_back(Station(xs[i], ys[i], station_name));
    }

    if (stations.size() - size_ori != xs.size()) {
        ostringstream msg;
        msg << "Only " << stations.size() - size_ori << " out of " << xs.size()
                << " have been inserted due to duplicates in " << "stations!";
        throw runtime_error(msg.str());
    }

    return;
}

void
AnEnReadNcdf::read(const NcGroup & nc, Times & times,
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
            msg << var_name << " should be 1-dimensional.";
            throw runtime_error(msg.str());
        }

        checkIndex(start, count, dims[0].getSize());
        readVector(nc, var_name, vec,{start},
        {
            count
        });
    }

    // Create the variable to ensure the timestamps are sorted
    size_t last_timestamp = 0;

    // Convert this vector to the dimension class
    for (size_t i = 0, dim_i = size_ori; i < vec.size(); ++i, ++dim_i) {

        if (last_timestamp > vec[i]) {
            throw runtime_error("Times should be in ascension order!");
        }

        times.push_back(vec[i]);
        last_timestamp = vec[i];
    }

    // Check for duplicates
    if (times.size() - size_ori != vec.size()) {
        ostringstream msg;
        msg << "Only " << times.size() - size_ori << " out of " << vec.size()
                << " have been inserted due to duplicates in " << var_name;
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
                Config::_DIM_PARS, Config::_DIM_STATIONS,
                Config::_DIM_TIMES, Config::_DIM_FLTS, Config::_DIM_CHARS
            };
            var_names = {
                Config::_DATA, Config::_FLTS, Config::_TIMES,
                Config::_PAR_NAMES, Config::_XS, Config::_YS
            };
            break;
        case FileType::Observations:
            dim_names = {
                Config::_DIM_PARS, Config::_DIM_STATIONS,
                Config::_DIM_TIMES, Config::_DIM_CHARS
            };
            var_names = {
                Config::_DATA, Config::_TIMES, Config::_PAR_NAMES,
                Config::_XS, Config::_YS
            };
            break;
        case FileType::Analogs:
            dim_names = {
                Config::_DIM_STATIONS, Config::_DIM_TEST_TIMES,
                Config::_DIM_FLTS, Config::_DIM_ANALOGS
            };
            break;
    }

    // Make sure dimensions and variables exist in the NetCDF file
    checkDims(nc, dim_names);
    checkVars(nc, var_names);

    // Make sure the data variable has the correct shape
    dim_names.clear();
    switch (file_type) {
        case FileType::Forecasts:
            var_name = Config::_DATA;
            dim_names = {
                Config::_DIM_PARS, Config::_DIM_STATIONS,
                Config::_DIM_TIMES, Config::_DIM_FLTS
            };
            break;
        case FileType::Observations:
            var_name = Config::_DATA;
            dim_names = {Config::_DIM_PARS, Config::_DIM_STATIONS, Config::_DIM_TIMES};
            break;
    }

    if (!var_name.empty()) checkVarShape(nc, var_name, dim_names);

    return;
}
