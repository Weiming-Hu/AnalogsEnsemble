/* 
 * File:   Offset.cpp
 * Author: Weiming Hu <cervone@psu.edu>
 * 
 * Created on January 22, 2020, 8:58 AM
 */

#include "Offset.h"

using namespace Rcpp;

Offset::Offset() {
}

Offset::Offset(const Offset& orig) {
    *this = orig;
}

Offset::Offset(IntegerVector dim) : dim_(dim) {
}

Offset::~Offset() {
}

int Offset::operator()(IntegerVector dims_index) const {
    
    int pos = dims_index[0];
    int offset = 1;
    
    // Convert dimension indices to position offset by column-major
    for (int i = 1; i < dim_.size(); ++i) {
        if (dims_index[i-1] >= dim_[i-1]) throw std::runtime_error("Index out of bound");
        offset *= dim_[i-1];
        pos += dims_index[i] * offset;
    }
    
    return pos;
}

IntegerVector Offset::getDims() const {
    return dim_;
}

Offset &
Offset::operator=(const Offset & rhs) {
    if (this != &rhs) {
        dim_ = rhs.dim_;
    }
    return *this;
}
