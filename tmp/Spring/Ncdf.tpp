/* 
 * File:   Ncdf.tpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on December 30, 2019, 11:10 AM
 */

#include "colorTexts.h"

template<typename T>
void
Ncdf::readVector(const netCDF::NcFile & nc, std::string var_name,
        std::vector<T> & results,
        std::vector<size_t> start, std::vector<size_t> count) {

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
        msg << BOLDRED << "Insufficient memory reading " <<
                var_name << "!" << RESET;
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