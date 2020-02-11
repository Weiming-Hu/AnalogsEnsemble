/* 
 * File:   AnEnWriteNcdf.h
 * Author: Weiming Hu (weiming@psu.edu)
 *
 * Created on February 10, 2020, 2:42 PM
 */

#ifndef ANENWRITENCDF_H
#define ANENWRITENCDF_H

#include <netcdf>

#include "Config.h"
#include "AnEnIS.h"
#include "AnEnSSE.h"

class AnEnWriteNcdf {
public:
    AnEnWriteNcdf();
    AnEnWriteNcdf(const AnEnWriteNcdf& orig);
    virtual ~AnEnWriteNcdf();

    void writeAnEn(const std::string & file, const AnEnIS &,
            const Times & test_times, const Times & search_times,
            const Times & forecast_flts,  const Parameters &, const Stations &,
            bool overwrite = false) const;
    void writeAnEn(const std::string & file, const AnEnSSE &,
            const Times & test_times, const Times & search_times,
            const Times & forecast_flts, const Parameters &, const Stations &,
            bool overwrite = false) const;
    
    const static bool _unlimited_stations;
    const static bool _unlimited_test_times;
    const static bool _unlimited_flts;
    const static bool _unlimited_members;

protected:
    Verbose verbose_;

    void addStations_(netCDF::NcFile & nc, const Stations &, bool) const;
};

#endif /* ANENWRITENCDF_H */

