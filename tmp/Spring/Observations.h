/* 
 * File:   Observations.h
 * Author: Weiming Hu (weiming@psu.edu)
 *
 * Created on May 4, 2018, 11:27 AM
 */

#ifndef OBSERVATIONS_H
#define OBSERVATIONS_H

#include "BasicData.h"

/**
 * \class Observations
 * 
 * \brief Observations class provides interface for reading and writing 
 * observations from and to a NetCDF file. It also supports data indexing
 * and searching.
 */
class Observations  : public BasicData {
public:
    Observations();
    Observations(const Observations& orig);
    Observations(const Parameters &, const Stations &, const Times &);

    virtual ~Observations();

    /**************************************************************************
     *                          Pure Virtual Functions                        *
     **************************************************************************/

   /**
     * Returns the total number of data values as one dimension.
     * @return A value.
     */
    virtual std::size_t size() const override = 0;

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
     */
    virtual void setDimensions(const Parameters & parameters,
            const Stations & stations, const Times & times) = 0;

    /**
     * Gets or sets a value using the array indices.
     * @param parameter_index Parameter index.
     * @param station_index Station index.
     * @param time_index Time index.
     * @return a value.
     */
    virtual double getValue(std::size_t parameter_index,
            std::size_t station_index, std::size_t time_index) const = 0;
    virtual void setValue(double val, std::size_t parameter_index,
            std::size_t station_index, std::size_t time_index) = 0;

    /**************************************************************************
     *                           Member Functions                             *
     **************************************************************************/
    
    virtual void print(std::ostream &) const;
    friend std::ostream& operator<<(std::ostream&, const Observations&);

protected:

};

#endif /* OBSERVATIONS_H */

