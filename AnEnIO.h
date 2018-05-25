/* 
 * File:   AnEnIO.h
 * Author: Weiming Hu (weiming@psu.edu)
 *
 * Created on May 4, 2018, 11:16 AM
 */

#ifndef ANENIO_H
#define ANENIO_H

#include <string>

#include "Times.h"
#include "Stations.h"
#include "Observations.h"
#include "Forecasts.h"

#include <netcdf>

class AnEnIO {
public:
    AnEnIO();
    AnEnIO(const AnEnIO& orig) = delete;
    virtual ~AnEnIO();

    int readObservations(std::string, Observations&) const;
    int readForecasts(std::string, Forecasts&) const;
    
    int readParameters(std::string, Parameters &) const;
    int readStations(std::string, Stations &) const;
    int readTimes(std::string, Times &) const;

protected:
    int open_file_(std::string, NcFile &) const;

    enum errorType {
        FILE_NOT_FOUND = -1,
        VAR_NAME_NOT_FOUND = -2,
        WRONG_TYPE = -3,
        INSUFFICIENT_MEMORY = -50,
        UNKNOWN_ERROR = -100,
        SUCCESS = 0
    };

};


//class AnEnIO_ncdf_serial
#endif /* ANENIO_H */

