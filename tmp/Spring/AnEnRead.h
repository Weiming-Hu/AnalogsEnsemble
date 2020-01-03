/* 
 * File:   AnEnRead.h
 * Author: Weiming Hu (weiming@psu.edu)
 *         Guido Cervone (cervone@psu.edu)
 *
 * Created on December 30, 2019, 11:10 AM
 */

#ifndef ANENREAD_H
#define ANENREAD_H

#include <string>

#include "Forecasts.h"
#include "Observations.h"


/**
 * \class AnEnRead
 * 
 * \brief AnEnRead is an abstract class that defines the interface for 
 * the reading functionality required for AnEn.
 */
class AnEnRead {
public:
    AnEnRead();
    AnEnRead(const AnEnRead& orig);
    virtual ~AnEnRead();
    
    virtual void readForecasts(
            const std::string & file_path,
            Forecasts & forecasts) const = 0;
    virtual void readObservations(
            const std::string & file_path,
            Observations & observations) const = 0;

};

#endif /* ANENREAD_H */

