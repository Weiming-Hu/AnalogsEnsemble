/* 
 * File:   Array4D.h
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <cervone@psu.edu>
 *
 * Created on January 31, 2020, 10:00 AM
 */

#ifndef ARRAY4D_H
#define ARRAY4D_H

#include <iostream>
#include <vector>

/**
 * \class Array4D
 * 
 * \brief Array4D is an abstract class for 4-dimensional array.
 */
class Array4D {
public:
    Array4D() = default;
    Array4D(const Array4D& orig) = default;
    virtual ~Array4D() = default;

    /**************************************************************************
     *                          Pure Virtual Functions                        *
     **************************************************************************/

    virtual const std::size_t* shape() const = 0;

    /**
     * Returns the total number of data values.
     * @return A value.
     */
    virtual std::size_t num_elements() const = 0;

    /**
     * Gets a double pointer to the start of the values.
     * @return A double pointer.
     */
    virtual const double* getValuesPtr() const = 0;
    virtual double* getValuesPtr() = 0;

    /**
     * Resize the dimensions and allocate memory.
     * @param Length of the first dimension
     * @param Length of the second dimension
     * @param Length of the third dimension
     * @param Length of the fourth dimension
     */
    virtual void resize(std::size_t, std::size_t, std::size_t, std::size_t) = 0;

    /**
     * Initialize the values
     * @param The value for initialization
     */
    virtual void initialize(double) = 0;

    /**
     * Gets or sets a value using the array indices.
     * @param parameter_index Parameter index.
     * @param station_index Station index.
     * @param time_index Time index.
     * @param flt_index FLT index.
     * @return A value.
     */
    virtual double getValue(std::size_t, std::size_t, std::size_t, std::size_t) const = 0;
    virtual void setValue(double val, std::size_t, std::size_t, std::size_t, std::size_t) = 0;

    virtual void subset(const std::vector<std::size_t> &, const std::vector<std::size_t> &,
            const std::vector<std::size_t> &, const std::vector<std::size_t> &,
            Array4D &) const = 0;

    virtual void print(std::ostream &) const = 0;
};

#endif /* ARRAY4D_H */

