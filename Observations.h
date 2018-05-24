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
#include <boost/multi_array.hpp>

class Observations {
public:
    Observations() = delete;
    Observations(const Observations& orig) = delete;
    Observations(Parameters, Stations, Times);

    virtual ~Observations();

    virtual double getValue( size_t, size_t, size_t ) const = 0;
    virtual bool setValue( double, size_t, size_t, size_t ) = 0;
    
    /// This is needed because NetCDF only provides a C style pointer
    virtual bool setValues( double* ) = 0;

    virtual size_t get_parameters_size() const = 0;
    virtual size_t get_stations_size() const = 0;
    virtual size_t get_times_size() const = 0;
    virtual size_t get_flts_size() const = 0;
    
    virtual void print(std::ostream &) const;
    friend std::ostream& operator<<(std::ostream&, const Observations&);


  
protected:

    Parameters parameters_;
    Stations stations_;
    Times times_;

    //size_t size_parameters_;
    //size_t size_stations_;
    //size_t size_times_;
 
      // A template function that deduces array size
    template<typename T, size_t n>
    size_t arraySize(const T(&)[n]) {
        return n;
    }

};

class Observations_array : public Observations {
public:
    Observations_array(Parameters, Stations, Times);
    virtual ~Observations_array();

    double getValue(size_t, size_t, size_t) const override;
    bool setValue(double, size_t, size_t, size_t) override;
    bool setValues(double*) override;


    void print(std::ostream&) const override;

    Observations_array() = delete;
    Observations_array(const Observations_array& orig) = delete;


private:
    boost::multi_array<double, 3> data_;

};

#endif /* OBSERVATIONS_H */

