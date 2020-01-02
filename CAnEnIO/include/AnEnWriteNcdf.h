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

class AnEnWriteNcdf {
public:

    enum class FileType {
        Forecasts, Observations, Similarity,
        Analogs, StandardDeviation
    };

    // A higher level will contain all messages from the lower levels. For
    // example, The progress level will contain errors and warnings. Errors,
    // if any, must be printed.
    //

    enum class Verbose {
        Error = 0, Warning = 1, Progress = 2,
        Detail = 3, Debug = 4
    };

    AnEnWriteNcdf();
    AnEnWriteNcdf(Verbose verbose);
    AnEnWriteNcdf(const AnEnWriteNcdf& orig);
    virtual ~AnEnWriteNcdf();

    void writeAnalogs(const std::string & file_path,
            const Analogs & analogs,
            const anenSta::Stations & analog_stations,
            const anenTime::Times & analog_times,
            const anenTime::FLTs & analog_flts,
            const anenSta::Stations & search_stations,
            const anenTime::Times & search_times) const;

protected:
    Verbose verbose_;

    netCDF::NcDim getDim_(const netCDF::NcFile & nc,
            std::string name, size_t len = 0) const;

    void append_(const netCDF::NcFile & nc, const Analogs & analogs) const;
    void append_(const netCDF::NcFile & nc, const anenTime::FLTs & flts,
            bool unlimited) const;

};

#endif /* ANENWRITENCDF_H */

