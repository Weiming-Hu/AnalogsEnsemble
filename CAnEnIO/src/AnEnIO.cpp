/* 
 * File:   AnEnIO.cpp
 * Author: Weiming Hu (weiming@psu.edu)
 * 
 * Created on May 4, 2018, 11:16 AM
 */

#include "AnEnIO.h"
#include "boost/filesystem.hpp"

#include <algorithm>
#include <exception>

#if defined(_OPENMP)
#include <omp.h>
#endif

size_t _max_chars = 50;

namespace filesys = boost::filesystem;
using namespace netCDF;
using namespace std;

using errorType = AnEnIO::errorType;

AnEnIO::AnEnIO(string mode, string file_path) :
mode_(mode), file_path_(file_path) {
    handleError(checkMode());
    handleError(checkFilePath());
}

AnEnIO::AnEnIO(string mode, string file_path, string file_type) :
mode_(mode), file_path_(file_path), file_type_(file_type) {
    handleError(checkMode());
    handleError(checkFilePath());
    if (mode_ != "Write") handleError(checkFileType());
}

AnEnIO::AnEnIO(string mode, string file_path,
        string file_type, int verbose) :
mode_(mode), file_path_(file_path),
file_type_(file_type), verbose_(verbose) {
    handleError(checkMode());
    handleError(checkFilePath());
    if (mode_ != "Write") handleError(checkFileType());
}

AnEnIO::AnEnIO(string mode, string file_path,
        string file_type, int verbose,
        vector<string> required_variables,
        vector<string> optional_variables) :
mode_(mode), file_path_(file_path),
file_type_(file_type), verbose_(verbose),
required_variables_(required_variables),
optional_variables_(optional_variables) {
    handleError(checkMode());
    handleError(checkFilePath());
    if (mode_ != "Write") handleError(checkFileType());
}

AnEnIO::~AnEnIO() {
}

errorType
AnEnIO::checkMode() const {
    if (verbose_ >= 3) {
        cout << "Checking mode ..." << endl;
    }

    if (mode_ != "Read" && mode_ != "Write") {
        if (verbose_ >= 1) {
            cout << BOLDRED << "Error: Mode should be 'Read' or 'Write'."
                    << endl;
        }

        return (UNKNOWN_MODE);
    }

    return (SUCCESS);
}

errorType
AnEnIO::checkFilePath() const {

    if (verbose_ >= 3) {
        cout << "Checking file (" << file_path_ << ") ..." << endl;
    }

    bool file_exists = filesys::exists(file_path_);

    if (file_exists && mode_ == "Write") {
        if (add_) {

            if (verbose_ >= 3) cout << "Writing to existing file "
                    << file_path_ << "!" << RESET << endl;

        } else {
            if (verbose_ >= 1) {
                cout << BOLDRED << "Error: File exists at "
                        << file_path_ << RESET << endl;
            }
        }

        return (FILE_EXISTS);

    } else if (!file_exists && mode_ == "Read") {

        if (verbose_ >= 1) {
            cout << BOLDRED << "Error: File not found "
                    << file_path_ << RESET << endl;
        }

        return (FILE_NOT_FOUND);
    } else {

        filesys::path boost_path(file_path_);

        if (boost_path.has_extension()) {
            string ext = boost_path.extension().string();

            if (mode_ == "Matrix"  || ext == ".txt") return (SUCCESS);
            if (ext == ".nc") return (SUCCESS);
        }

        if (verbose_ >= 1) {
            cout << BOLDRED << "Error: Unknown file extension "
                    << file_path_ << RESET << endl;
        }

        return (UNKOWN_FILE_TYPE);
    }
}

errorType
AnEnIO::checkFileType() const {

    if (verbose_ >= 3) {
        cout << "Checking file type (" << file_type_ << ") ..." << endl;
    }

    if (mode_ == "Write") {
        if (verbose_ >= 2) cout << RED << "Warning: No need to check "
                << "for mode 'Write'." << RESET << endl;
        return (SUCCESS);
    } else if (mode_ == "Read") {

        vector<string> dim_names, var_names;

        if (file_type_ == "Observations") {

            dim_names = {"num_parameters", "num_stations", "num_times", "num_chars"};
            var_names = {"Data", "Times", "StationNames", "ParameterNames"};

        } else if (file_type_ == "Forecasts") {

            dim_names = {"num_parameters", "num_stations",
                "num_times", "num_flts", "num_chars"};
            var_names = {"Data", "FLTs", "Times",
                "StationNames", "ParameterNames"};

        } else if (file_type_ == "Similarity") {

            dim_names = {"num_test_stations", "num_test_times",
                "num_flts", "num_entries", "num_cols"};
            var_names = {"SimilarityMatrices"};

        } else if (file_type_ == "Analogs") {

            dim_names = {"num_test_stations", "num_test_times",
                "num_flts", "num_members", "num_cols"};
            var_names = {"Analogs"};

        } else if (file_type_ == "Matrix") {
            
            if (verbose_ >= 2) cout << RED << "Warning: No available check "
                    << "for file type Matrix." << RESET << endl;
            return (SUCCESS);
            
        } else {
            if (verbose_ >= 1) {
                cout << BOLDRED << "Error: Unknown file type "
                        << file_type_ << RESET << endl;
            }
            return (UNKOWN_FILE_TYPE);
        }

        for (const auto & name : dim_names) {
            handleError(checkDim(name));
        }

        for (const auto & name : var_names) {
            handleError(checkVariable(name, false));
        }

        return (SUCCESS);
    } else {
        if (verbose_ >= 1) cout << BOLDRED << "Error: Unknown mode."
                << RESET << endl;
        return (WRONG_MODE);
    }
}

errorType
AnEnIO::checkVariable(string var_name, bool optional) const {

    if (verbose_ >= 3) {
        cout << "Checking variable (" << var_name << ") ..." << endl;
    }

    if (mode_ != "Read") {
        if (verbose_ >= 1) cout << BOLDRED << "Error: Mode should be 'Read'."
                << RESET << endl;
        return (WRONG_MODE);
    }

    NcFile nc(file_path_, NcFile::FileMode::read);
    bool is_null = nc.getVar(var_name).isNull();
    nc.close();

    if (is_null) {
        if (optional) {

            if (verbose_ >= 2) {
                cout << RED << "Warning: Optional variable (" << var_name
                        << ") is missing in file (" << file_path_
                        << ")!" << RESET << endl;
            }
            return (OPTIONAL_VARIABLE_MISSING);
        } else {

            if (verbose_ >= 1) {
                cout << BOLDRED << "Error: Variable (" << var_name
                        << ") is missing in file (" << file_path_
                        << ")!" << RESET << endl;
                return (REQUIRED_VARIABLE_MISSING);
            }
        }
    }

    return (SUCCESS);
}

errorType
AnEnIO::checkDim(string dim_name) const {

    if (verbose_ >= 3) {
        cout << "Checking dimension (" << dim_name << ") ..." << endl;
    }

    if (mode_ != "Read") {
        if (verbose_ >= 1) cout << BOLDRED << "Error: Mode should be 'Read'."
                << RESET << endl;
        return (WRONG_MODE);
    }

    NcFile nc(file_path_, NcFile::FileMode::read);
    NcDim dim = nc.getDim(dim_name);
    bool is_null = dim.isNull();
    nc.close();

    if (is_null) {
        if (verbose_ >= 1) {
            cout << BOLDRED << "Error: Dimension (" << dim_name
                    << ") missing!" << RESET << endl;
        }
        return (DIMENSION_MISSING);
    }

    return (SUCCESS);
}

errorType
AnEnIO::checkVariables() const {

    // This variable allows the function to check all the variables and return
    // the final status at the end of the check.
    //
    errorType return_status = SUCCESS;

    if (verbose_ >= 3) {
        cout << "Checking variables ..." << endl;
    }

    if (mode_ != "Read") {
        if (verbose_ >= 1) cout << BOLDRED << "Error: Mode should be 'Read'."
                << RESET << endl;
        return (WRONG_MODE);
    }

    if (required_variables_.empty()) {
        if (verbose_ >= 2)
            cout << RED << "Warning: Required variables are not set!"
                << RESET << endl;
        return (return_status);
    }

    for (const auto & name : optional_variables_) {
        if (SUCCESS != checkVariable(name, true))
            return_status = OPTIONAL_VARIABLE_MISSING;
    }

    for (const auto & name : required_variables_) {
        if (SUCCESS != checkVariable(name, false))
            return_status = REQUIRED_VARIABLE_MISSING;
    }
    return (return_status);
}

errorType
AnEnIO::checkDimensions() const {

    // This variable allows the function to check all the dimensions and return
    // the final status at the end of the check.
    //
    errorType return_status = SUCCESS;

    if (verbose_ >= 3) {
        cout << "Checking dimensions ..." << endl;
    }

    if (mode_ != "Read") {
        if (verbose_ >= 1) cout << BOLDRED << "Error: Mode should be 'Read'."
                << RESET << endl;
        return (WRONG_MODE);
    }

    if (required_dimensions_.empty()) {
        if (verbose_ >= 2)
            cout << RED << "Warning: Required dimensions are not set!"
                << RESET << endl;
        return (return_status);
    }

    for (const auto & name : required_dimensions_) {
        if (SUCCESS != checkDim(name))
            return_status = DIMENSION_MISSING;
    }
    return (return_status);
}

errorType
AnEnIO::readObservations(Observations & observations) {

    if (verbose_ >= 3) {
        cout << "Reading observation file ..." << endl;
    }

    if (mode_ != "Read") {
        if (verbose_ >= 1) cout << BOLDRED << "Error: Mode should be 'Read'."
                << RESET << endl;
        return (WRONG_MODE);
    }

    if (file_type_ != "Observations") {
        if (verbose_ >= 1) {
            cout << BOLDGREEN << "Error: File type should be Observations."
                    << RESET << endl;
        }
        return (WRONG_FILE_TYPE);
    }

    handleError(checkFilePath());
    handleError(checkFileType());

    // Read meta information
    anenPar::Parameters parameters;
    anenSta::Stations stations;
    anenTime::Times times;

    handleError(readParameters(parameters));
    handleError(readStations(stations));
    handleError(readTimes(times));

    // Read data
    if (verbose_ >= 3) cout << "Reading observation values from file ("
            << file_path_ << ") ..." << endl;
    vector<double> vals;
    handleError(read_vector_("Data", vals));

    observations.setParameters(parameters);
    observations.setStations(stations);
    observations.setTimes(times);
    observations.updateDataDims();
    observations.setValues(vals);

    return (SUCCESS);
}

errorType
AnEnIO::readObservations(Observations& observations, vector<size_t> start,
        vector<size_t> count, vector<ptrdiff_t> stride) {

    if (verbose_ >= 3) {
        cout << "Reading observation file ..." << endl;
    }

    if (mode_ != "Read") {
        if (verbose_ >= 1) cout << BOLDRED << "Error: Mode should be 'Read'."
                << RESET << endl;
        return (WRONG_MODE);
    }

    if (file_type_ != "Observations") {
        if (verbose_ >= 1) {
            cout << BOLDGREEN << "Error: File type should be Observations."
                    << RESET << endl;
        }
        return (WRONG_FILE_TYPE);
    }

    handleError(checkFilePath());
    handleError(checkFileType());

    // Check indices
    if (start.size() != 3) {
        if (verbose_ >= 1) cout << BOLDRED
                << "Error: Start should have 3 indices for observations."
                << RESET << endl;
        return (WRONG_INDEX_SHAPE);
    }

    if (count.size() != 3) {
        if (verbose_ >= 1) cout << BOLDRED
                << "Error: Count should have 3 indices for observations."
                << RESET << endl;
        return (WRONG_INDEX_SHAPE);
    }

    if (stride.size() != 3) {
        if (verbose_ >= 1) cout << BOLDRED
                << "Error: Stride should have 3 indices for observations."
                << RESET << endl;
        return (WRONG_INDEX_SHAPE);
    }

    // Read meta information
    anenPar::Parameters parameters;
    anenSta::Stations stations;
    anenTime::Times times;

    handleError(readParameters(parameters, start[0], count[0], stride[0]));
    handleError(readStations(stations, start[1], count[1], stride[1]));
    handleError(readTimes(times, start[2], count[2], stride[2]));

    // Read data
    if (verbose_ >= 3)
        cout << "Reading observation values from file ("
            << file_path_ << ") ..." << endl;
    vector<double> vals;
    handleError(read_vector_("Data", vals, start, count, stride));

    observations.setParameters(parameters);
    observations.setStations(stations);
    observations.setTimes(times);
    observations.updateDataDims();
    observations.setValues(vals);

    return (SUCCESS);
}

errorType
AnEnIO::readForecasts(Forecasts & forecasts) {

    if (verbose_ >= 3) {
        cout << "Reading forecast file ..." << endl;
    }

    if (mode_ != "Read") {
        if (verbose_ >= 1) cout << BOLDRED << "Error: Mode should be 'Read'."
                << RESET << endl;
        return (WRONG_MODE);
    }

    if (file_type_ != "Forecasts") {
        if (verbose_ >= 1) {
            cout << BOLDGREEN << "Error: File type should be Forecasts."
                    << RESET << endl;
        }
        return (WRONG_FILE_TYPE);
    }

    handleError(checkFilePath());
    handleError(checkFileType());

    anenPar::Parameters parameters;
    anenSta::Stations stations;
    anenTime::Times times;
    anenTime::FLTs flts;

    // Read meta information
    handleError(readParameters(parameters));
    handleError(readStations(stations));
    handleError(readTimes(times));
    handleError(readFLTs(flts));

    // Read data
    if (verbose_ >= 3) cout << "Reading forecast values from file ("
            << file_path_ << ") ..." << endl;
    vector<double> vals;
    handleError(read_vector_("Data", vals));

    forecasts.setParameters(parameters);
    forecasts.setStations(stations);
    forecasts.setTimes(times);
    forecasts.setFlts(flts);
    forecasts.updateDataDims();
    forecasts.setValues(vals);

    return (SUCCESS);
}

errorType
AnEnIO::readForecasts(Forecasts& forecasts,
        vector<size_t> start, vector<size_t> count,
        vector<ptrdiff_t> stride) {

    if (verbose_ >= 3) {
        cout << "Reading forecast file ..." << endl;
    }

    if (mode_ != "Read") {
        if (verbose_ >= 1) cout << BOLDRED << "Error: Mode should be 'Read'."
                << RESET << endl;
        return (WRONG_MODE);
    }

    if (file_type_ != "Forecasts") {
        if (verbose_ >= 1) {
            cout << BOLDGREEN << "Error: File type should be Forecasts."
                    << RESET << endl;
        }
        return (WRONG_FILE_TYPE);
    }

    handleError(checkFilePath());
    handleError(checkFileType());

    // Check indices
    if (start.size() != 4) {
        if (verbose_ >= 1) cout << BOLDRED
                << "Error: Start should have 4 indices for forecasts."
                << RESET << endl;
        return (WRONG_INDEX_SHAPE);
    }

    if (count.size() != 4) {
        if (verbose_ >= 1) cout << BOLDRED
                << "Error: Count should have 4 indices for forecasts."
                << RESET << endl;
        return (WRONG_INDEX_SHAPE);
    }

    if (stride.size() != 4) {
        if (verbose_ >= 1) cout << BOLDRED
                << "Error: Stride should have 4 indices for forecasts."
                << RESET << endl;
        return (WRONG_INDEX_SHAPE);
    }

    // Read meta information
    anenPar::Parameters parameters;
    anenSta::Stations stations;
    anenTime::Times times;
    anenTime::FLTs flts;

    handleError(readParameters(parameters, start[0], count[0], stride[0]));
    handleError(readStations(stations, start[1], count[1], stride[1]));
    handleError(readTimes(times, start[2], count[2], stride[2]));
    handleError(readFLTs(flts, start[3], count[3], stride[3]));

    // Read data
    if (verbose_ >= 3) cout << "Reading forecast values  from file ("
            << file_path_ << ") ..." << endl;
    vector<double> vals;
    handleError(read_vector_("Data", vals, start, count, stride));

    forecasts.setParameters(parameters);
    forecasts.setStations(stations);
    forecasts.setTimes(times);
    forecasts.setFlts(flts);
    forecasts.updateDataDims();
    forecasts.setValues(vals);

    return (SUCCESS);
}

errorType
AnEnIO::readFLTs(anenTime::FLTs& flts) {

    if (verbose_ >= 3) {
        cout << "Reading FLTs from file (" << file_path_ << ") ..." << endl;
    }

    if (mode_ != "Read") {
        if (verbose_ >= 1) cout << BOLDRED << "Error: Mode should be 'Read'."
                << RESET << endl;
        return (WRONG_MODE);
    }

    if (file_type_ != "Forecasts") {
        if (verbose_ >= 1) cout << BOLDRED
                << "Error: File type should be Forecasts!" << RESET << endl;
        return (WRONG_FILE_TYPE);
    }

    string var_name = "FLTs";
    handleError(checkVariable(var_name, false));

    vector<double> vec;
    handleError(read_vector_(var_name, vec));

    // copy the vector to the set
    flts.insert(flts.end(), vec.begin(), vec.end());

    if (flts.size() != vec.size()) {
        if (verbose_ >= 1) {
            cout << BOLDRED << "Error: The variable (FLTs)"
                    << " has duplicate elements!" << RESET << endl;
        }
        return (ELEMENT_NOT_UNIQUE);
    }

    return (SUCCESS);
}

errorType
AnEnIO::readFLTs(anenTime::FLTs& flts,
        size_t start, size_t count, ptrdiff_t stride) {

    if (verbose_ >= 3) {
        cout << "Reading FLTs  from file ("
                << file_path_ << ") ..." << endl;
    }

    if (mode_ != "Read") {
        if (verbose_ >= 1) cout << BOLDRED << "Error: Mode should be 'Read'."
                << RESET << endl;
        return (WRONG_MODE);
    }

    string var_name = "FLTs";
    handleError(checkVariable(var_name, false));

    vector<double> vec;
    handleError(read_vector_(var_name, vec, start, count, stride));

    // copy the vector to the set
    flts.insert(flts.end(), vec.begin(), vec.end());

    if (flts.size() != vec.size()) {
        if (verbose_ >= 1) {
            cout << BOLDRED << "Error: The variable (FLTs)"
                    << " has duplicate elements!" << RESET << endl;
        }
        return (ELEMENT_NOT_UNIQUE);
    }

    return (SUCCESS);

}

errorType
AnEnIO::readParameters(anenPar::Parameters & parameters) {

    if (verbose_ >= 3) {
        cout << "Reading Parameters from file ("
                << file_path_ << ") ..." << endl;
    }

    if (mode_ != "Read") {
        if (verbose_ >= 1) cout << BOLDRED << "Error: Mode should be 'Read'."
                << RESET << endl;
        return (WRONG_MODE);
    }

    size_t dim_len;
    handleError(readDimLength("num_parameters", dim_len));

    // Read variable ParameterNames
    vector<string> names;
    bool with_names = false;
    if (SUCCESS == checkVariable("ParameterNames", true)) {
        read_string_vector_("ParameterNames", names);
        with_names = true;

        if (names.size() != dim_len) {
            if (verbose_ >= 2) {
                cout << RED << "Warning: There should be " << dim_len
                        << " parameter names! Variable not used!"
                        << RESET << endl;
            }
            with_names = false;
        }
    }

    // Read variable ParameterCirculars
    vector<string> circulars;
    if (SUCCESS == checkVariable("ParameterCirculars", true)) {
        handleError(read_string_vector_("ParameterCirculars", circulars));
    }

    // Read variable ParameterWeights
    vector<double> weights;
    if (SUCCESS == checkVariable("ParameterWeights", true)) {
        handleError(read_vector_("ParameterWeights", weights));
    }

    // Construct anenPar::Parameter objects and
    // insert them into anenPar::Parameters
    //
    for (size_t i = 0; i < dim_len; i++) {

        anenPar::Parameter parameter;

        if (with_names) {

            string name = names.at(i);
            parameter.setName(name);

            // Set circular if name is present
            auto it_circular = find(circulars.begin(), circulars.end(), name);
            if (it_circular != circulars.end()) parameter.setCircular(true);
        }

        if (!(weights.empty()))
            parameter.setWeight(weights.at(i));

        if (!parameters.push_back(parameter).second) {

            if (verbose_ >= 1) {
                cout << BOLDRED << "Error: Parameter (ID: " << parameter.getID()
                        << ") is duplicate!" << RESET << endl;
            }
            return (ELEMENT_NOT_UNIQUE);
        }
    }

    return (SUCCESS);
}

errorType
AnEnIO::readParameters(anenPar::Parameters& parameters,
        size_t start, size_t count, ptrdiff_t stride) {

    if (verbose_ >= 3) {
        cout << "Reading Parameters  from file ("
                << file_path_ << ") ..." << endl;
    }

    if (mode_ != "Read") {
        if (verbose_ >= 1) cout << BOLDRED << "Error: Mode should be 'Read'."
                << RESET << endl;
        return (WRONG_MODE);
    }

    size_t dim_len;
    handleError(readDimLength("num_parameters", dim_len));

    // Check parameters
    if (start + (count - 1) * stride >= dim_len) {
        if (verbose_ >= 1) cout << BOLDRED
                << "Error: The parameter indices are not valid."
                << " The length of parameters is " << dim_len << RESET << endl;
        return (WRONG_INDEX_SHAPE);
    }

    // Read variable ParameterNames
    vector<string> names;
    bool with_names = false;
    if (SUCCESS == checkVariable("ParameterNames", true)) {
        read_string_vector_("ParameterNames",
                names, start, count, stride);
        with_names = true;
    }

    // Read variable ParameterCirculars
    vector<string> circulars;
    if (SUCCESS == checkVariable("ParameterCirculars", true)) {
        handleError(read_string_vector_("ParameterCirculars",
                circulars, start, count, stride));
    }

    // Read variable ParameterWeights
    vector<double> weights;
    if (SUCCESS == checkVariable("ParameterWeights", true)) {
        handleError(read_vector_("ParameterWeights",
                weights, start, count, stride));
    }

    // Construct anenPar::Parameter objects and
    // insert them into anenPar::Parameters
    //
    for (size_t i = 0; i < count; i++) {

        anenPar::Parameter parameter;

        if (with_names) {

            string name = names.at(i);
            parameter.setName(name);

            // Set circular if name is present
            auto it_circular = find(circulars.begin(), circulars.end(), name);
            if (it_circular != circulars.end()) parameter.setCircular(true);
        }

        if (!(weights.empty()))
            parameter.setWeight(weights.at(i));

        if (!parameters.push_back(parameter).second) {

            if (verbose_ >= 1) {
                cout << BOLDRED << "Error: Parameter (ID: " << parameter.getID()
                        << ") is duplicate!" << RESET << endl;
            }
            return (ELEMENT_NOT_UNIQUE);
        }
    }

    return (SUCCESS);
}

errorType
AnEnIO::readStations(anenSta::Stations& stations) {

    if (verbose_ >= 3) {
        cout << "Reading Stations from file (" << file_path_ << ") ..." << endl;
    }

    if (mode_ != "Read") {
        if (verbose_ >= 1) cout << BOLDRED << "Error: Mode should be 'Read'."
                << RESET << endl;
        return (WRONG_MODE);
    }

    size_t dim_len;
    handleError(readDimLength("num_stations", dim_len));

    // Read variable StationNames
    vector<string> names;
    bool with_names = false;
    if (SUCCESS == checkVariable("StationNames", false)) {
        read_string_vector_("StationNames", names);
        with_names = true;

        if (names.size() != dim_len) {
            if (verbose_ >= 2) {
                cout << RED << "Warning: There should be " << dim_len
                        << " station names! Variable not used!"
                        << RESET << endl;
            }
            with_names = false;
        }
    }

    // Read variables xs and ys (coordinates)
    vector<double> xs, ys;
    bool with_coordinates = false;
    if (SUCCESS == checkVariable("Xs", true) &&
            SUCCESS == checkVariable("Ys", true)) {
        handleError(read_vector_("Xs", xs));
        handleError(read_vector_("Ys", ys));
        with_coordinates = true;

        if (xs.size() != dim_len) {
            if (verbose_ >= 2) {
                cout << RED << "Warning: There should be " << dim_len
                        << " Xs! Coordinate variables not used!"
                        << RESET << endl;
            }
            with_coordinates = false;
        }
        if (ys.size() != dim_len) {
            if (verbose_ >= 2) {
                cout << RED << "Warning: There should be " << dim_len
                        << " Ys! Coordinate variables not used!"
                        << RESET << endl;
            }
            with_coordinates = false;
        }
    }

    // Construct Station object and insert them into anenSta::Stations
    for (size_t i = 0; i < dim_len; i++) {
        anenSta::Station station;

        if (with_names) {
            station.setName(names.at(i));
        }

        if (with_coordinates) {
            station.setX(xs.at(i));
            station.setY(ys.at(i));
        }

        if (!stations.push_back(station).second) {
            if (verbose_ >= 1) {
                cout << BOLDRED << "Error: Station (ID: " << station.getID()
                        << ") is duplicate!" << RESET << endl;
            }
            return (ELEMENT_NOT_UNIQUE);
        }
    }

    return (SUCCESS);
}

errorType
AnEnIO::readStations(anenSta::Stations& stations,
        size_t start, size_t count, ptrdiff_t stride) {

    if (verbose_ >= 3) {
        cout << "Reading Stations  from file ("
                << file_path_ << ") ..." << endl;
    }

    if (mode_ != "Read") {
        if (verbose_ >= 1) cout << BOLDRED << "Error: Mode should be 'Read'."
                << RESET << endl;
        return (WRONG_MODE);
    }

    size_t dim_len;
    handleError(readDimLength("num_stations", dim_len));

    // Check parameters
    if (start + (count - 1) * stride >= dim_len) {
        if (verbose_ >= 1) cout << BOLDRED
                << "Error: The station indices are not valid."
                << " The length of stations is " << dim_len << RESET << endl;
        return (WRONG_INDEX_SHAPE);
    }

    // Read variable StationNames
    vector<string> names;
    bool with_names = false;
    if (SUCCESS == checkVariable("StationNames", false)) {
        read_string_vector_("StationNames",
                names, start, count, stride);
        with_names = true;
    }

    // Read variables xs and ys (coordinates)
    vector<double> xs, ys;
    bool with_coordinates = false;
    if (SUCCESS == checkVariable("Xs", true) &&
            SUCCESS == checkVariable("Ys", true)) {
        handleError(read_vector_("Xs", xs, start, count, stride));
        handleError(read_vector_("Ys", ys, start, count, stride));
        with_coordinates = true;
    }

    // Construct Station object and insert them into anenSta::Stations
    for (size_t i = 0; i < count; i++) {
        anenSta::Station station;

        if (with_names) {
            station.setName(names.at(i));
        }

        if (with_coordinates) {
            station.setX(xs.at(i));
            station.setY(ys.at(i));
        }

        if (!stations.push_back(station).second) {
            if (verbose_ >= 1) {
                cout << BOLDRED << "Error: Station (ID: " << station.getID()
                        << ") is duplicate!" << RESET << endl;
            }
            return (ELEMENT_NOT_UNIQUE);
        }
    }

    return (SUCCESS);
}

errorType
AnEnIO::readTimes(anenTime::Times& times) {

    if (verbose_ >= 3) {
        cout << "Reading Times from file (" << file_path_
                << ")  ..." << endl;
    }

    if (mode_ != "Read") {
        if (verbose_ >= 1) cout << BOLDRED << "Error: Mode should be 'Read'."
                << RESET << endl;
        return (WRONG_MODE);
    }

    string var_name = "Times";
    handleError(checkVariable(var_name, false));

    vector<double> vec;
    handleError(read_vector_(var_name, vec));

    // copy the vector to the set
    times.insert(times.end(), vec.begin(), vec.end());

    if (times.size() != vec.size()) {
        if (verbose_ >= 1) {
            cout << BOLDRED << "Error: The variable (Times)"
                    << " has duplicate elements!" << RESET << endl;
        }
        return (ELEMENT_NOT_UNIQUE);
    }

    return (SUCCESS);
}

errorType
AnEnIO::readTimes(anenTime::Times& times,
        size_t start, size_t count, ptrdiff_t stride) {

    if (verbose_ >= 3) {
        cout << "Reading Times from file ("
                << file_path_ << ")   ..." << endl;
    }

    if (mode_ != "Read") {
        if (verbose_ >= 1) cout << BOLDRED << "Error: Mode should be 'Read'."
                << RESET << endl;
        return (WRONG_MODE);
    }

    string var_name = "Times";
    handleError(checkVariable(var_name, false));

    vector<double> vec;
    handleError(read_vector_(var_name, vec, start, count, stride));

    // copy the vector to the set
    times.insert(times.end(), vec.begin(), vec.end());

    if (times.size() != vec.size()) {
        if (verbose_ >= 1) {
            cout << BOLDRED << "Error: The variable (Times)"
                    << " has duplicate elements!" << RESET << endl;
        }
        return (ELEMENT_NOT_UNIQUE);
    }

    return (SUCCESS);
}

errorType
AnEnIO::readDimLength(string dim_name, size_t & len) {

    if (verbose_ >= 3) {
        cout << "Reading dimension (" << dim_name << ") length ..." << endl;
    }

    if (mode_ != "Read") {
        if (verbose_ >= 1) cout << BOLDRED << "Error: Mode should be 'Read'."
                << RESET << endl;
        return (WRONG_MODE);
    }

    handleError(checkDim(dim_name));

    NcFile nc(file_path_, NcFile::FileMode::read);
    NcDim dim = nc.getDim(dim_name);
    len = dim.getSize();
    nc.close();
    return (SUCCESS);
}

errorType
AnEnIO::writeForecasts(const Forecasts & forecasts) const {

    if (verbose_ >= 3) {
        cout << "Writing forecasts ..." << endl;
    }

    if (mode_ != "Write") {
        if (verbose_ >= 1) cout << BOLDRED << "Error: Mode should be 'Write'."
                << RESET << endl;
        return (WRONG_MODE);
    }

    // Create an empty file
    if (checkFilePath() != FILE_EXISTS) {
        cout << file_path_ << endl;
        NcFile nc_empty(file_path_, NcFile::FileMode::newFile,
                NcFile::FileFormat::nc4);
        nc_empty.close();
    } else if (!add_) {
        return (FILE_EXISTS);
    }

    // Write meta info
    handleError(writeParameters(forecasts.getParameters(), false));
    handleError(writeStations(forecasts.getStations(), false));
    handleError(writeTimes(forecasts.getTimes(), false));
    handleError(writeFLTs(forecasts.getFLTs(), false));

    // Write data
    if (verbose_ >= 3) cout << "Writing variable (Data) ..." << endl;

    NcFile nc(file_path_, NcFile::FileMode::write);

    NcVar var_data = nc.getVar("Data");
    if (!var_data.isNull()) {
        if (verbose_ >= 1) cout << BOLDRED << "Error: Variable (Data)"
                << " exists in file (" << file_path_ << ")." << RESET << endl;
        nc.close();
        return (VARIABLE_EXISTS);
    }

    // Create data variable
    vector<string> dim_data
            = {"num_flts", "num_times", "num_stations", "num_parameters"};
    var_data = nc.addVar("Data", "double", dim_data);
    var_data.putVar(forecasts.getValues());

    nc.close();
    return (SUCCESS);
}

errorType
AnEnIO::writeObservations(const Observations & observations) const {

    if (verbose_ >= 3) {
        cout << "Writing observations ..." << endl;
    }

    if (mode_ != "Write") {
        if (verbose_ >= 1) cout << BOLDRED << "Error: Mode should be 'Write'."
                << RESET << endl;
        return (WRONG_MODE);
    }

    handleError(checkFilePath());

    // Create an empty NetCDF file
    if (checkFilePath() != FILE_EXISTS) {
        NcFile nc_empty(file_path_, NcFile::FileMode::newFile,
                NcFile::FileFormat::nc4);
        nc_empty.close();
    } else if (!add_) {
        return (FILE_EXISTS);
    }

    // Write meta info
    handleError(writeParameters(observations.getParameters(), false));
    handleError(writeStations(observations.getStations(), false));
    handleError(writeTimes(observations.getTimes(), false));

    // Write data
    if (verbose_ >= 3) cout << "Writing variable (Data) ..." << endl;

    NcFile nc(file_path_, NcFile::FileMode::write);

    NcVar var_data = nc.getVar("Data");
    if (!var_data.isNull()) {
        if (verbose_ >= 1) cout << BOLDRED << "Error: Variable (Data)"
                << " exists in file (" << file_path_ << ")." << RESET << endl;
        nc.close();
        return (VARIABLE_EXISTS);
    }

    // Create data variable
    vector<string> dim_data = {"num_times", "num_stations", "num_parameters"};
    var_data = nc.addVar("Data", "double", dim_data);
    var_data.putVar(observations.getValues());

    nc.close();
    return (SUCCESS);
}

errorType
AnEnIO::writeFLTs(const anenTime::FLTs& flts, bool unlimited) const {

    if (verbose_ >= 3) cout << "Writing variable (FLTs) ..." << endl;

    if (mode_ != "Write") {
        if (verbose_ >= 1) cout << BOLDRED << "Error: Mode should be 'Write'."
                << RESET << endl;
        return (WRONG_MODE);
    }

    NcFile nc(file_path_, NcFile::FileMode::write);

    // Check if file already has dimension
    NcDim dim_flts = nc.getDim("num_flts");
    if (!dim_flts.isNull()) {
        if (dim_flts.getSize() != flts.size() ||
                dim_flts.isUnlimited() != unlimited) {
            if (verbose_ >= 1) cout << BOLDRED
                    << "Error: Dimension (num_flts) with different length"
                    << " exists in file (" << file_path_ << ")."
                    << RESET << endl;
            nc.close();
            return (DIMENSION_EXISTS);
        }
    } else {
        // Create dimension
        if (unlimited) {
            dim_flts = nc.addDim("num_flts");
        } else {
            dim_flts = nc.addDim("num_flts", flts.size());
        }
    }

    // Check if file already has variable
    NcVar var = nc.getVar("FLTs");
    if (!var.isNull()) {
        if (verbose_ >= 1) cout << BOLDRED << "Error: Variable (FLTs)"
                << " exists in file (" << file_path_ << ")." << RESET << endl;
        nc.close();
        return (VARIABLE_EXISTS);
    }

    var = nc.addVar("FLTs", NcType::nc_DOUBLE, dim_flts);

    // Convert from multi-index container type to a double pointer
    double* p = nullptr;
    try {
        p = new double [flts.size()]();
    } catch (bad_alloc & e) {
        cout << BOLDRED << "Error: Insufficient memory to write FLTs ("
                << flts.size() << ")." << RESET << endl;
        nc.close();
        return (INSUFFICIENT_MEMORY);
    }

    const anenTime::multiIndexTimes::index<anenTime::by_insert>::type&
            flts_by_insert = flts.get<anenTime::by_insert>();

#if defined(_OPENMP)
#pragma omp parallel for default(none) schedule(static) \
shared(flts_by_insert, p)
#endif
    for (size_t i = 0; i < flts_by_insert.size(); i++) {
        p[i] = flts_by_insert[i];
    }

    var.putVar(p);
    delete [] p;

    nc.close();
    return (SUCCESS);
}

errorType
AnEnIO::writeParameters(const anenPar::Parameters& parameters,
        bool unlimited) const {

    if (verbose_ >= 3) cout << "Writing variable (Parameters) ..." << endl;

    if (mode_ != "Write") {
        if (verbose_ >= 1) cout << BOLDRED << "Error: Mode should be 'Write'."
                << RESET << endl;
        return (WRONG_MODE);
    }

    // Check if parameters have weights
    size_t num_nan = 0;
    double sum = 0.0;
    const anenPar::multiIndexParameters::index<anenPar::by_insert>::type &
            parameters_by_insert = parameters.get<anenPar::by_insert>();
    for (const auto & parameter : parameters_by_insert) {
        double tmp = parameter.getWeight();
        if (std::isnan(tmp)) {
            num_nan++;
        } else {
            sum += tmp;
        }
    }

    if (num_nan == 0) {
        if (sum != 1) {
            if (verbose_ >= 1) cout << BOLDRED
                    << "Error: Parameter weights do not"
                    << " add up to 1." << RESET << endl;
            return (ERROR_SETTING_VALUES);
        }
    } else if (num_nan != parameters_by_insert.size()) {

        if (verbose_ >= 1) cout << BOLDRED
                << "Error: Weights can either be all NAN or all numbers."
                << RESET << endl;
        return (NAN_VALUES);
    }

    NcFile nc(file_path_, NcFile::FileMode::write);

    // Check if file already has dimensions
    NcDim dim_parameters = nc.getDim("num_parameters");
    if (!dim_parameters.isNull()) {
        if (dim_parameters.getSize() != parameters.size() ||
                dim_parameters.isUnlimited() != unlimited) {
            if (verbose_ >= 1) cout << BOLDRED
                    << "Error: Dimension (num_parameters) with a different"
                    << " length exists in file (" << file_path_ << ")."
                    << RESET << endl;
            nc.close();
            return (DIMENSION_EXISTS);
        }
    } else {
        if (unlimited) {
            dim_parameters = nc.addDim("num_parameters");
        } else {
            dim_parameters = nc.addDim("num_parameters", parameters.size());
        }
    }

    NcDim dim_chars = nc.getDim("num_chars");
    if (dim_chars.isNull()) {
        dim_chars = nc.addDim("num_chars", _max_chars);
    }

    // Check if file already has variable
    NcVar var;
    vector<string> var_names_to_check
            = {"ParameterNames", "ParameterWeights", "ParameterCirculars"};
    for (auto name : var_names_to_check) {
        var = nc.getVar(name);
        if (!var.isNull()) {
            if (verbose_ >= 1) cout << BOLDRED
                    << "Error: Variable (" << name << ") exists in file ("
                    << file_path_ << ")." << RESET << endl;
            nc.close();
            return (VARIABLE_EXISTS);
        }
    }

    vector<NcDim> dim_names = {dim_parameters, dim_chars};
    vector<NcDim> dim_circulars = {dim_parameters, dim_chars};
    NcVar var_names = nc.addVar("ParameterNames", NcType::nc_CHAR, dim_names);
    NcVar var_weights = nc.addVar("ParameterWeights",
            NcType::nc_DOUBLE, dim_parameters);
    NcVar var_circulars = nc.addVar("ParameterCirculars",
            NcType::nc_CHAR, dim_circulars);

    vector<string> circular_names;
    for (const auto & parameter : parameters_by_insert) {
        if (parameter.getCircular()) {
            circular_names.push_back(parameter.getName());
        }
    }

    // Convert types for writing
    char* p_names = nullptr;
    double* p_weights = nullptr;
    char* p_circulars = nullptr;
    try {
        p_names = new char [_max_chars * parameters.size()]();
        p_weights = new double [parameters.size()]();
        p_circulars = new char [_max_chars * parameters.size()]();
    } catch (bad_alloc & e) {
        cout << BOLDRED << "Error: Insufficient memory to write Parameters ("
                << parameters.size() << ")." << RESET << endl;
        nc.close();
        return (INSUFFICIENT_MEMORY);
    }

#if defined(_OPENMP)
#pragma omp parallel for default(none) schedule(static) \
shared(parameters_by_insert, p_circulars, p_names, p_weights, _max_chars)
#endif
    for (size_t i = 0; i < parameters_by_insert.size(); i++) {
        parameters_by_insert[i].getName().copy(
                p_names + (i * _max_chars), _max_chars, 0);
        p_weights[i] = parameters_by_insert[i].getWeight();
    }

    for (size_t i = 0; i < circular_names.size(); i++) {
        circular_names[i].copy(
                p_circulars + (i * _max_chars), _max_chars, 0);
    }

    var_names.putVar(p_names);
    var_weights.putVar(p_weights);
    var_circulars.putVar(p_circulars);
    delete [] p_names;
    delete [] p_weights;
    delete [] p_circulars;

    nc.close();
    return (SUCCESS);
}

errorType
AnEnIO::writeStations(
        const anenSta::Stations& stations,
        bool unlimited) const {

    if (verbose_ >= 3) cout << "Writing variable (Stations) ..." << endl;

    if (mode_ != "Write") {
        if (verbose_ >= 1) cout << BOLDRED << "Error: Mode should be 'Write'."
                << RESET << endl;
        return (WRONG_MODE);
    }

    NcFile nc(file_path_, NcFile::FileMode::write);

    // Check if file already has dimensions
    NcDim dim_stations = nc.getDim("num_stations");
    if (!dim_stations.isNull()) {
        if (dim_stations.getSize() != stations.size() ||
                dim_stations.isUnlimited() != unlimited) {
            if (verbose_ >= 1) cout << BOLDRED
                    << "Error: Dimension (num_stations) exists with a "
                    << "different length in file (" << file_path_ << ")."
                    << RESET << endl;
            nc.close();
            return (DIMENSION_EXISTS);
        }
    } else {
        if (unlimited) {
            dim_stations = nc.addDim("num_stations");
        } else {
            dim_stations = nc.addDim("num_stations", stations.size());
        }
    }

    NcDim dim_chars = nc.getDim("num_chars");
    if (dim_chars.isNull()) {
        dim_chars = nc.addDim("num_chars", _max_chars);
    }

    // Check if file already has variable
    NcVar var;
    vector<string> var_names_to_check = {"StationNames", "Xs", "Ys"};
    for (auto name : var_names_to_check) {
        var = nc.getVar(name);
        if (!var.isNull()) {
            if (verbose_ >= 1) cout << BOLDRED
                    << "Error: Variable (" << name << ") exists in file ("
                    << file_path_ << ")." << RESET << endl;
            nc.close();
            return (VARIABLE_EXISTS);
        }
    }

    vector<NcDim> dim_names = {dim_stations, dim_chars};
    NcVar var_names = nc.addVar("StationNames", NcType::nc_CHAR, dim_names);
    NcVar var_xs = nc.addVar("Xs", NcType::nc_DOUBLE, dim_stations);
    NcVar var_ys = nc.addVar("Ys", NcType::nc_DOUBLE, dim_stations);

    // Convert types for writing
    char* p_names = nullptr;
    double* p_xs = nullptr;
    double* p_ys = nullptr;
    try {
        p_names = new char [_max_chars * stations.size()]();
        p_xs = new double [stations.size()]();
        p_ys = new double [stations.size()]();
    } catch (bad_alloc e) {
        cout << BOLDRED << "Error: Insufficient memory to write Parameters ("
                << stations.size() << ")." << RESET << endl;
        nc.close();
        return (INSUFFICIENT_MEMORY);
    }

    const anenSta::multiIndexStations::index<anenSta::by_insert>::type&
            stations_by_insert = stations.get<anenSta::by_insert>();

#if defined(_OPENMP)
#pragma omp parallel for default(none) schedule(static) \
shared(stations_by_insert, p_names, p_xs, p_ys, _max_chars)
#endif
    for (size_t i = 0; i < stations_by_insert.size(); i++) {
        stations_by_insert[i].getName().copy(
                p_names + (i * _max_chars), _max_chars, 0);
        p_xs[i] = stations_by_insert[i].getX();
        p_ys[i] = stations_by_insert[i].getY();
    }

    var_names.putVar(p_names);
    var_xs.putVar(p_xs);
    var_ys.putVar(p_ys);
    delete [] p_names;
    delete [] p_xs;
    delete [] p_ys;

    nc.close();
    return (SUCCESS);
}

errorType
AnEnIO::writeTimes(const anenTime::Times& times, bool unlimited) const {

    if (verbose_ >= 3) cout << "Writing variable (Times) ..." << endl;

    if (mode_ != "Write") {
        if (verbose_ >= 1) cout << BOLDRED << "Error: Mode should be 'Write'."
                << RESET << endl;
        return (WRONG_MODE);
    }

    NcFile nc(file_path_, NcFile::FileMode::write);

    // Check if file already has dimension
    NcDim dim_times = nc.getDim("num_times");
    if (!dim_times.isNull()) {
        if (dim_times.getSize() != times.size() ||
                dim_times.isUnlimited() != unlimited) {
            if (verbose_ >= 1) cout << BOLDRED << "Error: Dimension (num_times)"
                    << " with a different length exists in file ("
                    << file_path_ << ")." << RESET << endl;
            nc.close();
            return (DIMENSION_EXISTS);
        }
    } else {
        // Create dimension
        if (unlimited) {
            dim_times = nc.addDim("num_times");
        } else {
            dim_times = nc.addDim("num_times", times.size());
        }
    }

    // Check if file already has variable
    NcVar var = nc.getVar("Times");
    if (!var.isNull()) {
        if (verbose_ >= 1) cout << BOLDRED << "Error: Variable (Times)"
                << " exists in file (" << file_path_ << ")." << RESET << endl;
        nc.close();
        return (VARIABLE_EXISTS);
    }

    var = nc.addVar("Times", NcType::nc_DOUBLE, dim_times);

    // Convert from multi-index container type to a double pointer
    double* p = nullptr;
    try {
        p = new double [times.size()]();
    } catch (bad_alloc e) {
        cout << BOLDRED << "Error: Insufficient memory to write Times ("
                << times.size() << ")." << RESET << endl;
        nc.close();
        return (INSUFFICIENT_MEMORY);
    }

    const anenTime::multiIndexTimes::index<anenTime::by_insert>::type&
            times_by_insert = times.get<anenTime::by_insert>();

#if defined(_OPENMP)
#pragma omp parallel for default(none) schedule(static) \
shared(times_by_insert, p)
#endif
    for (size_t i = 0; i < times_by_insert.size(); i++) {
        p[i] = times_by_insert[i];
    }

    var.putVar(p);
    delete [] p;

    nc.close();
    return (SUCCESS);
}

errorType
AnEnIO::readSimilarityMatrices(SimilarityMatrices & sims) {

    if (verbose_ >= 3) cout << "Reading similarity matrices ..." << endl;

    if (mode_ != "Read") {
        if (verbose_ >= 1) cout << BOLDRED << "Error: Mode should be 'Read'."
                << RESET << endl;
        return (WRONG_MODE);
    }

    if (file_type_ != "Similarity") {
        if (verbose_ >= 1)
            cout << BOLDRED << "Error: file type should be Similarity."
                << RESET << endl;
        return (WRONG_FILE_TYPE);
    }

    handleError(checkFilePath());
    handleError(checkFileType());

    // Resize similarity matrices to clear any leftover values
    SimilarityMatrices::extent_gen extens;
    sims.resize(0, 0, 0);

    // Read forecasts
    if (checkVariable("Data", true) == SUCCESS) {
        setFileType("Forecasts");
        Forecasts_array forecasts;
        handleError(readForecasts(forecasts));
        setFileType("Similarity");

        // Set forecast target for similarity matrices
        NcFile nc(file_path_, NcFile::FileMode::read);
        sims.setMaxEntries(nc.getDim("num_entries").getSize());
        sims.setTargets(forecasts);
        nc.close();
    } else {
        NcFile nc(file_path_, NcFile::FileMode::read);
        sims.setMaxEntries(nc.getDim("num_entries").getSize());
        sims.resize(nc.getDim("num_test_stations").getSize(),
                nc.getDim("num_test_times").getSize(),
                nc.getDim("num_flts").getSize());
        nc.close();
    }

    vector<double> values;
    handleError(read_vector_("SimilarityMatrices", values));
    sims.assign(values.begin(), values.end());

    return (SUCCESS);
}

errorType
AnEnIO::writeSimilarityMatrices(const SimilarityMatrices & sims) {

    if (verbose_ >= 3) cout << "Writing similarity matrices ..." << endl;

    if (mode_ != "Write") {
        if (verbose_ >= 1) cout << BOLDRED << "Error: Mode should be 'Write'."
                << RESET << endl;
        return (WRONG_MODE);
    }

    if (checkFilePath() != FILE_EXISTS) {
        NcFile nc_empty(file_path_, NcFile::FileMode::newFile,
                NcFile::FileFormat::nc4);
        nc_empty.close();
    } else if (!add_) {
        return (FILE_EXISTS);
    }

    size_t num_test_stations = sims.shape()[0],
            num_test_times = sims.shape()[1],
            num_flts = sims.shape()[2],
            num_entries = sims.shape()[3],
            num_cols = sims.shape()[4];

    NcFile nc(file_path_, NcFile::FileMode::write);

    // Create dimensions
    NcDim dim_stations = nc.addDim("num_test_stations", num_test_stations);
    NcDim dim_times = nc.addDim("num_test_times", num_test_times);
    NcDim dim_flts = nc.addDim("num_flts", num_flts);
    NcDim dim_entries = nc.addDim("num_entries", num_entries);
    NcDim dim_cols = nc.addDim("num_cols", num_cols);

    // Create similarity matrices variable
    NcVar var_sims = nc.addVar("SimilarityMatrices", NC_DOUBLE,{
        dim_stations, dim_times, dim_flts, dim_entries, dim_cols
    });

    var_sims.putVar(sims.data());
    nc.close();

    // Add Forecasts
    if (sims.getTargets().getDataLength() != 0) {
        bool ori = isAdd();
        setAdd(true);
        handleError(writeForecasts(sims.getTargets()));
        setAdd(ori);
    }

    return (SUCCESS);
}

errorType
AnEnIO::readAnalogs(Analogs & analogs) {
    if (verbose_ >= 3) cout << "Reading analogs ..." << endl;

    if (mode_ != "Read") {
        if (verbose_ >= 1) cout << BOLDRED << "Error: Mode should be 'Read'."
                << RESET << endl;
        return (WRONG_MODE);
    }

    if (file_type_ != "Analogs") {
        if (verbose_ >= 1)
            cout << BOLDRED << "Error: file type should be Analogs."
                << RESET << endl;
        return (WRONG_FILE_TYPE);
    }

    handleError(checkFilePath());
    handleError(checkFileType());

    if (checkVariable("StationNames", true) == SUCCESS &&
            checkVariable("FLTs", true) == SUCCESS &&
            checkVariable("Times", true) == SUCCESS) {
        anenSta::Stations stations;
        anenTime::Times times;
        anenTime::FLTs flts;

        readTimes(times);
        readFLTs(flts);
        readStations(stations);

        analogs.setTimes(times);
        analogs.setFLTs(flts);
        analogs.setStations(stations);
    } else {
        NcFile nc(file_path_, NcFile::FileMode::read);
        analogs.resize(boost::extents
                [nc.getDim("num_test_stations").getSize()]
                [nc.getDim("num_test_times").getSize()]
                [nc.getDim("num_flts").getSize()]
                [nc.getDim("num_members").getSize()]
                [nc.getDim("num_cols").getSize()]);
        nc.close();
    }

    vector<double> values;
    handleError(read_vector_("Analogs", values));
    analogs.assign(values.begin(), values.end());

    return (SUCCESS);
}

errorType
AnEnIO::writeAnalogs(const Analogs & analogs) const {
    if (verbose_ >= 3) cout << "Writing analogs ..." << endl;

    if (mode_ != "Write") {
        if (verbose_ >= 1) cout << BOLDRED << "Error: Mode should be 'Write'."
                << RESET << endl;
        return (WRONG_MODE);
    }

    if (checkFilePath() != FILE_EXISTS) {
        NcFile nc_empty(file_path_, NcFile::FileMode::newFile,
                NcFile::FileFormat::nc4);
        nc_empty.close();
    } else if (!add_) {
        return (FILE_EXISTS);
    }

    size_t num_test_stations = analogs.shape()[0],
            num_test_times = analogs.shape()[1],
            num_flts = analogs.shape()[2],
            num_members = analogs.shape()[3],
            num_cols = analogs.shape()[4];

    NcFile nc(file_path_, NcFile::FileMode::write);

    NcDim dim_stations = nc.addDim("num_test_stations", num_test_stations);
    NcDim dim_times = nc.addDim("num_test_times", num_test_times);
    NcDim dim_flts = nc.addDim("num_flts", num_flts);
    NcDim dim_members = nc.addDim("num_members", num_members);
    NcDim dim_cols = nc.addDim("num_cols", num_cols);

    // Create similarity matrices variable
    NcVar var_sims = nc.addVar("Analogs", NC_DOUBLE,{
        dim_cols, dim_members, dim_flts, dim_times, dim_stations
    });

    var_sims.putVar(analogs.data());
    nc.close();

    if (analogs.getFLTs().size() != 0)
        handleError(writeFLTs(analogs.getFLTs(), false));
    if (analogs.getStations().size() != 0)
        handleError(writeStations(analogs.getStations(), false));
    if (analogs.getTimes().size() != 0)
        handleError(writeTimes(analogs.getTimes(), false));

    return (SUCCESS);
}

void
AnEnIO::handleError(const errorType & indicator) const {
    if (indicator == SUCCESS ||
            indicator == OPTIONAL_VARIABLE_MISSING) {
        return;
    } else {
        throw runtime_error("Error code " + to_string((long long) indicator));
    }
}

bool
AnEnIO::isAdd() const {
    return add_;
}

int
AnEnIO::getVerbose() const {
    return verbose_;
}

string
AnEnIO::getMode() const {
    return mode_;
}

string
AnEnIO::getFilePath() const {
    return file_path_;
}

string
AnEnIO::getFileType() const {
    return file_type_;
}

vector<string>
AnEnIO::getOptionalVariables() const {
    return optional_variables_;
}

vector<string>
AnEnIO::getRequiredVariables() const {
    return required_variables_;
}

vector<string>
AnEnIO::getRequiredDimensions() const {
    return required_dimensions_;
}

void
AnEnIO::setAdd(bool add) {
    this->add_ = add;
}

void
AnEnIO::setVerbose(int verbose) {
    this->verbose_ = verbose;
}

void
AnEnIO::setMode(string mode) {
    this->mode_ = mode;
    handleError(checkMode());
    handleError(checkFilePath());
}

void
AnEnIO::setMode(string mode, string file_path) {
    this->mode_ = mode;
    this->file_path_ = file_path;
    handleError(checkMode());
    handleError(checkFilePath());
}

void
AnEnIO::setFilePath(string file_path) {
    this->file_path_ = file_path;
    handleError(checkFilePath());
}

void
AnEnIO::setFileType(string file_type) {
    this->file_type_ = file_type;
    if (mode_ != "Write") handleError(checkFileType());
}

void
AnEnIO::setOptionalVariables(vector<string> optional_variables) {
    this->optional_variables_ = optional_variables;
}

void
AnEnIO::setRequiredVariables(vector<string> required_variables) {
    this->required_variables_ = required_variables;
}

void
AnEnIO::setRequiredDimensions(vector<string> required_dimensions) {
    this->required_dimensions_ = required_dimensions;
}

void
AnEnIO::dumpVariable(string var_name, size_t start, size_t count) const {

    handleError(checkVariable(var_name, false));

    if (verbose_ >= 3)
        cout << "Dumping variable ("
            << var_name << ") values ..." << endl
            << "---------------------------------------------" << endl;

    if (mode_ != "Read") {
        if (verbose_ >= 1) cout << BOLDRED << "Error: Mode should be 'Read'."
                << RESET << endl;
        return;
    }

    NcFile nc(file_path_, NcFile::FileMode::read);
    NcVar var = nc.getVar(var_name);
    NcType var_type = var.getType();
    auto var_dims = var.getDims();

    size_t len = 1;
    for (const auto & dim : var_dims) {
        len *= dim.getSize();
    }

    if (count == 0) {
        count = len;
    }
    if (start >= len) {
        if (verbose_ >= 2)
            cout << RED << "Warning: change start (" << start << ") to "
                << (len - 1) << "." << RESET << endl;
        start = len - 1;
    }
    if (start + count > len) {
        if (verbose_ >= 2)
            cout << RED << "Warning: change count (" << count << ") to "
                << (len - start) << "." << RESET << endl;
        count = len - start;
    }

    void *p_vals;
    try {
        switch (var_type.getTypeClass()) {
            case NcType::nc_CHAR:
                p_vals = new char[len];
                break;
            case NcType::nc_BYTE:
            case NcType::nc_SHORT:
            case NcType::nc_INT:
                p_vals = new int[len];
                break;
            case NcType::nc_FLOAT:
                p_vals = new float[len];
                break;
            case NcType::nc_DOUBLE:
                p_vals = new double[len];
                break;
            case NcType::nc_UBYTE:
            case NcType::nc_USHORT:
            case NcType::nc_UINT:
            case NcType::nc_INT64:
            case NcType::nc_UINT64:
                p_vals = new unsigned int[len];
                break;
            default:
                if (verbose_ >= 1) cout << BOLDRED << "Error: Variable ("
                        << var_name << ") type not supported!"
                        << RESET << endl;
                return;
        }
    } catch (bad_alloc & e) {
        nc.close();
        if (verbose_ >= 1) cout << BOLDRED <<
                "Error: Insufficient memory when reading variable ("
                << var_name << ")!" << RESET << endl;
        return;
    }

    var.getVar(p_vals);

    switch (var_type.getTypeClass()) {
        case NcType::nc_CHAR:
            copy((char*) p_vals + start, (char*) p_vals + start + count,
                    ostream_iterator<char>(cout, ""));
            delete [] (char*) p_vals;
            break;
        case NcType::nc_BYTE:
        case NcType::nc_SHORT:
        case NcType::nc_INT:
            copy((int*) p_vals + start, (int*) p_vals + start + count,
                    ostream_iterator<int>(cout, " "));
            delete [] (int*) p_vals;
            break;
        case NcType::nc_FLOAT:
            copy((float*) p_vals + start, (float*) p_vals + start + count,
                    ostream_iterator<float>(cout, " "));
            delete [] (float*) p_vals;
            break;
        case NcType::nc_DOUBLE:
            copy((double*) p_vals + start, (double*) p_vals + start + count,
                    ostream_iterator<double>(cout, " "));
            delete [] (double*) p_vals;
            break;
        case NcType::nc_UBYTE:
        case NcType::nc_USHORT:
        case NcType::nc_UINT:
        case NcType::nc_INT64:
        case NcType::nc_UINT64:
            copy((unsigned int*) p_vals + start,
                    (unsigned int*) p_vals + start + count,
                    ostream_iterator<unsigned int>(cout, " "));
            delete [] (unsigned int*) p_vals;
            break;
        default:
            if (verbose_ >= 1) cout << BOLDRED << "Error: Variable ("
                    << var_name << ") type not supported!" << RESET << endl;
            return;
    }
    cout << endl;

    if (verbose_ >= 3)
        cout << "---------------------------------------------" << endl;

    nc.close();
    return;
}

errorType
AnEnIO::read_string_vector_(string var_name, vector<string> & results) const {

    if (mode_ != "Read") {
        if (verbose_ >= 1) cout << BOLDRED << "Error: Mode should be 'Read'."
                << RESET << endl;
        return (WRONG_MODE);
    }

    NcFile nc(file_path_, NcFile::FileMode::read);
    NcVar var = nc.getVar(var_name);
    auto var_dims = var.getDims();
    char *p_vals = nullptr;

    if (var_dims.size() == 2) {
        if (var.getType() == NcType::nc_CHAR) {

            size_t len = 1;
            for (const auto & dim : var_dims) {
                len *= dim.getSize();
            }

            try {
                p_vals = new char[len];
            } catch (bad_alloc & e) {
                nc.close();
                if (verbose_ >= 1) cout << BOLDRED <<
                        "Error: Insufficient memory reading variable ("
                        << var_name << ")!" << RESET << endl;
                return (INSUFFICIENT_MEMORY);
            }

            var.getVar(p_vals);

            size_t num_strs, num_chars;

            // Figure out which dimension is the number of chars and 
            // which dimension is the number of strings.
            //
            if (var_dims.at(0).getName() == "num_chars") {
                num_strs = var_dims.at(1).getSize();
                num_chars = var_dims.at(0).getSize();
            } else if (var_dims.at(1).getName() == "num_chars") {
                num_strs = var_dims.at(0).getSize();
                num_chars = var_dims.at(1).getSize();
            } else {
                if (verbose_ >= 1) {
                    cout << BOLDRED << "Error: The dimension (num_chars)"
                            << " is expected in variable (" << var_name
                            << ")!" << RESET << endl;
                }
                return (WRONG_VARIABLE_SHAPE);
            }

            results.resize(num_strs);
            for (size_t i = 0; i < num_strs; i++) {
                string str(p_vals + i*num_chars, p_vals + (i + 1) * num_chars);
                purge_(str);
                results.at(i) = str;
            }

            delete [] p_vals;
            nc.close();
            return (SUCCESS);

        } else {

            if (verbose_ >= 1) {
                cout << BOLDRED << "Error: Variable (" << var_name <<
                        ") is not nc_CHAR type!" << RESET << endl;
            }
            return (WRONG_VARIABLE_SHAPE);
        }
    } else {

        if (verbose_ >= 1) {
            cout << BOLDRED << "Error: Variable (" << var_name <<
                    ") should only have 2 dimensions!" << RESET << endl;
        }
        return (WRONG_VARIABLE_SHAPE);
    }
}

errorType
AnEnIO::read_string_vector_(string var_name, vector<string>& results,
        size_t start, size_t count, ptrdiff_t stride) const {

    if (mode_ != "Read") {
        if (verbose_ >= 1) cout << BOLDRED << "Error: Mode should be 'Read'."
                << RESET << endl;
        return (WRONG_MODE);
    }

    NcFile nc(file_path_, NcFile::FileMode::read);
    NcVar var = nc.getVar(var_name);
    auto var_dims = var.getDims();
    char *p_vals = nullptr;

    if (var_dims.size() == 2) {
        if (var.getType() == NcType::nc_CHAR) {

            size_t num_chars = nc.getDim("num_chars").getSize();
            size_t total = count * num_chars;

            try {
                p_vals = new char[total];
            } catch (bad_alloc & e) {
                nc.close();
                if (verbose_ >= 1) cout << BOLDRED <<
                        "Error: Insufficient memory reading variable ("
                        << var_name << ")!" << RESET << endl;
                return (INSUFFICIENT_MEMORY);
            }

            vector<size_t> vec_start{start, 0}, vec_count{count, num_chars};
            vector<ptrdiff_t> vec_stride{stride, 1};

            var.getVar(vec_start, vec_count, vec_stride, p_vals);

            results.resize(count);
            for (size_t i = 0; i < count; i++) {
                string str(p_vals + i*num_chars, p_vals + (i + 1) * num_chars);
                purge_(str);
                results.at(i) = str;
            }

            delete [] p_vals;
            nc.close();
            return (SUCCESS);

        } else {

            if (verbose_ >= 1) {
                cout << BOLDRED << "Error: Variable (" << var_name <<
                        ") is not nc_CHAR type!" << RESET << endl;
            }
            return (WRONG_VARIABLE_SHAPE);
        }
    } else {

        if (verbose_ >= 1) {
            cout << BOLDRED << "Error: Variable (" << var_name <<
                    ") should only have 2 dimensions!" << RESET << endl;
        }
        return (WRONG_VARIABLE_SHAPE);
    }
}

void AnEnIO::purge_(string & str) const {

    str.erase(remove_if(str.begin(), str.end(), [](const unsigned char & c) {
        bool remove = !(isalpha(c) || isdigit(c) || ispunct(c) || isspace(c));
        return (remove);
    }), str.end());

}

void AnEnIO::purge_(vector<string> & strs) const {

    for (auto & str : strs) purge_(str);
}
