/* 
 * File:   SimilarityMatrices.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 * 
 * Created on August 2, 2018, 8:32 PM
 */

#include "SimilarityMatrices.h"
#include "colorTexts.h"

#include <exception>
#include <algorithm>
#include <exception>
#include <iostream>

#if defined(_OPENMP)
#include <omp.h>
#endif

using namespace std;
using COL_TAG = SimilarityMatrices::COL_TAG;

// Overload swap function for sortRows
//
// Ref:
// http://coliru.stacked-crooked.com/a/bfbb5a98995cc2a4
// https://stackoverflow.com/questions/36117142/sorting-boosts-multi-array-using-sort-function-and-a-recursive-comparator
//
namespace boost {
    namespace detail {
        namespace multi_array {

            template < typename T, size_t dims >
            static void swap(sub_array < T, dims > lhs,
                    sub_array < T, dims > rhs) {
                using std::swap;
                for (auto ai = lhs.begin(), bi = rhs.begin();
                        ai != lhs.end() && bi != rhs.end(); ++ai, ++bi) {
                    swap(*ai, *bi);
                }
            }
        }
    }
}

SimilarityMatrices::SimilarityMatrices(size_t max_entries) :
max_entries_(max_entries) {
}

SimilarityMatrices::SimilarityMatrices(
        const Forecasts& forecasts, size_t max_entries) {
    max_entries_ = max_entries;
    setTargets(forecasts);
}

SimilarityMatrices::SimilarityMatrices(const size_t& num_stations,
        const size_t& num_times, const size_t& num_flts,
        const size_t& max_entries) :
boost::multi_array<double, 5>(boost::extents[num_stations][num_times]
[num_flts][max_entries][_NUM_COLS_]), max_entries_(max_entries) {
}

SimilarityMatrices::~SimilarityMatrices() {
}

void
SimilarityMatrices::resize() {
    try {
        boost::multi_array < double, 5 >::resize(
                boost::extents[targets_.getStationsSize()][targets_.getTimesSize()]
                [targets_.getFLTsSize()][max_entries_][_NUM_COLS_]);
    } catch (bad_alloc & e) {
        cerr << "ERROR: Insufficient memory to resize similarity matrix to store "
            << targets_.getStationsSize() * targets_.getTimesSize() * targets_.getFLTsSize() * max_entries_ * _NUM_COLS_
            << " double values!" << endl;
        throw e;
    }
}

void
SimilarityMatrices::resize(size_t dim0, size_t dim1, size_t dim2) {
    try {
        boost::multi_array < double, 5 >::resize(
                boost::extents[dim0][dim1][dim2][max_entries_][_NUM_COLS_]);
    } catch (bad_alloc & e) {
        cerr << "ERROR: Insufficient memory to resize similarity matrix to store "
            << dim0 * dim1 * dim2 * max_entries_ * _NUM_COLS_ << " double values!" << endl;
        throw e;
    }
}

void
SimilarityMatrices::setTargets(const Forecasts & targets) {

    targets_.setParameters(targets.getParameters());
    targets_.setStations(targets.getStations());
    targets_.setTimes(targets.getTimes());
    targets_.setFlts(targets.getFLTs());
    targets_.updateDataDims();

    auto p_vals = targets.getValues();
    vector<double> vec(p_vals, p_vals + targets.getDataLength());
    targets_.setValues(vec);

    // Resize the multi array values
    resize(targets.getStationsSize(),
            targets.getTimesSize(), targets.getFLTsSize());
}

bool
SimilarityMatrices::sortRows(bool quick, size_t length, COL_TAG col_tag) {

    matrixSort sortFunc;
    sortFunc.order_tag = col_tag;

    // Define variables for perfectly nexted parallel loops with collapse
    auto limit_i = this->shape()[0];
    auto limit_j = this->shape()[1];
    auto limit_k = this->shape()[2];


    if (quick) {
        if (length == 0) {
            length = this->size();
            cout << RED << "Warning: Length can't be 0 for quick sort."
                    << " It has been set to the number of entries."
                    << RESET << endl;
        }

#if defined(_OPENMP)
#pragma omp parallel for default(none) schedule(static) \
collapse(3) shared(sortFunc, length, limit_i, limit_j, limit_k)
#endif
        for (size_t i = 0; i < limit_i; i++) {
            for (size_t j = 0; j < limit_j; j++) {
                for (size_t k = 0; k < limit_k; k++) {
                    nth_element((*this)[i][j][k].begin(),
                            (*this)[i][j][k].begin() + length,
                            (*this)[i][j][k].end(), sortFunc);
                }
            }
        }

    } else {
#if defined(_OPENMP)
#pragma omp parallel for default(none) schedule(static) \
collapse(3) shared(sortFunc, limit_i, limit_j, limit_k)
#endif
        for (size_t i = 0; i < limit_i; i++) {
            for (size_t j = 0; j < limit_j; j++) {
                for (size_t k = 0; k < limit_k; k++) {
                    sort((*this)[i][j][k].begin(),
                            (*this)[i][j][k].end(), sortFunc);
                }
            }
        }
    }

    order_tag_ = col_tag;
    return true;
}

const Forecasts_array &
SimilarityMatrices::getTargets() const {
    return (targets_);
}

void
SimilarityMatrices::setOrderTag(COL_TAG order_tag) {
    if (order_tag >= STATION || order_tag <= VALUE) order_tag_ = order_tag;
    else throw range_error("Error: Invalid order tag");
}

void
SimilarityMatrices::setMaxEntries(size_t max_entries) {
    max_entries_ = max_entries;
}

int
SimilarityMatrices::getNumCols() {
    return _NUM_COLS_;
}

size_t
SimilarityMatrices::getMaxEntries() {
    return max_entries_;
}

COL_TAG
SimilarityMatrices::getOrderTag() const {
    return order_tag_;
}

void
SimilarityMatrices::print(ostream& os) const {
    //    cout << "Associated forecasts:" << endl
    //            << targets_.getStations()
    //            << targets_.getParameters()
    //            << targets_.getTimes()
    //            << targets_.getFLTs() << endl;
    printSize(os);

    size_t O = shape()[3];
    size_t P = _NUM_COLS_;

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
