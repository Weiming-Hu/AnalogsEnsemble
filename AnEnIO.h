/* 
 * File:   AnEnIO.h
 * Author: Weiming Hu (weiming@psu.edu)
 *
 * Created on May 4, 2018, 11:16 AM
 */

#ifndef ANENIO_H
#define ANENIO_H

#include <string>

#include "Observations.h"
#include "Forecasts.h"

class AnEnIO {
public:
    AnEnIO();
    virtual ~AnEnIO();
    
    virtual bool readObservations(std::string, Observations&) = 0;
    virtual bool readForecasts(std::string, Forecasts&) = 0;

    AnEnIO(const AnEnIO& orig) = delete;

};


//class AnEnIO_ncdf_serial
#endif /* ANENIO_H */

