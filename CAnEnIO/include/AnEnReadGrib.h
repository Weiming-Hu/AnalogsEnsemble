/* 
 * File:   AnEnReadGrib.h
 * Author: wuh20
 *
 * Created on February 7, 2020, 1:02 PM
 */

#ifndef ANENREADGRIB_H
#define ANENREADGRIB_H

#include <vector>

#include "Forecasts.h"
#include "Observations.h"
#include "FunctionsIO.h"

class AnEnReadGrib {
public:
    AnEnReadGrib();
    AnEnReadGrib(const AnEnReadGrib& orig);
    virtual ~AnEnReadGrib();

    virtual void readForecasts(std::vector<std::string>, Forecasts &
//    const std::string & regex_time_str,
//    const std::string & regex_flt_str,
//    const std::string & regex_cycle_str,
    ) const;
    virtual void readObservations(std::vector<std::string>, Observations &) const;

};

#endif /* ANENREADGRIB_H */

