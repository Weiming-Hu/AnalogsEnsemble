/* 
 * File:   Array4D.cpp
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <cervone@psu.edu>
 * 
 * Created on January 29, 2020, 12:36 PM
 */

#include "Array4DPointer.h"

#include <cmath>
#include <cstring>
#include <stdexcept>
#include <algorithm>

using namespace std;

const double Array4DPointer::_DEFAULT_VALUE = NAN;

Array4DPointer::Array4DPointer() : Array4D() {
    fill_n(dims_, 4, 0);
    data_ = nullptr;
    allocated_ = false;
}

Array4DPointer::Array4DPointer(const Array4DPointer& rhs) : Array4D(rhs) {
    *this = rhs;
}

Array4DPointer::Array4DPointer(size_t dim0, size_t dim1, size_t dim2, size_t dim3) :
Array4D() {
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
Array4DPointer::resize(const Array4D& arr) {
    const size_t * dims = arr.shape();
    resize(dims[0], dims[1], dims[2], dims[3]);
    return;
}


void
Array4DPointer::initialize(double value) {
    if (data_ != nullptr) fill_n(data_, num_elements(), value);
}

double
Array4DPointer::getValue(size_t dim0, size_t dim1, size_t dim2, size_t dim3) const {
    return data_[toIndex_(dim0, dim1, dim2, dim3)];
}

void
Array4DPointer::setValue(double val,
        size_t dim0, size_t dim1, size_t dim2, size_t dim3) {
    data_[toIndex_(dim0, dim1, dim2, dim3)] = val;
    return;
}

void
Array4DPointer::subset(const vector<size_t> & dim0_indices,
        const vector<size_t> & dim1_indices,
        const vector<size_t> & dim2_indices,
        const vector<size_t> & dim3_indices,
        Array4D & arr_subset) const {

    // Make sure the indices are sorted
    bool sorted = is_sorted(dim0_indices.begin(), dim0_indices.end()) &&
            is_sorted(dim1_indices.begin(), dim1_indices.end()) &&
            is_sorted(dim2_indices.begin(), dim2_indices.end()) &&
            is_sorted(dim3_indices.begin(), dim3_indices.end());
    if (!sorted) throw runtime_error("Subset indices must be sorted in ascension order");

    // Allocate memory
    arr_subset.resize(
            dim0_indices.size(),
            dim1_indices.size(),
            dim2_indices.size(),
            dim3_indices.size());

    // Get pointers to data
    const double * p_ori = getValuesPtr();
    double * p_subset = arr_subset.getValuesPtr();

    size_t from_pos, offset;
    size_t last_pos = 0;

    // Loop through the data in column-major
    for (size_t dim3_i : dim3_indices)
        for (size_t dim2_i : dim2_indices)
            for (size_t dim1_i : dim1_indices)
                for (size_t dim0_i : dim0_indices) {

                    // Calculate position offset
                    from_pos = toIndex_(dim0_i, dim1_i, dim2_i, dim3_i);
                    offset = from_pos - last_pos;

                    // Advance the original data pointer
                    p_ori = p_ori + offset;

                    // Copy this value
                    *p_subset = *p_ori;

                    // Advance the subset data pointer
                    p_subset++;

                    // Update last position
                    last_pos = from_pos;
                }

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

void
Array4DPointer::print(ostream & os) const {
    os << "[Array4D] shape [" << dims_[0] << "," << dims_[1] << ","
            << dims_[2] << "," << dims_[3] << "]" << endl;

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

ostream &
operator<<(ostream & os, const Array4DPointer & obj) {
    obj.print(os);
    return os;
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
