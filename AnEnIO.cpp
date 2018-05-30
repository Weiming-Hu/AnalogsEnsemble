/* 
 * File:   AnEnIO.cpp
 * Author: Weiming Hu (weiming@psu.edu)
 * 
 * Created on May 4, 2018, 11:16 AM
 */

#include "AnEnIO.h"

#include <boost/filesystem.hpp>
#include <algorithm>

namespace filesys = boost::filesystem;
using namespace netCDF;
using namespace std;

using errorType = AnEnIO::errorType;

AnEnIO::AnEnIO(string file_path) :
file_path_(file_path) {
    handleError(checkFile());
}

AnEnIO::AnEnIO(string file_path, string file_type) :
file_path_(file_path), file_type_(file_type) {
    handleError(checkFile());
    handleError(checkFileType());
}

AnEnIO::AnEnIO(string file_path, string file_type, int verbose) :
file_path_(file_path), file_type_(file_type), verbose_(verbose) {
    handleError(checkFile());
    handleError(checkFileType());
}

AnEnIO::AnEnIO(string file_path, string file_type, int verbose,
        vector<string> required_variables,
        vector<string> optional_variables) :
file_path_(file_path), file_type_(file_type), verbose_(verbose),
required_variables_(required_variables),
optional_variables_(optional_variables) {
    handleError(checkFile());
    handleError(checkFileType());
}

AnEnIO::~AnEnIO() {
}

errorType
AnEnIO::checkFile() const {

    if (verbose_ >= 3) {
        cout << "Checking file (" << file_path_ << ") ..." << endl;
    }

    if (filesys::exists(file_path_)) {

        filesys::path boost_path(file_path_);

        if (boost_path.has_extension()) {
            string ext = boost_path.extension().string();

            if (ext == ".nc") {
                return (SUCCESS);
            }
        }

        if (verbose_ >= 1) {
            cout << BOLDRED << "Error: unknown file type "
                    << file_path_ << RESET << endl;
        }
        return (UNKOWN_FILE_TYPE);
    } else {

        if (verbose_ >= 1) {
            cout << BOLDRED << "Error: File not found "
                    << file_path_ << RESET << endl;
        }
        return (FILE_NOT_FOUND);
    }
}

errorType
AnEnIO::checkFileType() const {

    if (verbose_ >= 3) {
        cout << "Checking file type (" << file_type_ << ") ..." << endl;
    }

    vector<string> dim_names, var_names;

    if (file_type_ == "Observations") {
        dim_names = {"num_parameters", "num_stations", "num_times"};
        var_names = {"Data"};
    } else if (file_type_ == "Forecasts") {
        dim_names = {"num_parameters", "num_stations", "num_times", "num_flts"};
        var_names = {"Data"};
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
}

errorType
AnEnIO::checkVariable(string var_name, bool optional) const {

    if (verbose_ >= 3) {
        cout << "Checking variable (" << var_name << ") ..." << endl;
    }

    NcFile nc(file_path_, NcFile::FileMode::read);
    bool is_null = nc.getVar(var_name).isNull();
    nc.close();

    if (is_null) {
        if (optional) {

            if (verbose_ >= 1) {
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
AnEnIO::readData(Observations & observations) {

    if (verbose_ >= 3) {
        cout << "Reading variable (Data) ..." << endl;
    }

    if (file_type_.empty()) {
        if (verbose_ >= 1) {
            cout << BOLDRED << "Error: File type not specified!"
                    << RESET << endl;
        }
        return (UNKOWN_FILE_TYPE);
    }

    vector<double> vals;
    handleError(read_vector_("Data", vals));

    if (observations.setValues(vals)) return (SUCCESS);
    else return (ERROR_SETTING_VALUES);
}

errorType
AnEnIO::readData(Forecasts & forecasts) {

    if (verbose_ >= 3) {
        cout << "Reading variable (Data) ..." << endl;
    }

    if (file_type_.empty()) {
        if (verbose_ >= 1) {
            cout << BOLDRED << "Error: File type not specified!"
                    << RESET << endl;
        }
        return (UNKOWN_FILE_TYPE);
    }

    vector<double> vals;
    read_vector_("Data", vals);

    if (forecasts.setValues(vals)) return (SUCCESS);
    else return (ERROR_SETTING_VALUES);
}

errorType
AnEnIO::readFLTs(FLTs& flts) {

    if (verbose_ >= 3) {
        cout << "Reading variable (FLTs) ..." << endl;
    }

    string var_name = "FLTs";
    handleError(checkVariable(var_name, false));

    vector<double> vec;
    read_vector_(var_name, vec);

    // copy the vector to the set
    copy(vec.begin(), vec.end(), inserter(flts, flts.end()));

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
AnEnIO::readParameters(Parameters & parameters) {

    if (verbose_ >= 3) {
        cout << "Reading parameters from file (" << file_path_ << ") ..." << endl;
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
                cout << BOLDRED << "Warning: There should be " << dim_len
                        << " parameter names! Variable not used!"
                        << RESET << endl;
            }
            with_names = false;
        }
    }

    // Read variable CircularParameters
    vector<string> circulars;
    if (SUCCESS == checkVariable("CircularParameters", true)) {
        read_vector_("CircularParameters", circulars);
    }

    // Construct Parameter objects and insert them into Parameters
    parameters.clear();
    for (size_t i = 0; i < dim_len; i++) {

        Parameter parameter;

        if (with_names) {
            // Set name
            string name = names.at(i);
            parameter.setName(name);

            // Set circular if name is present
            auto it_circular = find(circulars.begin(), circulars.end(), name);
            if (it_circular != circulars.end()) parameter.setCircular(true);
        }

        if (!parameters.insert(parameter).second) {
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
AnEnIO::readStations(Stations& stations) {

    if (verbose_ >= 3) {
        cout << "Reading stations from file (" << file_path_ << ") ..." << endl;
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
                cout << BOLDRED << "Warning: There should be " << dim_len
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
        read_vector_("Xs", xs);
        read_vector_("Ys", ys);
        with_coordinates = true;

        if (xs.size() != dim_len) {
            if (verbose_ >= 2) {
                cout << BOLDRED << "Warning: There should be " << dim_len
                        << " Xs! Coordinate variables not used!"
                        << RESET << endl;
            }
            with_coordinates = false;
        }
        if (ys.size() != dim_len) {
            if (verbose_ >= 2) {
                cout << BOLDRED << "Warning: There should be " << dim_len
                        << " Ys! Coordinate variables not used!" << RESET << endl;
            }
            with_coordinates = false;
        }
    }

    // Construct Station object and insert them into Stations
    stations.clear();
    for (size_t i = 0; i < dim_len; i++) {
        Station station;

        if (with_names) {
            station.setName(names.at(i));
        }

        if (with_coordinates) {
            station.setX(xs.at(i));
            station.setY(ys.at(i));
        }

        if (!stations.insert(station).second) {
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
AnEnIO::readTimes(Times& times) {

    if (verbose_ >= 3) {
        cout << "Reading variable (Times) ..." << endl;
    }

    string var_name = "Times";
    handleError(checkVariable(var_name, false));

    vector<double> vec;
    read_vector_(var_name, vec);

    // copy the vector to the set
    copy(vec.begin(), vec.end(), inserter(times, times.end()));

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

    handleError(checkDim(dim_name));

    NcFile nc(file_path_, NcFile::FileMode::read);
    NcDim dim = nc.getDim(dim_name);
    len = dim.getSize();
    nc.close();
    return (SUCCESS);
}

void
AnEnIO::handleError(const errorType & indicator) const {
    if (indicator == SUCCESS ||
            indicator == OPTIONAL_VARIABLE_MISSING) {
        return;
    } else {
        throw runtime_error("Error code " + to_string(indicator));
    }
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

vector<std::string>
AnEnIO::getRequiredDimensions() const {
    return required_dimensions_;
}

int
AnEnIO::getVerbose() const {
    return verbose_;
}

void
AnEnIO::setFilePath(string file_path_) {
    this->file_path_ = file_path_;
    handleError(checkFile());
}

void
AnEnIO::setFileType(string fileType_) {
    this->file_type_ = fileType_;
    handleError(checkFileType());
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
AnEnIO::setRequiredDimensions(std::vector<std::string> required_dimensions) {
    this->required_dimensions_ = required_dimensions;
}

void
AnEnIO::setVerbose(int verbose_) {
    this->verbose_ = verbose_;
}

errorType
AnEnIO::read_string_vector_(string var_name, vector<string> & vector) const {

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

            vector.resize(num_strs);
            for (size_t i = 0; i < num_strs; i++) {
                string str(p_vals + i*num_chars, p_vals + (i + 1) * num_chars);
                vector.at(i) = str;
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

void
AnEnIO::dumpVariable(string var_name, size_t start, size_t count) const {

    handleError(checkVariable(var_name, false));

    if (verbose_ >= 3)
        cout << "Dumping variable ("
            << var_name << ") values ..." << endl
            << "---------------------------------------------" << endl;

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
                        << var_name << ") type not supported!" << RESET << endl;
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