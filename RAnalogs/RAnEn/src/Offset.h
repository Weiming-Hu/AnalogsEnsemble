/* 
 * File:   Offset.h
 * Author: Weiming Hu <cervone@psu.edu>
 *
 * Created on January 22, 2020, 8:58 AM
 */

#ifndef OFFSET_H
#define OFFSET_H

// [[Rcpp::plugins(cpp11)]]

#include <Rcpp.h>

/**
 * Class Offset is used to create pointer offset for column-major array. This
 * class is used by ForecastsR and ObservationsR. The implementation is adopted
 * from Fabian Scheipl's implementation here:
 * 
 * https://gallery.rcpp.org/articles/simple-array-class/
 */
class Offset {
public:
    Offset();
    Offset(const Offset& orig);
    Offset(Rcpp::IntegerVector dim);
    virtual ~Offset();
    
    int operator() (Rcpp::IntegerVector dims_index) const;
    Rcpp::IntegerVector getDims() const;
    
    Offset & operator=(const Offset & rhs);
    
protected:
    Rcpp::IntegerVector dim_;
};

#endif /* OFFSET_H */

