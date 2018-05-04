/* 
 * File:   Forecasts.h
 * Author: guido
 *
 * Created on April 18, 2018, 12:40 AM
 */

#ifndef FORECASTS_H
#define FORECASTS_H

#include "Stations.h"
#include "Parameters.h"
#include "Times.h"
#include "Array4D.h"

class Forecasts {
public:
    Forecasts(Parameters, Stations, Times, FLTs);
    Forecasts(const Forecasts& orig);
    virtual ~Forecasts();
    
    virtual bool setValue(double, int, int, int, int) = 0;
    virtual double getValue(int, int, int, int) const = 0;
    
    virtual void print(std::ostream &) const;
    friend std::ostream& operator<<(std::ostream&, Forecasts const &);

    
protected:
    Forecasts();  // I specifically do not want to allow using this!
    
    Parameters parameters_;
    Stations stations_;
    Times times_;
    FLTs flts_;
    
    int size_parameters_;
    int size_stations_;
    int size_time_;
    int size_flt_;
    
};


class Forecasts_array : public Forecasts {
public:
    Forecasts_array(Parameters, Stations, Times, FLTs);    
    virtual ~Forecasts_array();
    
    bool setValue(double, int, int, int, int) override;
    double getValue(int, int, int, int) const override;
    
    void print(std::ostream &) const override;
    
private:
    Forecasts_array();
    
    Array4D data_;
};

#endif /* FORECASTS_H */

