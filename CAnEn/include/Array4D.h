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

using vector4 = size_t[4];

class Array4D {
public:
    Array4D();
    virtual ~Array4D();

    /**************************************************************************
     *                          Pure Virtual Functions                        *
     **************************************************************************/

    virtual const std::size_t* shape() const = 0;
    virtual std::size_t num_elements() const = 0;

    virtual const double* getValuesPtr() const = 0;
    virtual double* getValuesPtr() = 0;

    virtual void resize(std::size_t, std::size_t, std::size_t, std::size_t) = 0;
    virtual void initialize(double) = 0;

    virtual double getValue(std::size_t, std::size_t, std::size_t, std::size_t) const = 0;
    virtual void setValue(double val, std::size_t, std::size_t, std::size_t, std::size_t) = 0;
};

#endif /* ARRAY4D_H */

