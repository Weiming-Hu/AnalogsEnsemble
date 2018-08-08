/* 
 * File:   SimilarityMatrices.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 * 
 * Created on August 2, 2018, 8:32 PM
 */

#include "SimilarityMatrices.h"

#include <exception>
#include <algorithm>

#if defined(_OPENMP)
#include <omp.h>
#endif

using namespace std;
using TAG = SimilarityMatrix::COL_TAG;

/*****************************************************************************
 *                              SimilarityMatrix                             *
 *****************************************************************************/

SimilarityMatrix::SimilarityMatrix() {
    resize(0);
}

SimilarityMatrix::SimilarityMatrix(size_t nrows) {
    resize(nrows);
}

SimilarityMatrix::SimilarityMatrix(const SimilarityMatrix& orig) {
    *this = orig;
}

SimilarityMatrix::~SimilarityMatrix() {
}

void
SimilarityMatrix::resize(size_t nrows) {
    vector< vector< double > >::resize(nrows);
    for (auto & vec : * this) {
        vec.resize(NUM_COLS, 0);
    }
}

size_t
SimilarityMatrix::nrows() const {
    return size();
}

size_t
SimilarityMatrix::ncols() const {
    return NUM_COLS;
}

bool
SimilarityMatrix::sortRows(bool quick, size_t length, COL_TAG col_tag) {

    auto lambda = [col_tag](const vector<double> & lhs, const vector<double> & rhs) {
        return ( lhs[col_tag] < rhs[col_tag]);
    };

    if (quick) {
        if (length == 0) length = this->size();
        nth_element(this->begin(), this->begin() + length,
                this->end(), lambda);
    } else {
        sort(this->begin(), this->end(), lambda);
    }

    ordered_ = true;
    order_tag_ = col_tag;
    return true;
}

bool
SimilarityMatrix::isOrdered() {
    return (ordered_);
}

void 
SimilarityMatrix::setValues(vector<double> values) {
    
    if (values.size() % NUM_COLS != 0) 
        throw out_of_range("Error: The input vector has wrong length!");
    
    size_t nrows = values.size() / NUM_COLS;
    resize(nrows);
    
    size_t pos = 0;
    for (size_t i_col = 0; i_col < NUM_COLS; i_col++) {
        for (size_t i_row = 0; i_row < nrows; i_row++, pos++) {
            (*this)[i_row][i_col] = values[pos];
        }
    }
}

TAG
SimilarityMatrix::getOrderTag() {
    return (order_tag_);
}

void
SimilarityMatrix::print(ostream & os) const {

    printSize(os);
    if (ordered_) cout << "Ordered by the column: " << order_tag_ << endl;

    size_t O = size();
    size_t P = NUM_COLS;

    for (size_t p = 0; p < P; p++) {
        os << "\t[ ," << p << "]";
    }
    os << endl;

    for (size_t o = 0; o < O; o++) {
        os << "[" << o << ", ]\t";

        for (size_t p = 0; p < P; p++) {
            os << (*this)[o][p] << "\t";
        }
        os << endl;

    }
    os << endl;
}

void
SimilarityMatrix::printSize(ostream & os) const {
    os << "Similarity Matrix shape = "
            << "[" << size() << "]"
            << "[" << NUM_COLS << "]" << endl;
}

ostream & operator<<(ostream & os,
        const SimilarityMatrix& bv) {
    bv.print(os);
    return os;
}

SimilarityMatrix &
        SimilarityMatrix::operator=(const SimilarityMatrix& right) {
    if (this == &right) return *this;

    vector< std::vector< double > >::operator=(right);
    order_tag_ = right.order_tag_;
    ordered_ = right.ordered_;

    return *this;
}

/*****************************************************************************
 *                            SimilarityMatrices                             *
 *****************************************************************************/
SimilarityMatrices::SimilarityMatrices() {
}

SimilarityMatrices::SimilarityMatrices(const Forecasts& forecasts) {
    setTargets(forecasts);
}

SimilarityMatrices::~SimilarityMatrices() {
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

    resize(boost::extents[targets.getStationsSize()]
            [targets.getTimesSize()]
            [targets.getFLTsSize()]);
}

bool
SimilarityMatrices::sortRows(bool quick, size_t length,
        SimilarityMatrix::COL_TAG col_tag) {

#if defined(_OPENMP)
#pragma omp parallel for default(none) schedule(static) collapse(3) shared(this) 
#endif
    for (size_t i_station = 0; i_station < shape()[0]; i_station++) {
        for (size_t i_time = 0; i_time < shape()[1]; i_time++) {
            for (size_t i_flt = 0; i_flt < shape()[2]; i_flt++) {
                (*this)[i_station][i_time][i_flt].sortRows(
                        quick, length, col_tag);
            }
        }
    }
    
    return true;
}

const Forecasts_array &
SimilarityMatrices::getTargets() const {
    return (targets_);
}

void
SimilarityMatrices::print(ostream& os) const {
    cout << "Associated forecasts:" << endl
            << targets_.getStations()
            << targets_.getParameters()
            << targets_.getTimes()
            << targets_.getFLTs() << endl;
    printSize(os);
    for (size_t i = 0; i < shape()[0]; i++)
        for (size_t j = 0; j < shape()[1]; j++)
            for (size_t k = 0; k < shape()[2]; k++)
                cout << "SimilarityMatrices[" << i
                    << "][" << j << "][" << k << "]"
                    << endl << (*this)[i][j][k];
}

void
SimilarityMatrices::printSize(ostream& os) const {
    os << "Similarity Matrices shape = "
            << "[" << shape()[0] << "]"
            << "[" << shape()[1] << "]"
            << "[" << shape()[2] << "]" << endl;
}

ostream & operator<<(ostream & os,
        const SimilarityMatrices& bv) {
    bv.print(os);
    return os;
}