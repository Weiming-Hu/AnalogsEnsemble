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
#include "Forecasts.h"
#include "Observations.h"

class AnEnWriteNcdf {
public:
    AnEnWriteNcdf();
    AnEnWriteNcdf(const AnEnWriteNcdf& orig);
    AnEnWriteNcdf(Verbose verbose);
    virtual ~AnEnWriteNcdf();

    void writeAnEn(const std::string & file, const AnEnIS &,
            const Times & test_times, const Times & search_times,
            const Times & forecast_flts, const Parameters &, const Stations &,
            bool overwrite = false, bool append = false) const;
    void writeAnEn(const std::string & file, const AnEnSSE &,
            const Times & test_times, const Times & search_times,
            const Times & forecast_flts, const Parameters &, const Stations &,
            bool overwrite = false, bool append = false) const;

    void writeForecasts(const std::string & file, const Forecasts &,
            bool overwrite = false, bool append = false) const;
    void writeObservations(const std::string & file, const Observations &,
            bool overwrite = false, bool append = false) const;

    /*
     * Unlimited dimensions are not supported. Unlimited dimensions might be
     * a future feature.
     * 
     * To implement unlimited dimensions, you probably need to add dimensional
     * variables. Right now, dimensions are not associated with variables.
     * 
     * For good practices, please refer to 
     * https://www.unidata.ucar.edu/software/netcdf/docs/BestPractices.html
     */
    const static bool _unlimited_parameters;
    const static bool _unlimited_stations;
    const static bool _unlimited_test_times;
    const static bool _unlimited_search_times;
    const static bool _unlimited_times;
    const static bool _unlimited_flts;
    const static bool _unlimited_members;

protected:
    Verbose verbose_;

    void addBasicData_(netCDF::NcGroup &, const BasicData &) const;
    void addStations_(netCDF::NcGroup &, const Stations &, bool) const;
    void addMeta_(netCDF::NcGroup &) const;
};

#endif /* ANENWRITENCDF_H */

