/* 
 * File:   StandardDeviation.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on August 3, 2018, 3:32 PM
 */

#ifndef STANDARDDEVIATION_H
#define STANDARDDEVIATION_H

#ifndef BOOST_NO_AUTO_PTR
#define BOOST_NO_AUTO_PTR
#endif

#include "boost/multi_array.hpp"

#include <iostream>

/**
 * \class StandardDeviation
 * 
 * \brief StandardDeviation class is designed for storing the standard 
 * deviation of a Forecasts object. The dimensions are 
 *                  [parameters][stations][FLTs]
 * 
 * StandardDeviation data order is column-major.
 */
class StandardDeviation : public boost::multi_array<double, 3> {
public:
    StandardDeviation();
    StandardDeviation(size_t dim1, size_t dim2, size_t dim3);
    StandardDeviation(const StandardDeviation& orig) = delete;
    virtual ~StandardDeviation();

    void print(std::ostream &) const;
    void printSize(std::ostream &) const;
    friend std::ostream & operator<<(std::ostream &,
            const StandardDeviation&);
};

#endif /* STANDARDDEVIATION_H */

