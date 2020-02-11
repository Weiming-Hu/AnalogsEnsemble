/* 
 * File:   ReadNcdf.h
 * Author: Weiming Hu (weiming@psu.edu)
 *
 * Created on December 30, 2019, 7:55 PM
 */

#ifndef NCDF_H
#define NCDF_H

#include <array>
#include <netcdf>
#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>

#include "Array4D.h"

/**
 * Namespace ReadNcdf provides a collection of helper functions to examine and 
 * read NetCDF files. This is not made into a class because these functions
 * are considered "free" functions that are just wrappers over netCDF data
 * types and functions.
 */
namespace Ncdf {
    
    enum class Mode {
        Read, Write
    };

    void checkPath(const std::string & file_path, Mode mode);
    void checkIndex(size_t start, size_t count, size_t len);
    void checkDims(const netCDF::NcFile & nc,
            const std::vector<std::string> & names);
    void checkVars(const netCDF::NcFile & nc,
            const std::vector<std::string> & names);
    void checkVarShape(const netCDF::NcFile & nc,
            const std::string & var_name,
            const std::vector<std::string> & dim_names);
    bool dimExists(const netCDF::NcFile & nc, const std::string & name);
    bool varExists(const netCDF::NcFile & nc, const std::string & name);

    void readStringVector(const netCDF::NcFile & nc,
            std::string var_name, std::vector<std::string> & results,
            size_t start = 0, size_t count = 0,
            const std::string & name_char = "num_chars");

    netCDF::NcDim getDimension(netCDF::NcFile &,
            const std::string &, bool unlimited = false, size_t len = 0);
    void writeStringVector(netCDF::NcFile &, const std::string &,
            const std::string &, const std::vector<std::string> &,
            bool unlimited = false);
    void writeArray4D(netCDF::NcFile &, const Array4D &, const std::string &,
            const std::array<std::string, 4> &,
            const std::array<bool, 4> & unlimited = {false, false, false, false});

    void purge(std::string & str);
    void purge(std::vector<std::string> & strs);

    /************************************************************************
     *                         Template Functions                           *
     ************************************************************************/
    
    template <typename nctype, typename valuetype>
    void writeAttributes(nctype & nc, const std::string &,
            const valuetype &, netCDF::NcType, bool overwrite = false);
    
    template <typename nctype>
    void writeStringAttributes(nctype & nc, const std::string &,
            const std::string &, bool overwrite = false);
    
    template <typename T>
    void writeVector(netCDF::NcFile &, const std::string &,
            const std::string &, const std::vector<T> &, netCDF::NcType,
            bool unlimited = false);
    
    template <typename T>
    void readVector(const netCDF::NcFile & nc, std::string var_name,
            std::vector<T> & results,
            std::vector<size_t> start = {},
            std::vector<size_t> count = {});
};

// Template functions are defined in the following file
#include "Ncdf.tpp"

#endif /* NCDF_H */