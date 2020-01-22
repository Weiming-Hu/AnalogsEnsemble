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

class Offset {
public:
    Offset();
    Offset(const Offset& orig);
    Offset(Rcpp::IntegerVector dim);
    virtual ~Offset();
    
    int operator() (Rcpp::IntegerVector dims_index) const;
    Rcpp::IntegerVector getDims() const;
    
protected:
    Rcpp::IntegerVector dim_;
};

#endif /* OFFSET_H */

