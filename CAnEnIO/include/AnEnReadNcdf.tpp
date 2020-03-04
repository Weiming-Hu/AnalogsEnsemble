/* 
 * File:   AnEnReadNcdf.tpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on January 1, 2020, 12:53 PM
 */

template <typename T>
void
AnEnReadNcdf::read(const netCDF::NcGroup & nc,
        T * p_vals, const std::string & var_name,
        std::vector<size_t> start, std::vector<size_t> count) const {
    
    using namespace std;
    using namespace netCDF;

    if (verbose_ >= Verbose::Detail) {
        cout << "Reading variable " << var_name << " ..." << endl;
    }
    
    NcVar var = nc.getVar(var_name);
    
    if (start.size() == 0 || count.size() == 0) {
        // If read the entire data variable
        var.getVar(p_vals);
    } else {
        // If reading the partial data variable
        
        // *********************** Optimization ******************************
        //
        // Reverse of the indices is done for performance optimization.
        // NetCDF uses column-wise order, and therefore, boost arrays are
        // intentionally restructured using column-wise order during
        // instantiation. To be consistent, the indices of dimensions
        // should also be reversed before feeding them to NetCDF functions.
        //
        // *******************************************************************
        // 
        reverse(start.begin(), start.end());
        reverse(count.begin(), count.end());
        var.getVar(start, count, p_vals);
    }
    
    return;
}