/* 
 * File:   Offset.cpp
 * Author: Weiming Hu <cervone@psu.edu>
 * 
 * Created on January 22, 2020, 8:58 AM
 */

#include "Offset.h"
#include <stdexcept>
#include <numeric>

using namespace std;

Offset::Offset() {
}

Offset::Offset(const Offset& orig) {
    *this = orig;
}

Offset::Offset(const Rcpp::IntegerVector & dim) {
    dim_ = Rcpp::as< vector<size_t> >(dim);
}

Offset::~Offset() {
}

size_t
Offset::num_elements() const {
    return accumulate(dim_.begin(), dim_.end(), 1, std::multiplies<size_t>());
}

size_t
Offset::operator()(const vector<size_t> & dims_index) const {
    
    size_t pos = dims_index[0];
    size_t offset = 1;
    
    // Convert dimension indices to position offset by column-major
    for (size_t i = 1; i < dim_.size(); ++i) {
        if (dims_index[i-1] >= dim_[i-1]) throw std::runtime_error("Offset index out of bound");
        offset *= dim_[i-1];
        pos += dims_index[i] * offset;
    }
    
    return pos;
}

vector<size_t>
Offset::getDims() const {
    return dim_;
}

void
Offset::setDims(const std::vector<size_t> & dims) {
    dim_ = dims;
}

Offset &
Offset::operator=(const Offset & rhs) {
    if (this != &rhs) {
        dim_ = rhs.dim_;
    }
    return *this;
}
