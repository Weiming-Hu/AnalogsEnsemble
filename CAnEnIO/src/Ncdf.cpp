/* 
 * File:   ReadNcdf.cpp
 * Author: Weiming Hu (weiming@psu.edu)
 * 
 * Created on December 30, 2019, 7:55 PM
 */

#include "boost/filesystem.hpp"

#include "Ncdf.h"
#include "Config.h"

namespace filesys = boost::filesystem;
using namespace netCDF;
using namespace std;

void
Ncdf::checkExists(const string & file_path) {

    // Check whether the file exists
    bool file_exists = filesys::exists(file_path);

    if (!file_exists) {
        ostringstream msg;
        msg << "File to read is not found: " << file_path;
        throw invalid_argument(msg.str());
    }

    return;
}

bool
Ncdf::checkExists(const string & file_path, bool overwrite, bool append) {

    if (filesys::exists(file_path)) {
        if (overwrite) {
            filesys::remove(file_path);
        } else {

            if (append) {

                // I need to make sure the file format is nc4 for appending
                NcFile nc(file_path, NcFile::FileMode::read);
                
                int file_id = nc.getId();
                int file_format;

                nc_inq_format(file_id, &file_format);
                
                if (file_format != NC_FORMAT_NETCDF4) {
                    throw runtime_error("Only supporting NetCDF version 4 files for appending");
                }

                /*
                 * Return true if the file is found.
                 * 
                 * No errors because, although the file is found, variables are
                 * meant to be appended to the file.
                 * 
                 * NetCDF files support writing extra variables into an
                 * existing file by appending.
                 */
                return true;

            } else {
                ostringstream msg;
                msg << "File to write (" << file_path << ") exists. "
                        << "Use overwrite = true to remove the existing file or "
                        << "append = true to write to the existing file.";
                throw runtime_error(msg.str());
            }
        }
    }

    // Return false if the file is not found
    return false;
}

void
Ncdf::checkExtension(const string & file_path, const string & ext) {

    filesys::path boost_path(file_path);

    // Succeed if the file path has .nc as its extension
    if (boost_path.has_extension()) {
        if (boost_path.extension().string() == ext) return;
    }

    // Fail if the file path does not have a .nc extension
    ostringstream msg;
    msg << "Unknown extension " << file_path;
    throw invalid_argument(msg.str());
}

void
Ncdf::checkIndex(size_t start, size_t count, size_t len) {

    if (start + count - 1 >= len) {
        ostringstream msg;
        msg << "Indices out of bound. start:" << start << " count:"
                << count << " len:" << len;
        throw runtime_error(msg.str());
    }

    return;
}

void
Ncdf::checkDims(const NcGroup & nc,
        const vector<string> & names) {

    for (const auto & name : names) {
        if (!name.empty() && !dimExists(nc, name)) {
            ostringstream msg;
            msg << "Dimension (" + name + ") is missing";
            throw invalid_argument(msg.str());
        }
    }

    return;
}

void
Ncdf::checkVars(const NcGroup & nc,
        const vector<string> & names) {

    for (const auto & name : names) {
        if (!name.empty() && !varExists(nc, name)) {
            ostringstream msg;
            msg << "Variable (" << name << ") is missing";
            throw invalid_argument(msg.str());
        }
    }

    return;
}

void
Ncdf::checkVarShape(const NcGroup & nc,
        const string & var_name, const vector<string> & dim_names) {

    if (var_name.empty()) throw runtime_error("Ncdf::checkVarShape -> Empty variable name is not allowed");

    const auto & var = nc.getVar(var_name);
    auto dims = var.getDims();

    if (dims.size() != dim_names.size()) {
        ostringstream msg;
        msg << "Variable " << var_name << " has " << dims.size() <<
                " dimensions while " << dim_names.size() << " are expected";
        throw runtime_error(msg.str());
    }

    reverse(dims.begin(), dims.end());

    for (size_t i = 0; i < dims.size(); ++i) {
        if (dim_names[i] != dims[i].getName()) {
            ostringstream msg;
            msg << "Variable " << var_name << " dimension #" << i + 1 << " ("
                    << dims[i].getName() << ") should be " << dim_names[i];
            throw runtime_error(msg.str());
        }
    }

    return;
}

bool
Ncdf::dimExists(const NcGroup & nc, const string & name) {
    if (name.empty()) throw runtime_error("Empty dimension name is not allowed");
    return (!nc.getDim(name).isNull());
}

bool
Ncdf::varExists(const NcGroup & nc, const string & name) {

    if (name.empty()) throw runtime_error("Ncdf::varExists -> Empty variable name is not allowed");

    return (!nc.getVar(name).isNull());
}

void
Ncdf::readStringVector(
        const NcGroup & nc, string var_name,
        vector<string> & results, size_t start, size_t count,
        const string & name_char) {

    if (var_name.empty()) throw runtime_error("Ncdf::readStringVector -> Empty variable name is not allowed");
    
    // Check whether we are reading partial or the entire variable
    bool entire = (start == 0 || count == 0);

    NcVar var = nc.getVar(var_name);
    auto var_dims = var.getDims();
    char *p_vals = nullptr;

    // Check the dimensions
    if (var.getDims().size() != 2) {
        ostringstream msg;
        msg << "Variable (" << var_name << ") is not 2-dimensional";
        throw runtime_error(msg.str());
    }

    // Check the variable type
    if (var.getType() != NcType::nc_CHAR) {
        ostringstream msg;
        msg << "Variable (" << var_name << ") is not nc_CHAR type!";
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
        msg << "Insufficient memory reading " << var_name;
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

NcDim
Ncdf::getDimension(NcGroup & nc, const string & name, bool unlimited, size_t len) {
    
    if (name.empty()) throw runtime_error("Empty dimension name is not allowed");

    NcDim dim = nc.getDim(name);
    if (dim.isNull()) {

        // The dimension name does not exist. Add this parameter
        if (unlimited) return nc.addDim(name);
        else return nc.addDim(name, len);

    } else {
        // The dimension name already exists
        bool check = (dim.isUnlimited() == unlimited);
        if (!dim.isUnlimited()) check = check && (dim.getSize() == len);

        if (check) {
            // The exact dimension exists. Do not add this parameter
            return dim;
        } else {
            ostringstream msg;
            msg << "Dimension " << name << " already exists with different settings";
            throw runtime_error(msg.str());
        }
    }
}

void
Ncdf::writeStringVector(NcGroup & nc, const std::string & var_name,
        const std::string & dim_name, const std::vector<std::string> & values,
        bool unlimited) {

    using namespace netCDF;
    using namespace std;
    
    if (var_name.empty()) throw runtime_error("Ncdf::writeStringVector -> Empty variable name is not allowed");

    // Check whether the variable exists
    if (!nc.getVar(var_name).isNull()) {
        ostringstream msg;
        msg << "Variable " << var_name << " exists";
        throw runtime_error(msg.str());
    }

    // Get the dimension object
    NcDim dim = getDimension(nc, dim_name, unlimited, values.size());

    NcVar var = nc.addVar(var_name, NcType::nc_STRING, dim);

    for (size_t string_i = 0; string_i < values.size(); ++string_i) {
        /* For string vectors, there are no methods to add them directly.
         * But there is a method to add them one by one. So I'm using the
         * following method.
         */
        var.putVar({string_i}, values[string_i]);
    }

    return;
}

void
Ncdf::writeArray4D(NcGroup & nc, const Array4D & arr, const string & var_name,
        const array<string, 4> & dim_names, const array<bool, 4 > & unlimited) {

    if (var_name.empty()) throw runtime_error("Ncdf::writeArray4D -> Empty variable name is not allowed");

    // Check whether array is column major
    if (arr.num_elements() >= 2) {
        if (arr.getValue(1, 0, 0, 0) != arr.getValuesPtr()[1]) {
            throw runtime_error("The input array is not column major");
        }
    }

    // Check whether the variable name already exists
    NcVar var = nc.getVar(var_name);
    if (!var.isNull()) {
        ostringstream msg;
        msg << "Variable " << var_name << " exists";
        throw runtime_error(msg.str());
    }

    NcDim dim0, dim1, dim2, dim3;
    try {
        dim0 = getDimension(nc, dim_names[0], unlimited[0], arr.shape()[0]);
        dim1 = getDimension(nc, dim_names[1], unlimited[1], arr.shape()[1]);
        dim2 = getDimension(nc, dim_names[2], unlimited[2], arr.shape()[2]);
        dim3 = getDimension(nc, dim_names[3], unlimited[3], arr.shape()[3]);
    } catch (exception & e) {
        ostringstream msg;
        msg << "writeArray4D(var_name = " << var_name << ") -> " << e.what();
        throw runtime_error(msg.str());
    }

    /*
     * Create an NetCDF variable. Note the reversed dimension order so that we
     * can copy values from the column-major ordered pointer directly. The first
     * dimension in the initializer list is the slowest varying dimension
     */
    var = nc.addVar(var_name, NC_DOUBLE,{dim3, dim2, dim1, dim0});

    var.putVar(arr.getValuesPtr());
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
