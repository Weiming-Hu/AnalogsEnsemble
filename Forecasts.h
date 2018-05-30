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

#include "Stations.h"
#include "Parameters.h"
#include "Times.h"
#include "Array4D.h"

class Forecasts {
public:
    Forecasts() = delete;
    Forecasts(Forecasts const &) = delete;

    Forecasts(Parameters, Stations, Times, FLTs);
    virtual ~Forecasts();

    virtual double getValue(std::size_t parameter_ID, std::size_t station_ID,
            double timestamp, double flt) const = 0;
    virtual bool setValue(double,
            std::size_t parameter_ID, std::size_t station_ID,
            double timestamp, double flt) = 0;

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
    std::size_t get_flts_size() const;

    Parameters const & getParameters() const;
    Stations const & getStations() const;
    Times const & getTimes() const;
    FLTs const & getFLTs() const;


    virtual void print(std::ostream &) const;
    friend std::ostream& operator<<(std::ostream&, Forecasts const &);

protected:
    Parameters parameters_;
    Stations stations_;
    Times times_;
    FLTs flts_;
};

class Forecasts_array : public Forecasts {
public:
    Forecasts_array() = delete;
    Forecasts_array(const Forecasts_array & rhs) = delete;

    Forecasts_array(Parameters, Stations, Times, FLTs);
    virtual ~Forecasts_array();

    Array4D const & getValues() const;
    double getValue(std::size_t parameter_ID, std::size_t station_ID,
            double timestamp, double flt) const override;
    
    bool setValue(double val, std::size_t parameter_ID, std::size_t station_ID,
            double timestamp, double flt) override;
    bool setValues(const std::vector<double>& vals) override;

    void print(std::ostream &) const override;
    friend std::ostream& operator<<(std::ostream&, const Forecasts_array&);

private:
    Array4D data_;
};

#endif /* FORECASTS_H */

