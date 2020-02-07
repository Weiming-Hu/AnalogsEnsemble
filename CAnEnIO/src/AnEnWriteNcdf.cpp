/* 
 * File:   AnEnWriteNcdf.cpp
 * Author: Weiming Hu (weiming@psu.edu)
 * 
 * Created on January 1, 2020, 6:33 PM
 */

#include "AnEnWriteNcdf.h"
#include "AnEnNames.h"

#include <stdexcept>

using namespace std;
using namespace Ncdf;
using namespace netCDF;
using namespace AnEnNames;

AnEnWriteNcdf::AnEnWriteNcdf() {
    Config config;
    verbose_ = config.verbose;
}

AnEnWriteNcdf::AnEnWriteNcdf(Verbose verbose) :
verbose_(verbose) {
}

AnEnWriteNcdf::AnEnWriteNcdf(const AnEnWriteNcdf& orig) {
    if (this != &orig) verbose_ = orig.verbose_;
}

AnEnWriteNcdf::~AnEnWriteNcdf() {
}

NcDim
AnEnWriteNcdf::getDim_(const NcFile & nc, string name, size_t len) const {
    
    NcDim dim = nc.getDim(name);
    
    if (dim.isNull()) {
        // If it is not found in the NetCDF
        
        if (len == 0) dim = nc.addDim(name); // Unlimited dimension
        else dim = nc.addDim(name, len);     // Fixed length dimension
        
    } else {
        // If it is found in the NetCDF
        if (len == 0) {
            if (!dim.isUnlimited()) {
                ostringstream msg;
                msg << "Dimension (" << name << ") is found with" <<
                        " fixed length. But it should be unlimited.";
                throw runtime_error(msg.str());
            }
        } else {
            if (len != dim.getSize()) {
                ostringstream msg;
                msg << "Length of " << name << " (" << dim.getSize()
                        << ") should be " << len;
                throw runtime_error(msg.str());
            }
        }
    }
    
    return (dim);
}
