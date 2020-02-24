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

    void checkExists(const std::string & file_path);
    bool checkExists(const std::string& file_path, bool overwrite, bool append);
    void checkExtension(const std::string&, const std::string & ext = ".nc");
    void checkIndex(size_t start, size_t count, size_t len);
    void checkDims(const netCDF::NcGroup & nc,
            const std::vector<std::string> & names);
    void checkVars(const netCDF::NcGroup & nc,
            const std::vector<std::string> & names);
    void checkVarShape(const netCDF::NcGroup & nc,
            const std::string & var_name,
            const std::vector<std::string> & dim_names);
    bool dimExists(const netCDF::NcGroup & nc, const std::string & name);
    bool varExists(const netCDF::NcGroup & nc, const std::string & name);

    void readStringVector(const netCDF::NcGroup & nc,
            std::string var_name, std::vector<std::string> & results,
            size_t start = 0, size_t count = 0,
            const std::string & name_char = "num_chars");

    netCDF::NcDim getDimension(netCDF::NcGroup &,
            const std::string &, bool unlimited = false, size_t len = 0);
    void writeStringVector(netCDF::NcGroup &, const std::string &,
            const std::string &, const std::vector<std::string> &,
            bool unlimited = false);
    void writeArray4D(netCDF::NcGroup &, const Array4D &, const std::string &,
            const std::array<std::string, 4> &,
            const std::array<bool, 4> & unlimited = {false, false, false, false});

    void purge(std::string & str);
    void purge(std::vector<std::string> & strs);

    /************************************************************************
     *                         Template Functions                           *
     ************************************************************************/
    
    template <typename nctype, typename valuetype>
    void writeAttribute(nctype & nc, const std::string &,
            const valuetype &, netCDF::NcType, bool overwrite = false);
    
    template <typename nctype>
    void writeStringAttribute(nctype & nc, const std::string &,
            const std::string &, bool overwrite = false);
    
    template <typename T>
    void writeVector(netCDF::NcGroup &, const std::string &,
            const std::string &, const std::vector<T> &, netCDF::NcType,
            bool unlimited = false);
    
    template <typename T>
    void readVector(const netCDF::NcGroup & nc, std::string var_name,
            std::vector<T> & results,
            std::vector<size_t> start = {},
            std::vector<size_t> count = {});
};

// Template functions are defined in the following file
#include "Ncdf.tpp"

#endif /* NCDF_H */