/* 
 * File:   Array4D.cpp
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <cervone@psu.edu>
 * 
 * Created on January 29, 2020, 12:36 PM
 */

#include "Array4DPointer.h"
#include <cstring>
#include <cmath>
#include <algorithm>

using namespace std;

const double Array4DPointer::_DEFAULT_VALUE = NAN;

Array4DPointer::Array4DPointer() {
    data_ = nullptr;
    allocated_ = false;
}

Array4DPointer::Array4DPointer(const Array4DPointer& orig) {
    *this = orig;
}

Array4DPointer::Array4DPointer(size_t dim0, size_t dim1, size_t dim2, size_t dim3) {
    allocated_ = false;
    resize(dim0, dim1, dim2, dim3);
}

Array4DPointer::~Array4DPointer() {
    if (allocated_) delete [] data_;
}


const size_t*
Array4DPointer::shape() const {
    return dims_;
}

size_t
Array4DPointer::num_elements() const {
    return dims_[0] * dims_[1] * dims_[2] * dims_[3];
}

const double*
Array4DPointer::getValuesPtr() const {
    return data_;
}

double*
Array4DPointer::getValuesPtr() {
    return data_;
}

void
Array4DPointer::resize(size_t dim0, size_t dim1, size_t dim2, size_t dim3) {

    // Set members in the parent class
    dims_[0] = dim0;
    dims_[1] = dim1;
    dims_[2] = dim2;
    dims_[3] = dim3;

    // Allocate data
    allocateMemory_();
    return;
}

void
Array4DPointer::initialize(double value) {
    if ( data_ != nullptr ) fill_n(data_, num_elements(), value);
}


double
Array4DPointer::getValue(size_t dim0, size_t dim1, size_t dim2, size_t dim3) const {
    return data_[toIndex_(dim0, dim1, dim2, dim3)];
}

void Array4DPointer::setValue(double val,
        size_t dim0, size_t dim1, size_t dim2, size_t dim3) {
    data_[toIndex_(dim0, dim1, dim2, dim3)] = val;
    return;
}

size_t
Array4DPointer::toIndex_(size_t dim0, size_t dim1, size_t dim2, size_t dim3) const {
    // Convert dimension indices to position offset by column-major
    return dim0 + dims_[0] * (dim1 + dims_[1] * (dim2 + dims_[2] * dim3));
}

void
Array4DPointer::allocateMemory_() {

    // Allocate memory for underlying data structure
    if (allocated_) delete [] data_;

    size_t total = num_elements();
    data_ = new double [total];

    allocated_ = true;
    return;
}

Array4DPointer &
        Array4DPointer::operator=(const Array4DPointer & rhs) {
    if (this != &rhs) {

        // Copy dimensions
        memcpy(dims_, rhs.dims_, 4 * sizeof (size_t));

        // Copy values
        data_ = new double [rhs.num_elements()];
        memcpy(data_, rhs.data_, sizeof (double) * rhs.num_elements());

        allocated_ = true;
    }
    
    return *this;
}

void
Array4DPointer::print(std::ostream & os) const {
    os << "[Array4D] size: " << num_elements() << std::endl;

    for (size_t l = 0; l < dims_[0]; ++l) {
        for (size_t m = 0; m < dims_[1]; ++m) {
            cout << "[" << l << "," << m << ",,]" << endl;

            for (size_t p = 0; p < dims_[3]; ++p) os << "\t[,,," << p << "]";
            os << endl;

            for (size_t o = 0; o < dims_[2]; ++o) {
                os << "[,," << o << ",]\t";

                for (size_t p = 0; p < dims_[3]; ++p) {
                    os << getValue(l, m, o, p) << "\t";
                }

                os << endl;
            }

            os << endl;
        }
    }

    return;
}

std::ostream &
operator<<(std::ostream & os, const Array4DPointer & obj) {
    obj.print(os);
    return os;
}