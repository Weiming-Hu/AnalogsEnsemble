/* 
 * File:   Forecasts.h
 * Author: guido
 *
 * Created on April 18, 2018, 12:40 AM
 */

#ifndef FORECASTS_H
#define FORECASTS_H

#include <ostream>
#include <cmath>
#include <vector>
#include <memory>

#include "Stations.h"
#include "Parameters.h"
#include "Times.h"
#include "Array4D.h"
#include "colorTexts.h"

/**
 * \class Forecasts
 * 
 * \brief Forecasts class provides interface for reading and writing forecast
 * data from and to a NetCDF file. It also supports data indexing and searching.
 */
class Forecasts {
public:
    Forecasts();
    Forecasts(Forecasts const &) = delete;

    Forecasts(anenPar::Parameters, anenSta::Stations,
            anenTime::Times, anenTime::FLTs);

    virtual ~Forecasts();

    /**
     * Gets value using array index.
     * 
     * @param parameter_index Parameter index.
     * @param station_index Station index.
     * @param time_index Time index.
     * @param flt_index FLT index.
     * @return A value.
     */
    virtual double getValueByIndex(std::size_t parameter_index,
            std::size_t station_index, std::size_t time_index,
            std::size_t flt_index) const = 0;

    /**
     * Gets value by searching with IDs and timestamps.
     * 
     * @param parameter_ID Parameter ID.
     * @param station_ID Station ID.
     * @param timestamp A timestamp for Time.
     * @param flt A timestamp for FLT.
     * @return A value.
     */
    virtual double getValueByID(
            std::size_t parameter_ID, std::size_t station_ID,
            double timestamp, double flt) const = 0;

    /**
     * Gets data in form of a double pointer.
     * @return A double pointer.
     */
    virtual const double* getValues() const = 0;

    virtual void setValue(double val, std::size_t parameter_index,
            std::size_t station_index, std::size_t time_index,
            std::size_t flt_index) = 0;
    virtual void setValue(double,
            std::size_t parameter_ID, std::size_t station_ID,
            double timestamp, double flt) = 0;

    /**
     * Sets data values from a vector.
     * 
     * @param vals An std::vector<double> object
     */
    virtual void setValues(const std::vector<double> & vals) = 0;

    /**
     * Resizes underlying data member according to the sizes of parameters,
     * stations, times, and flts.
     */
    virtual void updateDataDims() = 0;

    std::size_t getParametersSize() const;
    std::size_t getStationsSize() const;
    std::size_t getTimesSize() const;
    std::size_t getFLTsSize() const;
    virtual std::size_t getDataLength() const = 0;

    anenPar::Parameters const & getParameters() const;
    anenSta::Stations const & getStations() const;
    anenTime::Times const & getTimes() const;
    anenTime::FLTs const & getFLTs() const;

    void setFlts(anenTime::FLTs flts);
    void setParameters(anenPar::Parameters parameters);
    void setStations(anenSta::Stations stations);
    void setTimes(anenTime::Times times);

    virtual void print(std::ostream &) const;
    friend std::ostream& operator<<(std::ostream&, Forecasts const &);

protected:
    anenPar::Parameters parameters_;
    anenSta::Stations stations_;
    anenTime::Times times_;
    anenTime::FLTs flts_;
};

/**
 * \class Forecasts_array
 * 
 * \brief Forecasts_array class is an implementation of class Forecasts. Data 
 * are stored using the Array4D class.
 */
class Forecasts_array : public Forecasts {
public:
    Forecasts_array();
    Forecasts_array(const Forecasts_array & rhs) = delete;

    Forecasts_array(anenPar::Parameters, anenSta::Stations,
            anenTime::Times, anenTime::FLTs);
    Forecasts_array(anenPar::Parameters, anenSta::Stations,
            anenTime::Times, anenTime::FLTs,
            const std::vector<double> & vals);

    virtual ~Forecasts_array();

    Array4D const & data() const;

    double getValueByIndex(std::size_t parameter_index, std::size_t station_index,
            std::size_t time_index, std::size_t flt_index) const override;
    double getValueByID(std::size_t parameter_ID, std::size_t station_ID,
            double timestamp, double flt) const override;

    const double* getValues() const override;

    void setValue(double val, std::size_t parameter_index,
            std::size_t station_index, std::size_t time_index,
            std::size_t flt_index) override;
    void setValue(double val, std::size_t parameter_ID, std::size_t station_ID,
            double timestamp, double flt) override;

    void setValues(const std::vector<double>& vals) override;

    void updateDataDims() override;
    
    size_t getDataLength() const override;

    void print(std::ostream &) const override;
    friend std::ostream& operator<<(std::ostream&, const Forecasts_array&);

private:
    Array4D data_ = Array4D(
            boost::extents[0][0][0][0],
            boost::fortran_storage_order());
};

#endif /* FORECASTS_H */

