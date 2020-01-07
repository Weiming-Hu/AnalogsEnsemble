/* 
 * File:   ReadNcdf.cpp
 * Author: Weiming Hu (weiming@psu.edu)
 * 
 * Created on December 30, 2019, 7:55 PM
 */

#include "Ncdf.h"
#include "boost/filesystem.hpp"

#include <stdexcept>

namespace filesys = boost::filesystem;
using namespace netCDF;
using namespace std;

void
Ncdf::checkPath(const string & file_path, Mode mode) {

    // Check whether the file exists
    bool file_exists = filesys::exists(file_path);
    
    if (mode == Mode::Read && !file_exists) {
        ostringstream msg;
        msg << BOLDRED << "File not found: " << file_path << RESET;
        throw invalid_argument(msg.str());
    }
    
    if (mode == Mode::Write && file_exists) {
        ostringstream msg;
        msg << BOLDRED << "File exists: " << file_path << RESET;
        throw invalid_argument(msg.str());
    }

    filesys::path boost_path(file_path);

    // Succeed if the file path has .nc as its extension
    if (boost_path.has_extension()) {
        if (boost_path.extension().string() == ".nc") return;
    }

    // Fail if the file path does not have a .nc extension
    ostringstream msg;
    msg << BOLDRED << "Unknown extension " << file_path << RESET;
    throw invalid_argument(msg.str());
}

void
Ncdf::checkIndex(size_t start, size_t count, size_t len) {

    if (start + count - 1 >= len) {
        ostringstream msg;
        msg << BOLDRED << "Indices out of bound. start:" << start <<
                " count:" << count << " len:" << len << RESET;
        throw runtime_error(msg.str());
    }

    return;
}

void
Ncdf::readStringVector(
        const NcFile & nc, string var_name,
        vector<string> & results, size_t start, size_t count,
        const string & name_char) {

    // Check whether we are reading partial or the entire variable
    bool entire = (start == 0 || count == 0);

    NcVar var = nc.getVar(var_name);
    auto var_dims = var.getDims();
    char *p_vals = nullptr;

    // Check the dimensions
    if (var.getDims().size() != 2) {
        ostringstream msg;
        msg << BOLDRED << "Variable (" << var_name <<
                ") is not 2-dimensional!" << RESET;
        throw runtime_error(msg.str());
    }

    // Check the variable type
    if (var.getType() != NcType::nc_CHAR) {
        ostringstream msg;
        msg << BOLDRED << "Variable (" << var_name <<
                ") is not nc_CHAR type!" << RESET;
        throw runtime_error(msg.str());
    }

    // Determine the length of the strings to read
    size_t len, num_strs = var_dims[0].getSize(),
            num_chars = var_dims[1].getSize();

    if (nc.getDim(name_char).getSize() != num_chars) {
        throw runtime_error("Can not read strings as vectors!");
    }

    if (entire) len = num_chars * num_strs;
    else len = num_chars * count;

    // Allocate memory for reading
    try {
        p_vals = new char[len];
    } catch (bad_alloc & e) {
        ostringstream msg;
        msg << BOLDRED << "Insufficient memory reading " <<
                var_name << "!" << RESET;
        throw runtime_error(msg.str());
    }

    // Convert the pointer to a vector of strings
    if (entire) {
        var.getVar(p_vals);
        results.resize(num_strs);
    } else {
        vector<size_t> vec_start{start, 0}, vec_count{count, num_chars};
        var.getVar(vec_start, vec_count, p_vals);
        results.resize(count);
    }

    for (size_t i = 0; i < results.size(); i++) {
        string str(p_vals + i*num_chars, p_vals + (i + 1) * num_chars);
        purge(str);
        results.at(i) = str;
    }

    // Deallocation
    delete [] p_vals;

    return;
}

void
Ncdf::purge(string & str) {
    str.erase(remove_if(str.begin(), str.end(), [](const unsigned char & c) {
        bool remove = !(isalpha(c) || isdigit(c) || ispunct(c) || isspace(c));
        return (remove);
    }), str.end());
}

void
Ncdf::purge(vector<string> & strs) {
    for (auto & str : strs) purge(str);
}

void
Ncdf::checkDims(const netCDF::NcFile & nc,
        const vector<string> & names) {

    for (const auto & name : names) {
        if (!name.empty() && !dimExists(nc, name)) {
            ostringstream msg;
            msg << BOLDRED << "Dimension (" + name + ") is missing!" << RESET;
            throw invalid_argument(msg.str());
        }
    }

    return;
}

void
Ncdf::checkVars(const netCDF::NcFile & nc,
        const vector<string> & names) {

    for (const auto & name : names) {
        if (!name.empty() && !varExists(nc, name)) {
            ostringstream msg;
            msg << BOLDRED << "Variable (" << name << ") is missing!" << RESET;
            throw invalid_argument(msg.str());
        }
    }

    return;
}

void
Ncdf::checkVarShape(const NcFile & nc,
        const string & var_name, const vector<string> & dim_names) {

    const auto & var = nc.getVar(var_name);
    auto dims = var.getDims();
    
    if (dims.size() != dim_names.size()) {
        ostringstream msg;
        msg << BOLDRED << "Variable " << var_name << " has " << dims.size() <<
                " dimensions while " << dim_names.size() << " are expected" <<
                RESET << endl;
        throw runtime_error(msg.str());
    }
    
    reverse(dims.begin(), dims.end());

    for (size_t i = 0; i < dims.size(); ++i) {
        if (dim_names[i] != dims[i].getName()) {
            ostringstream msg;
            msg << BOLDRED << "Variable " << var_name << " dimension #" <<
                    i + 1 << " (" << dims[i].getName() << ") should be " <<
                    dim_names[i] << RESET;
            throw runtime_error(msg.str());
        }
    }

    return;
}

bool
Ncdf::dimExists(const NcFile & nc, const string & name) {
    return (!nc.getDim(name).isNull());
}

bool
Ncdf::varExists(const NcFile & nc, const string & name) {
    return (!nc.getVar(name).isNull());
}
