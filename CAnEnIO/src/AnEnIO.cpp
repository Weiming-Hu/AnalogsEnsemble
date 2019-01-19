/* 
 * File:   AnEnIO.cpp
 * Author: Weiming Hu (weiming@psu.edu)
 * 
 * Created on May 4, 2018, 11:16 AM
 */

#include "AnEnIO.h"
#include "boost/filesystem.hpp"
#include "AnEn.h"

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
    if (verbose_ >= 4) {
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

    if (verbose_ >= 4) {
        cout << "Checking file (" << file_path_ << ") ..." << endl;
    }

    bool file_exists = filesys::exists(file_path_);

    if (file_exists && mode_ == "Write") {
        if (add_) {

            if (verbose_ >= 4) cout << "Writing to existing file "
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

            if (mode_ == "Matrix" || ext == ".txt") return (SUCCESS);
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

    if (verbose_ >= 4) {
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
            var_names = {"Data", "Times", "StationNames", "ParameterNames",
            "ParameterCirculars", "Xs", "Ys"};

        } else if (file_type_ == "Forecasts") {

            dim_names = {"num_parameters", "num_stations",
                "num_times", "num_flts", "num_chars"};
            var_names = {"Data", "FLTs", "Times", "StationNames", "ParameterWeights",
                "ParameterNames", "ParameterCirculars", "Xs", "Ys"};

        } else if (file_type_ == "Similarity") {

            dim_names = {"num_stations", "num_times", "num_flts", "num_entries",
                "num_cols", "num_parameters", "num_chars", "search_num_statios",
                "search_num_times"};
            var_names = {"SimilarityMatrices", "ParameterNames", "ParameterWeights",
                "ParameterCirculars", "StationNames", "Xs", "Ys", "Times", "FLTs",
                "SearchTimes", "SearchStationNames", "SearchXs", "SearchYs"};

        } else if (file_type_ == "Analogs") {

            dim_names = {"num_stations", "num_times", "num_flts", "num_members",
                "num_cols", "num_chars", "member_num_stations", "member_num_times"};
            var_names = {"Analogs", "StationNames", "Xs", "Ys", "Times", "FLTs",
                "MemberTimes", "MemberStationNames", "MemberXs", "MemberYs"};

        } else if (file_type_ == "StandardDeviation") {

            dim_names = {"num_parameters", "num_stations", "num_flts"};
            var_names = {"StandardDeviation", "FLTs", "StationNames", "ParameterWeights",
                "ParameterNames", "ParameterCirculars", "Xs", "Ys"};

        } else if (file_type_ == "Matrix") {

            if (verbose_ >= 3) {
                cout << RED << "File check for Matrix type is not defined." << RESET << endl;
            }
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

    if (verbose_ >= 4) {
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

    if (verbose_ >= 4) {
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

    if (verbose_ >= 4) {
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

    if (verbose_ >= 4) {
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
AnEnIO::readObservations(Observations_array & observations) const {

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

    // Read meta information
    anenPar::Parameters parameters;
    anenSta::Stations stations;
    anenTime::Times times;

    handleError(readParameters(parameters));
    handleError(readStations(stations));
    handleError(readTimes(times));

    // Prepare the meta information in Observations first
    observations.setParameters(parameters);
    observations.setStations(stations);
    observations.setTimes(times);
    observations.updateDataDims();

    // Read data
    if (verbose_ >= 3) cout << "Reading observation values from file ("
            << file_path_ << ") ..." << endl;
    handleError(readObservationsArrayData_(observations));

    return (SUCCESS);
}

errorType
AnEnIO::readObservations(Observations_array & observations, vector<size_t> start,
        vector<size_t> count, vector<ptrdiff_t> stride) const {

    if (verbose_ >= 3) {
        cout << "Reading observation file according to start and count ..." << endl;
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

    observations.setParameters(parameters);
    observations.setStations(stations);
    observations.setTimes(times);
    observations.updateDataDims();

    // Read data
    if (verbose_ >= 3) cout << "Reading observation values from file ("
            << file_path_ << ") ..." << endl;
    handleError(readObservationsArrayData_(observations, start, count, stride));

    return (SUCCESS);
}

errorType
AnEnIO::readForecasts(Forecasts_array & forecasts) const {

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
            cout << BOLDRED << "Error: File type should be Forecasts."
                    << RESET << endl;
        }
        return (WRONG_FILE_TYPE);
    }

    anenPar::Parameters parameters;
    anenSta::Stations stations;
    anenTime::Times times;
    anenTime::FLTs flts;

    // Read meta information
    handleError(readParameters(parameters));
    handleError(readStations(stations));
    handleError(readTimes(times));
    handleError(readFLTs(flts));

    // Prepare the meta information in forecasts first
    forecasts.setParameters(parameters);
    forecasts.setStations(stations);
    forecasts.setTimes(times);
    forecasts.setFlts(flts);
    forecasts.updateDataDims();

    // Read data
    if (verbose_ >= 3) cout << "Reading forecast values from file ("
            << file_path_ << ") ..." << endl;
    vector<double> vals;
    handleError(read_vector_("Data", vals));
    forecasts.setValues(vals);


    return (SUCCESS);
}

errorType
AnEnIO::readForecasts(Forecasts_array & forecasts,
        vector<size_t> start, vector<size_t> count,
        vector<ptrdiff_t> stride) const {

    if (verbose_ >= 3) {
        cout << "Reading forecast file according to start and count ..." << endl;
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

    // Prepare the meta information in forecasts first
    forecasts.setParameters(parameters);
    forecasts.setStations(stations);
    forecasts.setTimes(times);
    forecasts.setFlts(flts);
    forecasts.updateDataDims();

    // Read data
    if (verbose_ >= 3) cout << "Reading partial forecast values from file ("
            << file_path_ << ") ..." << endl;
    vector<double> vals;
    handleError(read_vector_("Data", vals, start, count, stride));
    forecasts.setValues(vals);

    return (SUCCESS);
}

errorType
AnEnIO::readFLTs(anenTime::FLTs& flts) const {

    if (verbose_ >= 3) {
        cout << "Reading FLTs from file (" << file_path_ << ") ..." << endl;
    }

    if (mode_ != "Read") {
        if (verbose_ >= 1) cout << BOLDRED << "Error: Mode should be 'Read'."
                << RESET << endl;
        return (WRONG_MODE);
    }

    if (file_type_ == "Observations") {
        if (verbose_ >= 1) cout << BOLDRED
                << "Error: File type is set as Observations but observations do not have FLTs!" << RESET << endl;
        return (WRONG_FILE_TYPE);
    }

    string var_name = "FLTs";

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
        size_t start, size_t count, ptrdiff_t stride) const {

    if (verbose_ >= 3) {
        cout << "Reading FLTs from file ("
                << file_path_ << ") ..." << endl;
    }

    if (mode_ != "Read") {
        if (verbose_ >= 1) cout << BOLDRED << "Error: Mode should be 'Read'."
                << RESET << endl;
        return (WRONG_MODE);
    }

    string var_name = "FLTs";

    size_t dim_len;
    handleError(readDimLength("num_flts", dim_len));

    // Check parameters
    if (start + (count - 1) * stride >= dim_len) {
        if (verbose_ >= 1) cout << BOLDRED
                << "Error: The FLT indices are not valid."
                << " The length of FLTs is " << dim_len << RESET << endl;
        return (WRONG_INDEX_SHAPE);
    }

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
AnEnIO::readParameters(anenPar::Parameters & parameters) const {

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
    read_string_vector_("ParameterNames", names);

    if (names.size() != dim_len) {
        if (verbose_ >= 1) {
            cout << BOLDRED << "Error: There should be " << dim_len
                    << " parameter names!" << RESET << endl;
        }

        return (WRONG_VARIABLE_SHAPE);
    }

    // Read variable ParameterCirculars
    vector<string> circulars;
    handleError(read_string_vector_("ParameterCirculars", circulars));

    // Read variable ParameterWeights
    vector<double> weights;
    handleError(read_vector_("ParameterWeights", weights));

    // Construct anenPar::Parameter objects and
    // insert them into anenPar::Parameters
    //
    for (size_t i = 0; i < dim_len; i++) {

        anenPar::Parameter parameter;

        string name = names.at(i);
        parameter.setName(name);

        // Set circular if name is present
        auto it_circular = find(circulars.begin(), circulars.end(), name);
        if (it_circular != circulars.end()) parameter.setCircular(true);

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
        size_t start, size_t count, ptrdiff_t stride) const {

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

    // Check parameters
    if (start + (count - 1) * stride >= dim_len) {
        if (verbose_ >= 1) cout << BOLDRED
                << "Error: The parameter indices are not valid."
                << " The length of parameters is " << dim_len << RESET << endl;
        return (WRONG_INDEX_SHAPE);
    }

    // Read variable ParameterNames
    vector<string> names;
    read_string_vector_("ParameterNames", names, start, count, stride);

    // Read variable ParameterCirculars
    vector<string> circulars;
    handleError(read_string_vector_("ParameterCirculars",
            circulars, start, count, stride));

    // Read variable ParameterWeights
    vector<double> weights;
    handleError(read_vector_("ParameterWeights", weights, start, count, stride));

    // Construct anenPar::Parameter objects and
    // insert them into anenPar::Parameters
    //
    for (size_t i = 0; i < count; i++) {

        anenPar::Parameter parameter;

        string name = names.at(i);
        parameter.setName(name);

        // Set circular if name is present
        auto it_circular = find(circulars.begin(), circulars.end(), name);
        if (it_circular != circulars.end()) parameter.setCircular(true);

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
AnEnIO::readStations(anenSta::Stations& stations,
        const string & dim_name_prefix,
        const string & var_name_prefix) const {

    if (verbose_ >= 3) {
        cout << "Reading Stations from file (" << file_path_ << ") ..." << endl;
    }

    if (mode_ != "Read") {
        if (verbose_ >= 1) cout << BOLDRED << "Error: Mode should be 'Read'."
                << RESET << endl;
        return (WRONG_MODE);
    }

    size_t dim_len;
    handleError(readDimLength(dim_name_prefix + "num_stations", dim_len));

    // Read variable StationNames
    vector<string> names;
    read_string_vector_(var_name_prefix + "StationNames", names);

    if (names.size() != dim_len) {
        if (verbose_ >= 1) {
            cout << BOLDRED << "Error: There should be " << dim_len << " "
                    << " station names!" << RESET << endl;
        }
        return (WRONG_VARIABLE_SHAPE);
    }

    // Read variables xs and ys (coordinates)
    vector<double> xs, ys;
    handleError(read_vector_(var_name_prefix + "Xs", xs));
    handleError(read_vector_(var_name_prefix + "Ys", ys));

    if (xs.size() != dim_len) {
        if (verbose_ >= 1) {
            cout << BOLDRED << "Error: There should be " << dim_len
                    << " Xs!" << RESET << endl;
        }
        return (WRONG_VARIABLE_SHAPE);
    }
    if (ys.size() != dim_len) {
        if (verbose_ >= 1) {
            cout << BOLDRED << "Error: There should be " << dim_len
                    << " Ys!" << RESET << endl;
        }
        return (WRONG_VARIABLE_SHAPE);
    }

    // Construct Station object and insert them into anenSta::Stations
    for (size_t i = 0; i < dim_len; i++) {
        anenSta::Station station;

        station.setName(names.at(i));

        station.setX(xs.at(i));
        station.setY(ys.at(i));

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
        size_t start, size_t count, ptrdiff_t stride,
        const string & dim_name_prefix,
        const string & var_name_prefix) const {

    if (verbose_ >= 3) {
        cout << "Reading " << var_name_prefix << "Stations from file ("
                << file_path_ << ") ..." << endl;
    }

    if (mode_ != "Read") {
        if (verbose_ >= 1) cout << BOLDRED << "Error: Mode should be 'Read'."
                << RESET << endl;
        return (WRONG_MODE);
    }

    size_t dim_len;
    handleError(readDimLength(dim_name_prefix + "num_stations", dim_len));

    // Check parameters
    if (start + (count - 1) * stride >= dim_len) {
        if (verbose_ >= 1) cout << BOLDRED
                << "Error: The station indices are not valid."
                << " The length of stations is " << dim_len << RESET << endl;
        return (WRONG_INDEX_SHAPE);
    }

    // Read variable StationNames
    vector<string> names;
    read_string_vector_(var_name_prefix + "StationNames",
            names, start, count, stride);

    // Read variables xs and ys (coordinates)
    vector<double> xs, ys;
    handleError(read_vector_(var_name_prefix + "Xs", xs, start, count, stride));
    handleError(read_vector_(var_name_prefix + "Ys", ys, start, count, stride));

    // Construct Station object and insert them into anenSta::Stations
    for (size_t i = 0; i < count; i++) {
        anenSta::Station station;

        station.setName(names.at(i));
        station.setX(xs.at(i));
        station.setY(ys.at(i));

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
AnEnIO::readTimes(anenTime::Times& times, const string & var_name) const {

    if (verbose_ >= 3) {
        cout << "Reading " << var_name << " from file (" << file_path_
                << ")  ..." << endl;
    }

    if (mode_ != "Read") {
        if (verbose_ >= 1) cout << BOLDRED << "Error: Mode should be 'Read'."
                << RESET << endl;
        return (WRONG_MODE);
    }

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
        size_t start, size_t count, ptrdiff_t stride,
        const string & var_name) const {

    if (verbose_ >= 3) {
        cout << "Reading Times from file ("
                << file_path_ << ")   ..." << endl;
    }

    if (mode_ != "Read") {
        if (verbose_ >= 1) cout << BOLDRED << "Error: Mode should be 'Read'."
                << RESET << endl;
        return (WRONG_MODE);
    }

    size_t dim_len;
    handleError(readDimLength("num_times", dim_len));

    // Check parameters
    if (start + (count - 1) * stride >= dim_len) {
        if (verbose_ >= 1) cout << BOLDRED
                << "Error: The time indices are not valid."
                << " The length of times is " << dim_len << RESET << endl;
        return (WRONG_INDEX_SHAPE);
    }

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
AnEnIO::readDimLength(string dim_name, size_t & len) const {

    if (verbose_ >= 3) {
        cout << "Reading dimension (" << dim_name << ") length ..." << endl;
    }

    if (mode_ != "Read") {
        if (verbose_ >= 1) cout << BOLDRED << "Error: Mode should be 'Read'."
                << RESET << endl;
        return (WRONG_MODE);
    }

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
        const anenSta::Stations& stations, bool unlimited,
        const string & dim_name_prefix,
        const string & var_name_prefix) const {

    if (verbose_ >= 3) cout << "Writing variable (" << var_name_prefix << "Stations) ..." << endl;

    if (mode_ != "Write") {
        if (verbose_ >= 1) cout << BOLDRED << "Error: Mode should be 'Write'."
                << RESET << endl;
        return (WRONG_MODE);
    }

    NcFile nc(file_path_, NcFile::FileMode::write);

    // Check if file already has dimensions
    NcDim dim_stations = nc.getDim(dim_name_prefix + "num_stations");
    if (!dim_stations.isNull()) {
        if (dim_stations.getSize() != stations.size() ||
                dim_stations.isUnlimited() != unlimited) {
            if (verbose_ >= 1) cout << BOLDRED
                    << "Error: Dimension (" << dim_name_prefix
                    << "num_stations) exists with a "
                    << "different length in file (" << file_path_ << ")."
                    << RESET << endl;
            nc.close();
            return (DIMENSION_EXISTS);
        }
    } else {
        if (unlimited) {
            dim_stations = nc.addDim(dim_name_prefix + "num_stations");
        } else {
            dim_stations = nc.addDim(dim_name_prefix + "num_stations", stations.size());
        }
    }

    NcDim dim_chars = nc.getDim("num_chars");
    if (dim_chars.isNull()) {
        dim_chars = nc.addDim("num_chars", _max_chars);
    }

    // Check if file already has variable
    NcVar var;
    vector<string> var_names_to_check ={var_name_prefix + "StationNames", var_name_prefix + "Xs", var_name_prefix + "Ys"};
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
    NcVar var_names = nc.addVar(var_name_prefix + "StationNames", NcType::nc_CHAR, dim_names);
    NcVar var_xs = nc.addVar(var_name_prefix + "Xs", NcType::nc_DOUBLE, dim_stations);
    NcVar var_ys = nc.addVar(var_name_prefix + "Ys", NcType::nc_DOUBLE, dim_stations);

    // Convert types for writing
    char* p_names = nullptr;
    double* p_xs = nullptr;
    double* p_ys = nullptr;
    try {
        p_names = new char [_max_chars * stations.size()]();
        p_xs = new double [stations.size()]();
        p_ys = new double [stations.size()]();
    } catch (bad_alloc & e) {
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
AnEnIO::writeTimes(const anenTime::Times& times, bool unlimited,
        const string & dim_name, const string & var_name) const {

    if (verbose_ >= 3) cout << "Writing variable (" << var_name << ") ..." << endl;

    if (mode_ != "Write") {
        if (verbose_ >= 1) cout << BOLDRED << "Error: Mode should be 'Write'."
                << RESET << endl;
        return (WRONG_MODE);
    }

    NcFile nc(file_path_, NcFile::FileMode::write);

    // Check if file already has dimension
    NcDim dim_times = nc.getDim(dim_name);
    if (!dim_times.isNull()) {
        if (dim_times.getSize() != times.size() ||
                dim_times.isUnlimited() != unlimited) {
            if (verbose_ >= 1) cout << BOLDRED << "Error: Dimension ("
                    << dim_name << ") with a different length exists in file ("
                    << file_path_ << ")." << RESET << endl;
            nc.close();
            return (DIMENSION_EXISTS);
        }
    } else {
        // Create dimension
        if (unlimited) {
            dim_times = nc.addDim(dim_name);
        } else {
            dim_times = nc.addDim(dim_name, times.size());
        }
    }

    // Check if file already has variable
    NcVar var = nc.getVar(var_name);
    if (!var.isNull()) {
        if (verbose_ >= 1) cout << BOLDRED << "Error: Variable (" << var_name
                << ") exists in file (" << file_path_ << ")." << RESET << endl;
        nc.close();
        return (VARIABLE_EXISTS);
    }

    var = nc.addVar(var_name, NcType::nc_DOUBLE, dim_times);

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

    // Resize similarity matrices to clear any leftover values
    SimilarityMatrices::extent_gen extens;
    sims.resize(0, 0, 0);

    // Resize similarity matrices to the correct dimensions
    NcFile nc(file_path_, NcFile::FileMode::read);
    sims.setMaxEntries(nc.getDim("num_entries").getSize());
    sims.resize(nc.getDim("num_stations").getSize(),
            nc.getDim("num_times").getSize(),
            nc.getDim("num_flts").getSize());
    nc.close();

    vector<double> values;
    handleError(read_vector_("SimilarityMatrices", values));
    sims.assign(values.begin(), values.end());

    return (SUCCESS);
}

errorType
AnEnIO::writeSimilarityMatrices(
        const SimilarityMatrices & sims,
        const anenPar::Parameters & parameters,
        const anenSta::Stations & test_stations,
        const anenTime::Times & test_times,
        const anenTime::FLTs & flts,
        const anenSta::Stations & search_stations,
        const anenTime::Times & search_times) const {

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

    handleError(writeSimilarityMatricesOnly_(sims));
    handleError(writeParameters(parameters, false));
    handleError(writeStations(test_stations, false));
    handleError(writeTimes(test_times, false));
    handleError(writeFLTs(flts, false));
    handleError(writeStations(search_stations, false, "search_", "Search"));
    handleError(writeTimes(search_times,
            false, "search_num_times", "SearchTimes"));

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

    NcFile nc(file_path_, NcFile::FileMode::read);
    analogs.resize(boost::extents
            [nc.getDim("num_stations").getSize()]
            [nc.getDim("num_times").getSize()]
            [nc.getDim("num_flts").getSize()]
            [nc.getDim("num_members").getSize()]
            [nc.getDim("num_cols").getSize()]);
    nc.close();

    vector<double> values;
    handleError(read_vector_("Analogs", values));
    analogs.assign(values.begin(), values.end());

    return (SUCCESS);
}

errorType
AnEnIO::writeAnalogs(
        const Analogs & analogs,
        const anenSta::Stations & test_stations,
        const anenTime::Times & test_times,
        const anenTime::FLTs & flts,
        const anenSta::Stations & search_stations,
        const anenTime::Times & search_times) const {

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

    handleError(writeAnalogsOnly_(analogs));
    handleError(writeStations(test_stations, false));
    handleError(writeTimes(test_times, false));
    handleError(writeFLTs(flts, false));
    handleError(writeStations(search_stations, false, "member_", "Member"));
    handleError(writeTimes(search_times, false, "member_num_times", "MemberTimes"));

    return (SUCCESS);
}

errorType
AnEnIO::readStandardDeviation(StandardDeviation & sds) {
    if (verbose_ >= 3) cout << "Reading standard deviation ..." << endl;

    if (mode_ != "Read") {
        if (verbose_ >= 1) cout << BOLDRED << "Error: Mode should be 'Read'."
                << RESET << endl;
        return (WRONG_MODE);
    }

    if (file_type_ != "StandardDeviation") {
        if (verbose_ >= 1)
            cout << BOLDRED << "Error: file type should be StandardDeviation."
                << RESET << endl;
        return (WRONG_FILE_TYPE);
    }

    NcFile nc(file_path_, NcFile::FileMode::read);
    sds.resize(boost::extents
            [nc.getDim("num_parameters").getSize()]
            [nc.getDim("num_stations").getSize()]
            [nc.getDim("num_flts").getSize()]);
    nc.close();

    vector<double> values;
    handleError(read_vector_("StandardDeviation", values));
    sds.assign(values.begin(), values.end());

    return (SUCCESS);
}

errorType
AnEnIO::readStandardDeviation(StandardDeviation & sds,
        vector<size_t> start, vector<size_t> count, vector<ptrdiff_t> stride) {
    if (verbose_ >= 3) cout << "Reading partial standard deviation ..." << endl;

    if (mode_ != "Read") {
        if (verbose_ >= 1) cout << BOLDRED << "Error: Mode should be 'Read'."
                << RESET << endl;
        return (WRONG_MODE);
    }

    if (file_type_ != "StandardDeviation") {
        if (verbose_ >= 1)
            cout << BOLDRED << "Error: file type should be StandardDeviation."
                << RESET << endl;
        return (WRONG_FILE_TYPE);
    }

    // Check indices
    if (start.size() != 3) {
        if (verbose_ >= 1) cout << BOLDRED
                << "Error: Start should have 3 indices for StandardDeviation."
                << RESET << endl;
        return (WRONG_INDEX_SHAPE);
    }

    if (count.size() != 3) {
        if (verbose_ >= 1) cout << BOLDRED
                << "Error: Count should have 3 indices for StandardDeviation."
                << RESET << endl;
        return (WRONG_INDEX_SHAPE);
    }

    if (stride.size() != 3) {
        if (verbose_ >= 1) cout << BOLDRED
                << "Error: Stride should have 3 indices for StandardDeviation."
                << RESET << endl;
        return (WRONG_INDEX_SHAPE);
    }

    sds.resize(boost::extents[count[0]][count[1]][count[2]]);

    // Read data
    if (verbose_ >= 3) cout << "Reading StandardDeviation values from file ("
            << file_path_ << ") ..." << endl;
    vector<double> values;
    handleError(read_vector_("StandardDeviation", values,
            start, count, stride));
    sds.assign(values.begin(), values.end());

    return (SUCCESS);
}

errorType
AnEnIO::writeStandardDeviation(
        const StandardDeviation & sds,
        const anenPar::Parameters & parameters,
        const anenSta::Stations & stations,
        const anenTime::FLTs & flts) const {

    if (verbose_ >= 3) cout << "Writing standard deviation ..." << endl;

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

    handleError(writeStandardDeviationOnly_(sds));
    handleError(writeParameters(parameters, false));
    handleError(writeStations(stations, false));
    handleError(writeFLTs(flts, false));

    return (SUCCESS);
}

void
AnEnIO::handleError(const errorType & indicator) const {
    if (indicator == SUCCESS ||
            indicator == OPTIONAL_VARIABLE_MISSING) {
        return;
    } else {
        throw runtime_error("Error code "
                + to_string((long long) indicator)
                + "\nCode reference: https://weiming-hu.github.io/AnalogsEnsemble/CXX/class_an_en_i_o.html#aa56bc1ec6610b86db4349bce20f9ead0");
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
AnEnIO::combineParameters(
        const vector<string> & in_files,
        const string & file_type,
        anenPar::Parameters & parameters, int verbose) {

    if (verbose >= 3) cout << "Combining parameters ..." << endl;

    parameters.clear();
    for_each(in_files.begin(), in_files.end(),
            [&parameters, &file_type]
            (const string & file) {
                AnEnIO io("Read", file, file_type, 2);
                anenPar::Parameters parameters_single;
                io.handleError(io.readParameters(parameters_single));
                parameters.insert(parameters.end(),
                        parameters_single.begin(), parameters_single.end());
            }
    );

    return (SUCCESS);
}

errorType
AnEnIO::combineStations(
        const vector<string> & in_files,
        const string & file_type,
        anenSta::Stations & stations, int verbose) {

    if (verbose >= 3) cout << "Combining stations ..." << endl;

    stations.clear();
    for_each(in_files.begin(), in_files.end(),
            [&stations, &file_type]
            (const string & file) {
                AnEnIO io("Read", file, file_type, 2);
                anenSta::Stations stations_single;
                io.handleError(io.readStations(stations_single));
                stations.insert(stations.end(),
                        stations_single.begin(), stations_single.end());
            }
    );

    return (SUCCESS);
}

errorType
AnEnIO::combineTimes(
        const vector<string> & in_files,
        const string & file_type,
        anenTime::Times & times, int verbose) {

    if (verbose >= 3) cout << "Combining times ..." << endl;

    times.clear();
    for_each(in_files.begin(), in_files.end(),
            [&times, &file_type]
            (const string & file) {
                AnEnIO io("Read", file, file_type, 2);
                anenTime::Times times_single;
                io.handleError(io.readTimes(times_single));
                times.insert(times.end(),
                        times_single.begin(), times_single.end());
            }
    );

    return (SUCCESS);
}

errorType
AnEnIO::combineFLTs(
        const vector<string> & in_files,
        const string & file_type,
        anenTime::FLTs & flts, int verbose) {

    if (verbose >= 3) cout << "Combining FLTs ..." << endl;

    flts.clear();
    for_each(in_files.begin(), in_files.end(),
            [&flts, &file_type]
            (const string & file) {
                AnEnIO io("Read", file, file_type, 2);
                anenTime::FLTs flts_single;
                io.handleError(io.readFLTs(flts_single));
                flts.insert(flts.end(),
                        flts_single.begin(), flts_single.end());
            }
    );

    return (SUCCESS);
}

errorType
AnEnIO::combineForecastsArray(const vector<string> & in_files,
        Forecasts_array & forecasts, size_t along, int verbose) {

    // Clear values in forecasts array data
    forecasts.data().resize(boost::extents[0][0][0][0]);

    // Create meta information from the first file
    if (verbose >= 3) cout << "Processing meta information ..." << endl;
    AnEnIO io("Read", in_files[0], "Forecasts", 2);

    anenPar::Parameters parameters;
    io.handleError(io.readParameters(parameters));

    anenSta::Stations stations;
    io.handleError(io.readStations(stations));

    anenTime::Times times;
    io.handleError(io.readTimes(times));

    anenTime::FLTs flts;
    io.handleError(io.readFLTs(flts));

    if (along == 0) io.handleError(io.combineParameters(in_files, "Forecasts", parameters, verbose));
    else if (along == 1) io.handleError(io.combineStations(in_files, "Forecasts", stations, verbose));
    else if (along == 2) io.handleError(io.combineTimes(in_files, "Forecasts", times, verbose));
    else if (along == 3) io.handleError(io.combineFLTs(in_files, "Forecasts", flts, verbose));
    else return (ERROR_SETTING_VALUES);

    // Update the dimensions of combined forecasts
    if (verbose >= 3) cout << "Update dimensions of forecasts ..." << endl;
    forecasts.setParameters(parameters);
    forecasts.setStations(stations);
    forecasts.setTimes(times);
    forecasts.setFlts(flts);
    forecasts.updateDataDims(false);
    auto & data = forecasts.data();

    // Identify which dimension is being appended. The appended dimension number
    // will be removed, and the remain dimensions will stay the same after the
    // data combination.
    //
    vector<size_t> same_dimensions = {0, 1, 2, 3};
    same_dimensions.erase(find(same_dimensions.begin(), same_dimensions.end(), along));

    if (verbose >= 3) cout << "Copy data values into the new format ..." << endl;

    size_t append_count = 0;

#if defined(_OPENMP)
#pragma omp parallel default(none) \
shared(data, same_dimensions, along, append_count, in_files)
#endif
    for (const auto & file : in_files) {
        AnEnIO io_thread("Read", in_files[0], "Forecasts", 2);
        Forecasts_array forecasts_single;

        io_thread.setFilePath(file);
        io_thread.readForecasts(forecasts_single);
        const auto & data_single = forecasts_single.data();

#if defined(_OPENMP)
#pragma omp for schedule(static) collapse(4)
#endif
        for (size_t i = 0; i < data.shape()[same_dimensions[2]]; i++) {
            for (size_t j = 0; j < data.shape()[same_dimensions[1]]; j++) {
                for (size_t m = 0; m < data.shape()[same_dimensions[0]]; m++) {
                    for (size_t l = 0; l < data_single.shape()[along]; l++) {
                        if (along == 0) data[append_count + l][m][j][i] = data_single[l][m][j][i];
                        else if (along == 1) data[m][append_count + l][j][i] = data_single[m][l][j][i];
                        else if (along == 2) data[m][j][append_count + l][i] = data_single[m][j][l][i];
                        else if (along == 3) data[m][j][i][append_count + l] = data_single[m][j][i][l];
                    }
                }
            }
        }

#if defined(_OPENMP)
#pragma omp single
#endif
        append_count += data_single.shape()[along];
    }

    return (SUCCESS);
}

errorType
AnEnIO::combineObservationsArray(const vector<string> & in_files,
        Observations_array & observations, size_t along, int verbose) {

    // Clear values in observations array data
    observations.data().resize(boost::extents[0][0][0]);

    // Create meta information from the first file
    if (verbose >= 3) cout << "Processing meta information ..." << endl;
    AnEnIO io("Read", in_files[0], "Observations", 2);

    anenPar::Parameters parameters;
    io.handleError(io.readParameters(parameters));

    anenSta::Stations stations;
    io.handleError(io.readStations(stations));

    anenTime::Times times;
    io.handleError(io.readTimes(times));

    if (along == 0) io.handleError(io.combineParameters(in_files, "Observations", parameters, verbose));
    else if (along == 1) io.handleError(io.combineStations(in_files, "Observations", stations, verbose));
    else if (along == 2) io.handleError(io.combineTimes(in_files, "Observations", times, verbose));
    else return (ERROR_SETTING_VALUES);

    // Update dimensions
    if (verbose >= 3) cout << "Update dimensions of observations ..." << endl;
    observations.setParameters(parameters);
    observations.setStations(stations);
    observations.setTimes(times);
    observations.updateDataDims(false);
    auto & data = observations.data();

    // Identify which dimension is being appended. The appended dimension number
    // will be removed, and the remain dimensions will stay the same after the
    // data combination.
    //
    vector<size_t> same_dimensions = {0, 1, 2};
    same_dimensions.erase(find(same_dimensions.begin(), same_dimensions.end(), along));

    if (verbose >= 3) cout << "Copy data values into the new format ..." << endl;

    size_t append_count = 0;

#if defined(_OPENMP)
#pragma omp parallel default(none) \
shared(data, same_dimensions, along, append_count, in_files)
#endif
    for (const auto & file : in_files) {
        AnEnIO io_thread("Read", in_files[0], "Observations", 2);
        Observations_array observations_single;

        io_thread.setFilePath(file);
        io_thread.readObservations(observations_single);
        const auto & data_single = observations_single.data();

#if defined(_OPENMP)
#pragma omp for schedule(static) collapse(3)
#endif
        for (size_t j = 0; j < data.shape()[same_dimensions[1]]; j++) {
            for (size_t m = 0; m < data.shape()[same_dimensions[0]]; m++) {
                for (size_t l = 0; l < data_single.shape()[along]; l++) {
                    if (along == 0) data[append_count + l][m][j] = data_single[l][m][j];
                    else if (along == 1) data[m][append_count + l][j] = data_single[m][l][j];
                    else if (along == 2) data[m][j][append_count + l] = data_single[m][j][l];
                }
            }
        }

#if defined(_OPENMP)
#pragma omp single
#endif
        append_count += data_single.shape()[along];
    }

    return (SUCCESS);
}

errorType
AnEnIO::combineStandardDeviation(const vector<string> & in_files,
        StandardDeviation & sds, 
        anenPar::Parameters & parameters,
        anenSta::Stations & stations,
        anenTime::FLTs & flts,
        size_t along, int verbose) {

    // Clear values in sds
    sds.resize(boost::extents[0][0][0]);

    // Create meta information from the first file
    if (verbose >= 3) cout << "Processing meta information ..." << endl;
    AnEnIO io("Read", in_files[0], "StandardDeviation", 2);

    io.handleError(io.readParameters(parameters));
    io.handleError(io.readStations(stations));
    io.handleError(io.readFLTs(flts));
    
    if (along == 0) io.handleError(io.combineParameters(in_files, "StandardDeviation", parameters, verbose));
    else if (along == 1) io.handleError(io.combineStations(in_files, "StandardDeviation", stations, verbose));
    else if (along == 2) io.handleError(io.combineFLTs(in_files, "StandardDeviation", flts, verbose));
    else return (ERROR_SETTING_VALUES);
    
    // Identify which dimension is being appended. The appended dimension number
    // will be removed, and the remain dimensions will stay the same after the
    // data combination.
    //
    vector<size_t> same_dimensions = {0, 1, 2};
    same_dimensions.erase(find(same_dimensions.begin(), same_dimensions.end(), along));
    
    // Update dimensions
    if (verbose >= 3) cout << "Update dimensions of standard deviation ..." << endl;
    sds.resize(boost::extents[parameters.size()][stations.size()][flts.size()]);

    if (verbose >= 3) cout << "Copy data values into the new format ..." << endl;

    size_t append_count = 0;

#if defined(_OPENMP)
#pragma omp parallel default(none) \
shared(sds, same_dimensions, along, append_count, in_files)
#endif
    for (const auto & file : in_files) {
        AnEnIO io_thread("Read", in_files[0], "StandardDeviation", 2);
        StandardDeviation sds_single;

        io_thread.setFilePath(file);
        io_thread.readStandardDeviation(sds_single);

#if defined(_OPENMP)
#pragma omp for schedule(static) collapse(3)
#endif
        for (size_t j = 0; j < sds.shape()[same_dimensions[1]]; j++) {
            for (size_t m = 0; m < sds.shape()[same_dimensions[0]]; m++) {
                for (size_t l = 0; l < sds_single.shape()[along]; l++) {
                    if (along == 0) sds[append_count + l][m][j] = sds_single[l][m][j];
                    else if (along == 1) sds[m][append_count + l][j] = sds_single[m][l][j];
                    else if (along == 2) sds[m][j][append_count + l] = sds_single[m][j][l];
                }
            }
        }

#if defined(_OPENMP)
#pragma omp single
#endif
        append_count += sds_single.shape()[along];
    }

    return (SUCCESS);
}

errorType
AnEnIO::combineSimilarityMatrices(
        const std::vector<std::string> & in_files,
        SimilarityMatrices & sims,
        anenSta::Stations & stations,
        anenTime::Times & times,
        anenTime::FLTs & flts,
        size_t along, int verbose) {
    
    // Clear values
    sims.resize(0, 0, 0);

    // Create meta information from the first file
    if (verbose >= 3) cout << "Processing meta information ..." << endl;
    AnEnIO io("Read", in_files[0], "Similarity", 2);
    
    size_t num_entries;
    io.readDimLength("num_entries", num_entries);

    io.handleError(io.readStations(stations));
    io.handleError(io.readTimes(times));
    io.handleError(io.readFLTs(flts));

    if (along == 0) {
        if (verbose >= 1) cout << BOLDRED << "Error: Can not append along the dimension."
                << RESET << endl;
        return (ERROR_SETTING_VALUES);
    } else if (along == 1) {
        num_entries = 0;
        size_t num_entries_single = 0;
        for_each(in_files.begin(), in_files.end(),
                [&num_entries, &num_entries_single]
                (const string & file) {
                    AnEnIO io_each("Read", file, "Similarity", 2);
                    io_each.readDimLength("num_entries", num_entries_single);
                    num_entries += num_entries_single;
                }
        );
    } else if (along == 2)
        io.handleError(io.combineFLTs(in_files, "Similarity", flts, verbose));
    else if (along == 3)
        io.handleError(io.combineTimes(in_files, "Similarity", times, verbose));
    else if (along == 4)
        io.handleError(io.combineStations(in_files, "Similarity", stations, verbose));
    else
        return (ERROR_SETTING_VALUES);

    // Identify which dimension is being appended. The appended dimension number
    // will be removed, and the remain dimensions will stay the same after the
    // data combination.
    //
    vector<size_t> same_dimensions = {1, 2, 3, 4};
    same_dimensions.erase(find(same_dimensions.begin(), same_dimensions.end(), along));
    
    // Update dimensions
    if (verbose >= 3) cout << "Update dimensions of standard deviation ..." << endl;
    sims.resize(flts.size(), times.size(), stations.size());

    if (verbose >= 3) cout << "Copy data values into the new format ..." << endl;

    size_t append_count = 0;

#if defined(_OPENMP)
#pragma omp parallel default(none) \
shared(sims, same_dimensions, along, append_count, in_files)
#endif
    for (const auto & file : in_files) {
        AnEnIO io_thread("Read", in_files[0], "Similarity", 2);
        SimilarityMatrices sims_single;

        io_thread.setFilePath(file);
        io_thread.readSimilarityMatrices(sims_single);

#if defined(_OPENMP)
#pragma omp for schedule(static) collapse(5)
#endif
        for (size_t k = 0; k < sims.shape()[same_dimensions[2]]; k++) {
            for (size_t j = 0; j < sims.shape()[same_dimensions[1]]; j++) {
                for (size_t m = 0; m < sims.shape()[same_dimensions[0]]; m++) {
                    for (size_t l = 0; l < sims_single.shape()[along]; l++) {
                        for (size_t i = 0; i < SimilarityMatrices::_NUM_COLS; i++) {
                            
                            if (along == 1) sims[i][append_count + l][m][j][k] = sims_single[i][l][m][j][k];
                            else if (along == 2) sims[i][m][append_count + l][j][k] = sims_single[i][m][l][j][k];
                            else if (along == 3) sims[i][m][j][append_count + l][k] = sims_single[i][m][j][l][k];
                            else if (along == 4) sims[i][m][j][k][append_count + l] = sims_single[i][m][j][k][l];
                            
                        }
                    }
                }
            }
        }

#if defined(_OPENMP)
#pragma omp single
#endif
        append_count += sims_single.shape()[along];
    }

    return (SUCCESS);
}

errorType
AnEnIO::combineAnalogs(const vector<string> & in_files,
        Analogs & analogs, 
        anenSta::Stations & stations,
        anenTime::Times & times,
        anenTime::FLTs & flts,
        size_t along, int verbose) {

    // Clear values
    analogs.resize(boost::extents[0][0][0][0][0]);

    // Create meta information from the first file
    if (verbose >= 3) cout << "Processing meta information ..." << endl;
    AnEnIO io("Read", in_files[0], "Analogs", 2);

    size_t num_members;
    io.readDimLength("num_entries", num_members);

    io.handleError(io.readStations(stations));
    io.handleError(io.readTimes(times));
    io.handleError(io.readFLTs(flts));

    if (along == 4) {
        if (verbose >= 1) cout << BOLDRED << "Error: Can not append along the dimension."
                << RESET << endl;
        return (ERROR_SETTING_VALUES);
    } else if (along == 3) {
        num_members = 0;
        size_t num_entries_single = 0;
        for_each(in_files.begin(), in_files.end(),
                [&num_members, &num_entries_single]
                (const string & file) {
                    AnEnIO io_each("Read", file, "Analogs", 2);
                    io_each.readDimLength("num_members", num_entries_single);
                    num_members += num_entries_single;
                }
        );
    } else if (along == 2)
        io.handleError(io.combineFLTs(in_files, "Analogs", flts, verbose));
    else if (along == 1)
        io.handleError(io.combineTimes(in_files, "Analogs", times, verbose));
    else if (along == 0)
        io.handleError(io.combineStations(in_files, "Analogs", stations, verbose));
    else
        return (ERROR_SETTING_VALUES);

    // Identify which dimension is being appended. The appended dimension number
    // will be removed, and the remain dimensions will stay the same after the
    // data combination.
    //
    vector<size_t> same_dimensions = {0, 1, 2, 3};
    same_dimensions.erase(find(same_dimensions.begin(), same_dimensions.end(), along));

    // Update dimensions
    if (verbose >= 3) cout << "Update dimensions of Analogs ..." << endl;
    analogs.resize(boost::extents
            [stations.size()][times.size()][flts.size()][num_members][Analogs::_NUM_COLS]);

    if (verbose >= 3) cout << "Copy data values into the new format ..." << endl;

    size_t append_count = 0;

#if defined(_OPENMP)
#pragma omp parallel default(none) \
shared(analogs, same_dimensions, along, append_count, in_files)
#endif
    for (const auto & file : in_files) {
        AnEnIO io_thread("Read", in_files[0], "Analogs", 2);
        Analogs analogs_single;

        io_thread.setFilePath(file);
        io_thread.readAnalogs(analogs_single);

#if defined(_OPENMP)
#pragma omp for schedule(static) collapse(5)
#endif
        for (size_t k = 0; k < analogs.shape()[same_dimensions[2]]; k++) {
            for (size_t j = 0; j < analogs.shape()[same_dimensions[1]]; j++) {
                for (size_t m = 0; m < analogs.shape()[same_dimensions[0]]; m++) {
                    for (size_t l = 0; l < analogs_single.shape()[along]; l++) {
                        for (size_t i = 0; i < Analogs::_NUM_COLS; i++) {

                            if (along == 0) analogs[append_count + l][m][j][k][i] = analogs_single[l][m][j][k][i];
                            else if (along == 1) analogs[m][append_count + l][j][k][i] = analogs_single[m][l][j][k][i];
                            else if (along == 2) analogs[m][j][append_count + l][k][i] = analogs_single[m][j][l][k][i];
                            else if (along == 3) analogs[m][j][k][append_count + l][i] = analogs_single[m][j][k][l][i];

                        }
                    }
                }
            }
        }

#if defined(_OPENMP)
#pragma omp single
#endif
        append_count += analogs_single.shape()[along];
    }

    return (SUCCESS);
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

errorType
AnEnIO::writeSimilarityMatricesOnly_(const SimilarityMatrices& sims) const {

    size_t num_test_stations = sims.shape()[0],
            num_test_times = sims.shape()[1],
            num_flts = sims.shape()[2],
            num_entries = sims.shape()[3],
            num_cols = sims.shape()[4];

    NcFile nc(file_path_, NcFile::FileMode::write);

    // Create dimensions
    NcDim dim_stations = nc.addDim("num_stations", num_test_stations);
    NcDim dim_times = nc.addDim("num_times", num_test_times);
    NcDim dim_flts = nc.addDim("num_flts", num_flts);
    NcDim dim_entries = nc.addDim("num_entries", num_entries);
    NcDim dim_cols = nc.addDim("num_cols", num_cols);

    // Create similarity matrices variable
    NcVar var_sims = nc.addVar("SimilarityMatrices", NC_DOUBLE,{
        dim_stations, dim_times, dim_flts, dim_entries, dim_cols
    });

    var_sims.putVar(sims.data());
    nc.close();

    return (SUCCESS);
}

errorType
AnEnIO::writeAnalogsOnly_(const Analogs& analogs) const {

    size_t num_test_stations = analogs.shape()[0],
            num_test_times = analogs.shape()[1],
            num_flts = analogs.shape()[2],
            num_members = analogs.shape()[3],
            num_cols = analogs.shape()[4];

    NcFile nc(file_path_, NcFile::FileMode::write);

    NcDim dim_stations = nc.addDim("num_stations", num_test_stations);
    NcDim dim_times = nc.addDim("num_times", num_test_times);
    NcDim dim_flts = nc.addDim("num_flts", num_flts);
    NcDim dim_members = nc.addDim("num_members", num_members);
    NcDim dim_cols = nc.addDim("num_cols", num_cols);

    // Create Analogs variable
    NcVar var_sims = nc.addVar("Analogs", NC_DOUBLE,{
        dim_cols, dim_members, dim_flts, dim_times, dim_stations
    });

    var_sims.putVar(analogs.data());
    nc.close();

    return (SUCCESS);
}

errorType
AnEnIO::writeStandardDeviationOnly_(const StandardDeviation& sds) const {

    size_t num_parameters = sds.shape()[0],
            num_stations = sds.shape()[1],
            num_flts = sds.shape()[2];

    NcFile nc(file_path_, NcFile::FileMode::write);
    NcDim dim_parameters = nc.addDim("num_parameters", num_parameters),
            dim_stations = nc.addDim("num_stations", num_stations),
            dim_flts = nc.addDim("num_flts", num_flts);
    NcVar var_sds = nc.addVar("StandardDeviation", NC_DOUBLE,{
        dim_flts, dim_stations, dim_parameters
    });

    var_sds.putVar(sds.data());
    nc.close();

    return (SUCCESS);
}

errorType
AnEnIO::readObservationsArrayData_(Observations_array & observations) const {

    try {

        string var_name = "Data";
        double *p_vals = observations.data().data();
        NcFile nc(file_path_, NcFile::FileMode::read);
        NcVar var = nc.getVar(var_name);

        if (var.isNull()) {
            if (verbose_ >= 1) cout << BOLDRED << "Error: Could not"
                    << " find variable " << var_name << "!" << RESET << endl;
            return (WRONG_INDEX_SHAPE);
        }

        // Please realize that I'm directly reading to the forecasts data
        // pointer which can be dangerous if handled uncautionsly. But I
        // don't have to create a copy of the data by doing this so I
        // think the benefit in memory and speed outweighs the downside.
        //
        var.getVar(p_vals);

        nc.close();
    } catch (...) {
        throw;
    }

    return (SUCCESS);
}

errorType
AnEnIO::readObservationsArrayData_(Observations_array & observations,
        vector<size_t> start, vector<size_t> count, vector<ptrdiff_t> stride) const {

    try {

        string var_name = "Data";
        double *p_vals = observations.data().data();
        NcFile nc(file_path_, NcFile::FileMode::read);
        NcVar var = nc.getVar(var_name);

        if (var.isNull()) {
            if (verbose_ >= 1) cout << BOLDRED << "Error: Could not"
                    << " find variable " << var_name << "!" << RESET << endl;
            return (WRONG_INDEX_SHAPE);
        }

        // Please realize that I'm directly reading to the forecasts data
        // pointer which can be dangerous if handled uncautionsly. But I
        // don't have to create a copy of the data by doing this so I
        // think the benefit in memory and speed outweighs the downside.
        //
        var.getVar(start, count, stride, p_vals);

        nc.close();
    } catch (...) {
        throw;
    }

    return (SUCCESS);
}