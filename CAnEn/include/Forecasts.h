/* 
 * File:   Forecasts.h
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <cervone@psu.edu>
 *
 * Created on April 18, 2018, 12:40 AM
 */

#ifndef FORECASTS_H
#define FORECASTS_H

#include "BasicData.h"

/**
 * \class Forecasts
 * 
 * \brief Forecasts class provides interface for reading and writing forecast
 * data from and to a NetCDF file.
 */
class Forecasts : public BasicData {
public:
    Forecasts();
    Forecasts(Forecasts const & orig);
    Forecasts(const Parameters &, const Stations &,
            const Times &, const Times &);
    
    virtual ~Forecasts();

    /**************************************************************************
     *                          Pure Virtual Functions                        *
     **************************************************************************/
    
    /**
     * Returns the total number of data values as one dimension.
     * @return A value.
     */
    virtual std::size_t num_elements() const override = 0;

    /**
     * Gets a double pointer to the start of the values.
     * @return A double pointer.
     */
    virtual const double* getValuesPtr() const override = 0;
    virtual double * getValuesPtr() override = 0;

    /**
     * Sets the dimensions of forecasts and allocates memory for data values.
     * @param parameters The Parameters container.
     * @param stations The Stations container.
     * @param times The Times container.
     * @param flts The FLTs container.
     */
    virtual void setDimensions(const Parameters & parameters,
            const Stations & stations, const Times & times,
            const Times & flts) = 0;

    /**
     * Gets or sets a value using the array indices.
     * @param parameter_index Parameter index.
     * @param station_index Station index.
     * @param time_index Time index.
     * @param flt_index FLT index.
     * @return A value.
     */
    virtual double getValue(std::size_t parameter_index,
            std::size_t station_index, std::size_t time_index,
            std::size_t flt_index) const = 0;
    virtual void setValue(double val, std::size_t parameter_index,
            std::size_t station_index, std::size_t time_index,
            std::size_t flt_index) = 0;

    /**************************************************************************
     *                           Member Functions                             *
     **************************************************************************/
    
    Times const & getFLTs() const;
    Times & getFLTs();
    
    std::size_t getFltTimeStamp(std::size_t index) const;
    std::size_t getFltTimeIndex(std::size_t timestamp) const;
    std::size_t getFltTimeIndex(const Time &) const;
    
    virtual void print(std::ostream &) const;
    friend std::ostream& operator<<(std::ostream&, Forecasts const &);

protected:
    Times flts_;
};

#endif /* FORECASTS_H */

