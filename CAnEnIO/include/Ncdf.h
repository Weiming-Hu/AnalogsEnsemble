/* 
 * File:   ReadNcdf.h
 * Author: Weiming Hu (weiming@psu.edu)
 *
 * Created on December 30, 2019, 7:55 PM
 */

#ifndef NCDF_H
#define NCDF_H

#include <netcdf>
#include <vector>
#include <array>
#include <string>
#include <algorithm>
#include <sstream>

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

    void readStringVector(const netCDF::NcFile & nc,
            std::string var_name, std::vector<std::string> & results,
            size_t start = 0, size_t count = 0,
            const std::string & name_char = "num_chars");

    void purge(std::string & str);
    void purge(std::vector<std::string> & strs);

    /************************************************************************
     *                           Inline Functions                           *
     ************************************************************************/

    inline bool dimExists(const netCDF::NcFile & nc, const std::string & name) {
        return (!nc.getDim(name).isNull());
    }

    inline bool varExists(const netCDF::NcFile & nc, const std::string & name) {
        return (!nc.getVar(name).isNull());
    }

    /************************************************************************
     *                         Template Functions                           *
     ************************************************************************/

    template <std::size_t SIZE>
    void checkDims(const netCDF::NcFile & nc,
            const std::array<std::string, SIZE> & names);

    template <std::size_t SIZE>
    void checkVars(const netCDF::NcFile & nc,
            const std::array<std::string, SIZE> & names);

    template <typename T>
    void readVector(const netCDF::NcFile & nc, std::string var_name,
            std::vector<T> & results,
            std::vector<size_t> start = {},
            std::vector<size_t> count = {});
};

// Template functions are defined in the following file
#include "Ncdf.tpp"

#endif /* NCDF_H */