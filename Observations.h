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
#include <set>
#include <map>

class Observations {
public:
    // Don't allow default constructor
    Observations() = delete;

    // Delete copy constructor to avoid unexpected call to default constructor
    Observations(const Observations& orig) = delete;

    Observations(Parameters, Stations, Times, const std::vector<double> & vals);
    virtual ~Observations();

    virtual double getValue(std::size_t parameter_index,
            std::size_t station_index, std::size_t time_intex) const = 0;
    virtual bool setValue(std::size_t parameter_index,
            std::size_t station_index, std::size_t time_intex) = 0;


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
    //virtual bool setValues() = 0;

    std::size_t get_parameters_size() const;
    std::size_t get_stations_size() const;
    std::size_t get_times_size() const;

    Parameters const & getParameters() const;
    Stations const & getStations() const;
    Times const & getTimes() const;

    
    virtual std::size_t getParametersIndex( std::size_t parameterID) const = 0;
    virtual std::size_t getStationsIndex( std::size_t stationID) const = 0;
    virtual std::size_t getTimeIndex( double timestamp) const = 0;
    
    
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

    Observations_array(Parameters, Stations, Times, const std::vector<double> & vals);
    virtual ~Observations_array();

    double getValue(std::size_t parameter_ID,
            std::size_t station_ID, double timestamp) const override;

    bool setValue(double val, std::size_t parameter_ID,
            std::size_t station_ID, double timestamp) override;

    
    int getTimesIndex(double timestamp) {
        auto f = times_.find(timestamp);
        if (f == times_.end()) {
            return -1;
        }
        return (std::distance(times_.begin(), f));
    } const override;

    int getParametersIndex( std::size_t parameter_ID) {
        auto ret = parameters_indexes_.at(parameter_ID);
        return ( (ret.second) ? ret.first->second : -1 );
    }const override;

    int getStationsIndex( std::size_t station_ID) {
        auto ret = stations_indexes_.at(station_ID);
        return ( (ret.second) ? ret.first->second : -1 );
    }const override;

    
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
    
    std::map <unsigned int, unsigned int> parameters_indexes_;
    std::map <unsigned int, unsigned int> stations_indexes_;


};

#endif /* OBSERVATIONS_H */

