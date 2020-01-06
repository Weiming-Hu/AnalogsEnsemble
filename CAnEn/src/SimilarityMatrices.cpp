/* 
 * File:   SimilarityMatrices.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 * 
 * Created on August 2, 2018, 8:32 PM
 */

#include "SimilarityMatrices.h"
#include "colorTexts.h"

#include <stdexcept>
#include <algorithm>
#include <exception>
#include <iostream>
#include <sstream>

#if defined(_OPENMP)
#include <omp.h>
#endif

namespace boost {
    namespace detail {
        namespace multi_array {
            template <typename T, size_t dims>
            static void swap(sub_array<T, dims> lhs,
                    sub_array<T, dims> rhs) {
                return SimilarityMatrices::swap(lhs, rhs);
            }
        }
    }
}
using namespace std;
const static int _NUM_COLS = 3;

SimilarityMatrices::SimilarityMatrices(size_t max_entries) :
boost::multi_array<double, 5>(boost::extents[0][0][0][max_entries][_NUM_COLS]),
order_tag_(VALUE), max_entries_(max_entries) {
}

SimilarityMatrices::SimilarityMatrices(const size_t& num_stations,
        const size_t& num_times, const size_t& num_flts,
        const size_t& max_entries) :
boost::multi_array<double, 5>(boost::extents[num_stations][num_times]
[num_flts][max_entries][_NUM_COLS]),
order_tag_(VALUE), max_entries_(max_entries) {
}

SimilarityMatrices::~SimilarityMatrices() {
}

void
SimilarityMatrices::resize(size_t dim0, size_t dim1, size_t dim2) {
    try {
        boost::multi_array < double, 5 >::resize(
                boost::extents[dim0][dim1][dim2][max_entries_][_NUM_COLS]);
    } catch (bad_alloc & e) {
        ostringstream msg;
        msg << BOLDRED << "Insufficient memory for SimilarityMatrices["
                << dim0 << "][" << dim1 << "][" << dim2 << "]["
                << max_entries_ << "][" << _NUM_COLS << "]!" << RESET;
        throw runtime_error(msg.str());
    }
}

bool
SimilarityMatrices::sortRows(bool quick, size_t length, COL_TAG col_tag) {

    order_tag_ = col_tag;

    // Define variables for perfectly nested parallel loops with collapse
    auto limit_i = this->shape()[0];
    auto limit_j = this->shape()[1];
    auto limit_k = this->shape()[2];
    
    if (length == 0) length = this->shape()[3];

#if defined(_OPENMP)
#pragma omp parallel for default(none) schedule(static) \
collapse(3) shared(quick, length, limit_i, limit_j, limit_k)
#endif
    for (size_t i = 0; i < limit_i; i++) {
        for (size_t j = 0; j < limit_j; j++) {
            for (size_t k = 0; k < limit_k; k++) {
                if (quick) {
                    nth_element((*this)[i][j][k].begin(),
                            (*this)[i][j][k].begin() + length,
                            (*this)[i][j][k].end());
                } else {
                    partial_sort((*this)[i][j][k].begin(),
                            (*this)[i][j][k].begin() + length,
                            (*this)[i][j][k].end());
                }
            }
        }
    }

    return true;
}

void
SimilarityMatrices::setOrderTag(COL_TAG order_tag) {
    order_tag_ = order_tag;
}

void
SimilarityMatrices::setMaxEntries(size_t max_entries) {
    max_entries_ = max_entries;
}

int
SimilarityMatrices::getNumCols() {
    return _NUM_COLS;
}

size_t
SimilarityMatrices::getMaxEntries() {
    return max_entries_;
}

SimilarityMatrices::COL_TAG
SimilarityMatrices::getOrderTag() const {
    return order_tag_;
}

void
SimilarityMatrices::print(ostream& os) const {
    printSize(os);

    size_t O = shape()[3];
    size_t P = _NUM_COLS;

    for (size_t i = 0; i < shape()[0]; i++) {
        for (size_t j = 0; j < shape()[1]; j++) {
            for (size_t k = 0; k < shape()[2]; k++) {
                cout << "SimilarityMatrices[" << i << ", " << j
                        << ", " << k << ", , ]" << endl;

                for (size_t p = 0; p < P; p++) {
                    os << "\t[ ," << p << "]";
                }
                os << endl;

                for (size_t o = 0; o < O; o++) {
                    os << "[" << o << ", ]\t";
                    for (size_t p = 0; p < P; p++) {
                        os << (*this)[i][j][k][o][p] << "\t";
                    }
                    os << endl;
                }
                os << endl;
            }
        }
    }
}

void
SimilarityMatrices::printSize(ostream& os) const {
    os << "SimilarityMatrices shape = "
            << "[" << shape()[0] << "]"
            << "[" << shape()[1] << "]"
            << "[" << shape()[2] << "]"
            << "[" << shape()[3] << "]"
            << "[" << shape()[4] << "]" << endl;
}

ostream & operator<<(ostream & os,
        const SimilarityMatrices& bv) {
    bv.print(os);
    return os;
}
