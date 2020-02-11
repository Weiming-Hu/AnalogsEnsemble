/* 
 * File:   Ncdf.tpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on December 30, 2019, 11:10 AM
 */

#include <algorithm>

#include "Ncdf.h"

template <typename nctype, typename valuetype>
void
Ncdf::writeAttributes(nctype & nc, const std::string & att_name,
        const valuetype & value, netCDF::NcType type, bool overwrite) {

    /*
     * If you don't want to overwrite, I need to check whether the attribute
     * already exists in the object.
     */
    if (!overwrite) {
        auto att = nc.getAtt(att_name);
        if (!att.isNull()) {
            std::ostringstream msg;
            msg << "Attribute " << att_name << " exists. Use overwrite = true";
            throw std::runtime_error(msg.str());
        }
    }
    nc.putAtt(att_name, type, value);
    return;
}

template <typename nctype>
void
Ncdf::writeStringAttributes(nctype & nc, const std::string & att_name,
        const std::string & value, bool overwrite) {

    /*
     * If you don't want to overwrite, I need to check whether the attribute
     * already exists in the object.
     */
    if (!overwrite) {
        auto att = nc.getAtt(att_name);
        if (!att.isNull()) {
            std::ostringstream msg;
            msg << "Attribute " << att_name << " exists. Use overwrite = true";
            throw std::runtime_error(msg.str());
        }
    }

    nc.putAtt(att_name, value);
    return;
}

template <typename T>
void
Ncdf::writeVector(netCDF::NcFile & nc, const std::string & var_name,
        const std::string & dim_name, const std::vector<T> & values,
        netCDF::NcType type, bool unlimited) {

    using namespace netCDF;
    using namespace std;

    // Check whether the variable exists
    if (!nc.getVar(var_name).isNull()) {
        ostringstream msg;
        msg << "Variable " << var_name << " exists";
        throw runtime_error(msg.str());
    }

    // Get the dimension object
    NcDim dim = getDimension(nc, dim_name, unlimited, values.size());

    NcVar var = nc.addVar(var_name, type, dim);
    var.putVar(values.data());
    return;
}

template <typename T>
void
Ncdf::readVector(const netCDF::NcFile & nc, std::string var_name,
        std::vector<T> & results, std::vector<size_t> start, std::vector<size_t> count) {

    using namespace netCDF;
    using namespace std;

    // Check whether we are reading partial or the entire variable
    bool entire = (start.size() == 0 || count.size() == 0);

    NcVar var = nc.getVar(var_name);
    const auto & var_dims = var.getDims();
    T *p_vals = nullptr;

    // Determine the total number of values to read
    size_t total = 1;
    if (entire) for (const auto & dim : var_dims) total *= dim.getSize();
    else for (const auto & i : count) total *= i;

    // Allocate memory for the data
    try {
        results.resize(total);
    } catch (bad_alloc & e) {
        ostringstream msg;
        msg << "Insufficient memory reading " << var_name;
        throw runtime_error(msg.str());
    }

    p_vals = results.data();

    if (entire) {
        var.getVar(p_vals);
    } else {
        // Reverse the order because of the storage style of NetCDF 
        // follows the Fortran style which is column-wise.
        //
        reverse(start.begin(), start.end());
        reverse(count.begin(), count.end());
        var.getVar(start, count, p_vals);
    }

    // Do not delete the pointer manually because it is managed by the vector
    // object and it will be deconstructed when it goes out of scope.
    //
    // delete [] p_vals;

    return;
};