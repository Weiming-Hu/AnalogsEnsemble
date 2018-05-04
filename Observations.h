/* 
 * File:   Observations.h
 * Author: Weiming Hu (weiming@psu.edu)
 *
 * Created on May 4, 2018, 11:27 AM
 */

#ifndef OBSERVATIONS_H
#define OBSERVATIONS_H

#include <ostream>

#include "Parameters.h"
#include "Stations.h"
#include "Times.h"
#include "boost/multi_array.hpp"

class Observations {
public:
    Observations(Parameters, Stations, Times);
    virtual ~Observations();

    virtual bool setValue(double, size_t, size_t, size_t) = 0;
    virtual double getValue(size_t, size_t, size_t) const = 0;

    virtual void print(std::ostream &) const;
    friend std::ostream& operator<<(std::ostream&, const Observations&);
    
    
    Observations() = delete;
    Observations(const Observations& orig) = delete;

protected:

    Parameters parameters_;
    Stations stations_;
    Times times_;

    size_t size_parameters_;
    size_t size_stations_;
    size_t size_times_;

};

class Observations_array : public Observations {
public:
    Observations_array(Parameters, Stations, Times);
    virtual ~Observations_array();

    bool setValue(double, size_t, size_t, size_t) override;
    double getValue(size_t, size_t, size_t) const override;

    void print(std::ostream&) const override;

    Observations_array() = delete;
    Observations_array(const Observations_array& orig) = delete;


private:
    boost::multi_array<double, 3> data_;

};

#endif /* OBSERVATIONS_H */

