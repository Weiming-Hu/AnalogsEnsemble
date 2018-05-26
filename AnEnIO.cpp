/* 
 * File:   AnEnIO.cpp
 * Author: Weiming Hu (weiming@psu.edu)
 * 
 * Created on May 4, 2018, 11:16 AM
 */

#include "AnEnIO.h"

#include <boost/filesystem.hpp>
#include <exception>

using namespace netCDF;
using namespace std;

AnEnIO::AnEnIO() {
}

AnEnIO::~AnEnIO() {
}

int
AnEnIO::checkForecasts(std::string file) {
    NcFile nc;

    handleError(0);

    handleError(4);

    return (0);
}

//int
//AnEnIO::readParameters(std::string file, Parameters & parameters) const {
//    // We need to check if the file has the correct format and variable names
//
//    NcFile nc;
//
//    int ret = open_file_(file, nc);
//    if (ret != SUCCESS) {
//        return (ret);
//    }
//
//    // Assume we have the variable Data
//    NcVar var_data = nc.getVar("Parameters");
//
//    if (var_data.isNull()) {
//        return (VAR_NAME_NOT_FOUND);
//    }
//
//    // Get the dimension of the variable
//    // to define the length of the pointer
//    //
//    vector<NcDim> var_dims = var_data.getDims();
//
//    size_t var_len = 1;
//
//    for (auto const & dim : var_dims) {
//        var_len *= dim.getSize();
//    }
//
//    NcType type = var_data.getType();
//
//    if (type == NcType::ncType::nc_DOUBLE) {
//
//        double *p_data = nullptr;
//
//        try {
//            p_data = new double[var_len];
//        } catch (bad_alloc &e) {
//            nc.close();
//            return ( INSUFFICIENT_MEMORY);
//        } catch (...) {
//            nc.close();
//            return ( UNKNOWN_ERROR);
//        }
//
//        var_data.getVar(p_data);
//
//
//
//    } else {
//        return ( WRONG_TYPE);
//    }
//
//    return ( SUCCESS);
//}
//
//int
//AnEnIO::open_file_(std::string file, NcFile & nc) const {
//
//    nc.open(file, NcFile::FileMode::read);
//
//    if (nc.isNull()) {
//        return ( FILE_NOT_FOUND);
//    }
//
//    return ( SUCCESS);
//}

void
AnEnIO::handleError(int indicator) {
    if (indicator == SUCCESS) {
        return;
    } else {
        throw runtime_error("Error occurred!");
    }
}