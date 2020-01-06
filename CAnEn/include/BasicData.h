/* 
 * File:   BasicData.h
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <weiming@psu.edu>*
 * Created on January 2, 2020, 3:18 PM
 */

#ifndef BASICDATA_H
#define BASICDATA_H

#include "Parameters.h"
#include "Stations.h"
#include "Times.h"




class BasicData {
public:
    BasicData();
    BasicData(const Parameters &, const Stations &, const Times &);
    BasicData(const BasicData& orig);
    virtual ~BasicData();

    /**************************************************************************
     *                          Pure Virtual Functions                        *
     **************************************************************************/
    
    virtual std::size_t size() const = 0;
    virtual double * getValuesPtr() = 0;
    virtual const double* getValuesPtr() const = 0;
    
    /**************************************************************************
     *                           Member Functions                             *
     **************************************************************************/

    const Parameters & getParameters() const;
    const Stations & getStations() const;
    const Times & getTimes() const;
  
    Parameters & getParameters();
    Stations & getStations();
    Times & getTimes();
    

protected:
    Parameters parameters_;
    Stations stations_;
    Times times_;
};

#endif /* BASICDATA_H */

