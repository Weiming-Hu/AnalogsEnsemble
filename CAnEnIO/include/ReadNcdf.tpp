/* 
 * File:   ReadNcdf.tpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on December 30, 2019, 11:10 AM
 */

template <std::size_t SIZE>
void
ReadNcdf::checkDims(const netCDF::NcFile & nc,
        const std::array<std::string, SIZE> & names, bool verbose) {

    using namespace std;

    for (const auto & name : names) {
        if (!dimExists(nc, name)) {
            string msg = "Dimension (" + name + ") is missing!";
            if (verbose) cerr << BOLDRED << msg << RESET << endl;
            throw invalid_argument(msg);
        }
    }

    return;
}

template <std::size_t SIZE>
void
ReadNcdf::checkVars(const netCDF::NcFile & nc,
        const std::array<std::string, SIZE> & names, bool verbose) {

    using namespace std;

    for (const auto & name : names) {
        if (!varExists(nc, name)) {
            string msg = "Variable (" + name + ") is missing!";
            if (verbose) cerr << BOLDRED << msg << RESET << endl;
            throw invalid_argument(msg);
        }
    }

    return;
}

template<typename T>
void
ReadNcdf::readVector(const netCDF::NcFile & nc, std::string var_name,
        std::vector<T> & results, bool verbose,
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
        string msg = "Insufficient memory reading " + var_name + "!";
        if (verbose) cerr << BOLDRED << msg << RESET << endl;
        throw runtime_error(msg);
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