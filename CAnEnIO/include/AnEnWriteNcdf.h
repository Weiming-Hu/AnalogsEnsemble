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
#include "AnEnDefaults.h"

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
    AnEnWriteNcdf(AnEnDefaults::Verbose verbose);
    AnEnWriteNcdf(const AnEnWriteNcdf& orig);
    virtual ~AnEnWriteNcdf();

    void writeAnalogs(const std::string & file_path,
            const Analogs & analogs,
            const Stations & analog_stations,
            const Times & analog_times,
            const Times & analog_flts,
            const Stations & search_stations,
            const Times & search_times) const;

protected:
    AnEnDefaults::Verbose verbose_;

    netCDF::NcDim getDim_(const netCDF::NcFile & nc,
            std::string name, size_t len = 0) const;

    void append_(const netCDF::NcFile & nc, const Analogs & analogs) const;
    void append_(const netCDF::NcFile & nc, const Times & flts,
            bool unlimited) const;

};

#endif /* ANENWRITENCDF_H */

