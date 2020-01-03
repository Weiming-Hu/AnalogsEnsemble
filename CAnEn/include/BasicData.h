/* 
 * File:   BasicData.h
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <weiming@psu.edu>*
 * Created on January 2, 2020, 3:18 PM
 */

#ifndef BASICDATA_H
#define BASICDATA_H

class BasicData {
public:
    BasicData();
    BasicData(const BasicData& orig);
    virtual ~BasicData();

    virtual const double* getValuesPtr() const = 0;
    virtual double * getValuesPtr() = 0;

    virtual std::size_t size() const = 0;

    anenPar::Parameters const & getParameters() const;
    anenSta::Stations const & getStations() const;
    anenTime::Times const & getTimes() const;
  
    anenPar::Parameters & getParameters();
    anenSta::Stations & getStations();
    anenTime::Times & getTimes();

protected:
    anenPar::Parameters parameters_;
    anenSta::Stations stations_;
    anenTime::Times times_;
};

#endif /* BASICDATA_H */

