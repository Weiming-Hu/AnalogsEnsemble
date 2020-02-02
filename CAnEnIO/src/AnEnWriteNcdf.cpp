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

void
AnEnWriteNcdf::append_(const NcFile & nc, const Analogs & analogs) const {

    if (verbose_ >= Verbose::Detail) cout << "Appending analogs ..." << endl;

    // Create analog dimensions
    const auto & shape = analogs.shape();
    NcDim dim_stations = getDim_(nc, DIM_STATIONS, shape[0]),
            dim_times = getDim_(nc, DIM_TIMES, shape[1]),
            dim_flts = getDim_(nc, DIM_FLTS, shape[2]),
            dim_members = getDim_(nc, DIM_MEMBERS, shape[3]),
            dim_cols = getDim_(nc, DIM_COLS, shape[4]);

    // Create the analog variable
    NcVar var_analogs = nc.addVar(VAR_ANALOGS, NC_DOUBLE,{
        dim_cols, dim_members, dim_flts, dim_times, dim_stations
    });

    // Put data
    var_analogs.putVar(analogs.data());
    return;
}

void
AnEnWriteNcdf::append_(const NcFile & nc,
        const Times & flts, bool unlimited) const {
    
    if (verbose_ >= Verbose::Detail) cout << "Appending lead times ..." << endl;
    
    if (varExists(nc, VAR_FLTS)) {
        ostringstream msg;
        msg << VAR_FLTS << " exists";
        throw runtime_error(msg.str());
    }
    
    size_t len = (unlimited ? 0 : flts.size());
    NcDim dim_flts = getDim_(nc, DIM_FLTS, len);
    NcVar var = nc.addVar(VAR_FLTS, NcType::nc_DOUBLE, dim_flts);
    
}