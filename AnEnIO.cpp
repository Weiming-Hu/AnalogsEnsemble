/* 
 * File:   AnEnIO.cpp
 * Author: Weiming Hu (weiming@psu.edu)
 * 
 * Created on May 4, 2018, 11:16 AM
 */

#include "AnEnIO.h"

using namespace netCDF;
using namespace std;
using namespace AnEnIO::errorType;

AnEnIO::AnEnIO() {
}

AnEnIO::~AnEnIO() {
}

int
AnEnIO::readObservations(std::string file, Observations& observations) const {
    return 0;
}


// Return Value
// -2: 
// -1: 
// 0: otherwise

int
AnEnIO::readParameters(std::string fname, Parameters & parameters ) const {
 // We need to check if the file has the correct format and variable names
    
    NcFile nc;
    
    int ret = open_file_( fname, nc );
    if( ret != SUCCESS ) {
        return( ret ); 
    }
    
    // Assume we have the variable Data
    NcVar var_data = nc.getVar('Parameters');
    
    if (var_data.isNull()) {
        return (VAR_NAME_NOT_FOUND);
    }
   
    // Get the dimension of the variable
    // to define the length of the pointer
    //
    vector<NcDim> var_dims = var_data.getDims();
    
    size_t var_len = 1;
    
    for (auto const & dim : var_dims) {
        var_len *= dim.getSize();
    }
    
    NcType type = var_data.getType();
    
    if (type == NcType::ncType::nc_DOUBLE) {
        
        double *p_data = nullptr;
        
        try {
            p_data = new double[var_len];
        } catch (bad_alloc &e) {
            nc.close();
            return ( INSUFFICIENT_MEMORY );
        } catch (...) {
            nc.close();
            return ( UNKNOWN_ERROR );
        }
        
        var_data.getVar(p_data);
        
        
        
    } else {
        return ( WRONG_TYPE );
    }
       
}

// Return Value
// -1: File not found
// 0: otherwise
int
AnEnIO::open_file_( std::string fname, NcFile & nc ) const {
     
    nc.open(file, NcFile::FileMode::read);
    
    if (nc.isNull()) {
        return ( FILE_NOT_FOUND );
    }
    return ( SUCCESS );
}