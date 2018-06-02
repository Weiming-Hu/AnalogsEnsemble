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

/**
 * \class Observations
 * 
 * \brief Observations class provides interface for reading and writing 
 * observations from and to a NetCDF file. It also supports data indexing
 * and searching.
 */
class Observations {
public:
    // Don't allow default constructor
    Observations();

    // Delete copy constructor to avoid unexpected call to default constructor
    Observations(const Observations& orig) = delete;

    Observations(anenPar::Parameters, anenSta::Stations, anenTime::Times);

    virtual ~Observations();

    /**
     * Gets value using array index.
     * 
     * @param parameter_index Parameter index.
     * @param station_index Station index.
     * @param time_index Time index.
     * @return A double.
     */
    virtual double getValue(std::size_t parameter_index,
            std::size_t station_index, std::size_t time_index) const = 0;
    
    /**
     * Gets value by searching with IDs and timestamps.
     * 
     * @param parameter_ID Parameter ID.
     * @param station_ID Station ID.
     * @param timestamp A double for the time.
     * @return A double.
     */
    virtual double getValue(std::size_t parameter_ID,
            std::size_t station_ID, double timestamp) const = 0;

    virtual void setValue(double val, std::size_t parameter_index,
            std::size_t station_index, std::size_t time_index) = 0;
    virtual void setValue(double val, std::size_t parameter_ID,
            std::size_t station_ID, double timestamp) = 0;

    /**
     * Sets values from a vector.
     * 
     * @param vals A vector<double> with values. Call 
     * Observations::updateDataDims to make sure sufficient space is allocated.
     */
    virtual void setValues(const std::vector<double> & vals) = 0;
    
    /**
     * Resizes underlying data member according to the sizes of parameters,
     * stations, and times.
     */
    virtual void updateDataDims() = 0;

    /**
     * Given a valid parameter ID, this function returns the corresponding
     * index of that specific parameter from underlying data.
     * 
     * @param parameter_ID A valid parameter ID.
     * @return The parameter ID index.
     */
    std::size_t getParameterIndex(std::size_t parameter_ID) const;
    std::size_t getStationIndex(std::size_t station_ID) const;
    std::size_t getTimeIndex(double timestamp) const;
    
    std::size_t get_parameters_size() const;
    std::size_t get_stations_size() const;
    std::size_t get_times_size() const;

    const anenPar::Parameters & getParameters() const;
    const anenSta::Stations & getStations() const;
    const anenTime::Times & getTimes() const;
    
    void setParameters(anenPar::Parameters parameters_);
    void setStations(anenSta::Stations stations_);
    void setTimes(anenTime::Times times_);

    virtual void print(std::ostream &) const;
    friend std::ostream& operator<<(std::ostream&, const Observations&);

protected:
    anenPar::Parameters parameters_;
    anenSta::Stations stations_;
    anenTime::Times times_;
};

/**
 * \class Observations_array
 * 
 * \brief Observations_array class is an implementation of the class 
 * Observations. Data are stored using the boost::multi_array<double, 3> class.
 */
class Observations_array : public Observations {
public:
    Observations_array();
    Observations_array(const Observations_array& orig) = delete;

    Observations_array(anenPar::Parameters, anenSta::Stations, anenTime::Times);
    Observations_array(anenPar::Parameters, anenSta::Stations, anenTime::Times,
            const std::vector<double> & vals);

    virtual ~Observations_array();
    
    boost::multi_array<double, 3> const & getValues() const;

    double getValue(std::size_t parameter_index,
            std::size_t station_index, std::size_t time_index) const override;
    double getValue(std::size_t parameter_ID,
            std::size_t station_ID, double timestamp) const override;

    void setValue(double val, std::size_t parameter_index,
            std::size_t station_index, std::size_t time_index) override;
    void setValue(double val, std::size_t parameter_ID,
            std::size_t station_ID, double timestamp) override;

    void setValues(const std::vector<double> & vals) override;
    
    void updateDataDims() override;

    void print(std::ostream&) const override;
    friend std::ostream& operator<<(std::ostream&, const Observations_array&);

private:
    /**
     * The private variable data_ is initialized with Fortran storage order,
     * a.k.a column-based order. This is because the values read from NetCDF
     * are in column-based order (boost::fortran_storage_order()).
     */
    boost::multi_array<double, 3> data_ =
            boost::multi_array<double, 3> (
            boost::extents[0][0][0],
            boost::fortran_storage_order());
};

#endif /* OBSERVATIONS_H */

