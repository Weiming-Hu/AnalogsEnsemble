/* 
 * File:   Offset.h
 * Author: Weiming Hu <cervone@psu.edu>
 *
 * Created on January 22, 2020, 8:58 AM
 */

#ifndef OFFSET_H
#define OFFSET_H

#include <Rcpp.h>
#include <vector>

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
    Offset(const Rcpp::IntegerVector & dim);
    virtual ~Offset();
    
    std::size_t num_elements() const;
    
    size_t operator() (const std::vector<size_t> & dims_index) const;
    std::vector<size_t> getDims() const;
    
    void setDims(const std::vector<size_t> & dims);
    
    Offset & operator=(const Offset & rhs);
    
protected:
    std::vector<size_t> dim_;
};

#endif /* OFFSET_H */

