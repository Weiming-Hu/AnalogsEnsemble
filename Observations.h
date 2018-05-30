/* 
 * File:   Observations.h
 * Author: Weiming Hu (weiming@psu.edu)
 *
 * Created on May 4, 2018, 11:27 AM
 */

#ifndef OBSERVATIONS_H
#define OBSERVATIONS_H

#include <ostream>
#include <vector>
#include <cmath>

#include "Parameters.h"
#include "Stations.h"
#include "Times.h"
#include <boost/multi_array.hpp>

class Observations {
public:
    // Don't allow default constructor
    Observations() = delete;

    // Delete copy constructor to avoid unexpected call to default constructor
    Observations(const Observations& orig) = delete;

    Observations(Parameters, Stations, Times);
    virtual ~Observations();

    virtual double getValue(std::size_t parameter_ID,
            std::size_t station_ID, double timestamp) const = 0;
    virtual bool setValue(double, std::size_t parameter_ID,
            std::size_t station_ID, double timestamp) = 0;

    /**
     * Sets data values from a vector.
     * 
     * @param vals An std::vector<double> object
     * @return Returns true if succeed setting values.
     */
    virtual bool setValues(const std::vector<double> & vals) = 0;

    std::size_t get_parameters_size() const;
    std::size_t get_stations_size() const;
    std::size_t get_times_size() const;

    Parameters const & getParameters() const;
    Stations const & getStations() const;
    Times const & getTimes() const;

    virtual void print(std::ostream &) const;
    friend std::ostream& operator<<(std::ostream&, const Observations&);

protected:
    Parameters parameters_;
    Stations stations_;
    Times times_;
};

class Observations_array : public Observations {
public:
    Observations_array() = delete;
    Observations_array(const Observations_array& orig) = delete;

    Observations_array(Parameters, Stations, Times);
    virtual ~Observations_array();

    double getValue(std::size_t parameter_ID,
            std::size_t station_ID, double timestamp) const override;
    
    bool setValue(double val, std::size_t parameter_ID,
            std::size_t station_ID, double timestamp) override;
    bool setValues(const std::vector<double> & vals) override;

    void print(std::ostream&) const override;
    friend std::ostream& operator<<(std::ostream&, const Observations_array&);

private:

    /**
     * The private variable data_ is initialized with fortran storage order,
     * a.k.a column-based order. This is because the values read from NetCDF
     * are in column-based order.
     */
    boost::multi_array<double, 3> data_ =
            boost::multi_array<double, 3> (
            boost::extents[0][0][0],
            boost::fortran_storage_order());
    ;
};

#endif /* OBSERVATIONS_H */

