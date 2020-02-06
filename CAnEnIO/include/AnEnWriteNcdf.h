/* 
 * File:   AnEnWriteNcdf.h
 * Author: Weiming Hu (weiming@psu.edu)
 *
 * Created on January 1, 2020, 6:33 PM
 */

#ifndef ANENWRITENCDF_H
#define ANENWRITENCDF_H

#include "Ncdf.h"
#include "Analogs.h"
#include "Stations.h"
#include "Times.h"
#include "Parameters.h"
#include "Config.h"

class AnEnWriteNcdf {
public:

    // TODO: when writing to a NetCDF file Make sure that you
    // - Create a NetCDF-4 format
    // - Using 64-bit file format
    enum class FileType {
        Forecasts, Observations, Similarity,
        Analogs, StandardDeviation
    };

    // A higher level will contain all messages from the lower levels. For
    // example, The progress level will contain errors and warnings. Errors,
    // if any, must be printed.
    //

    AnEnWriteNcdf();
    AnEnWriteNcdf(Verbose verbose);
    AnEnWriteNcdf(const AnEnWriteNcdf& orig);
    virtual ~AnEnWriteNcdf();

protected:
    Verbose verbose_;

    netCDF::NcDim getDim_(const netCDF::NcFile & nc,
            std::string name, size_t len = 0) const;

    void append_(const netCDF::NcFile & nc, const Analogs & analogs) const;

};

#endif /* ANENWRITENCDF_H */

