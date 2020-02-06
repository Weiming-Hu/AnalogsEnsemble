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

/**
 * \class BasicData
 * 
 * \brief BasicData is an implementation of the data structure used by Analog
 * Ensemble. It includes parameters, stations, and times.
 */
class BasicData {
public:
    BasicData();
    BasicData(const Parameters &, const Stations &, const Times &);
    BasicData(const BasicData& orig);
    virtual ~BasicData();

    const Parameters & getParameters() const;
    const Stations & getStations() const;
    const Times & getTimes() const;
  
    Parameters & getParameters();
    Stations & getStations();
    Times & getTimes();
    
    std::size_t getTimeStamp(std::size_t index) const;
    std::size_t getTimeIndex(std::size_t timestamp) const;
    std::size_t getTimeIndex(const Time &) const;
    
    virtual void print(std::ostream &) const;
    friend std::ostream& operator<<(std::ostream&, BasicData const &);
    
protected:
    Parameters parameters_;
    Stations stations_;
    Times times_;
};

#endif /* BASICDATA_H */

