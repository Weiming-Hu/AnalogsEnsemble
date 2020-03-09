/*
 * File:   AnEnReadGrib.h
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on February 7, 2020, 1:02 PM
 */

#ifndef ANENREADGRIB_H
#define ANENREADGRIB_H

#include <vector>

#include "Config.h"
#include "Forecasts.h"
#include "FunctionsIO.h"
#include "Observations.h"
#include "ParameterGrib.h"

class AnEnReadGrib {
public:
    AnEnReadGrib();
    AnEnReadGrib(const AnEnReadGrib& orig);
    AnEnReadGrib(Verbose verbose);
    virtual ~AnEnReadGrib();

    void readForecasts(Forecasts & forecasts,
            const std::vector<ParameterGrib> & grib_parameters,
            const std::vector<std::string> & files,
            const std::string & regex_str,
            size_t unit_in_seconds = 3600,
            bool delimited = false,
            std::vector<int> stations_index = {}) const;

protected:
    Verbose verbose_;
    
    void readForecastsMeta_(Forecasts & forecasts,
            const std::vector<ParameterGrib> & grib_parameters,
            const std::vector<std::string> & files,
            const std::string & regex_str,
            size_t unit_in_seconds, bool delimited,
            std::vector<int> stations_index) const;

    void readStations_(Stations&, const std::string &,
            const std::vector<int> & stations_index = {}) const;
};

#endif /* ANENREADGRIB_H */

