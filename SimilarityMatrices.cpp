/* 
 * File:   SimilarityMatrices.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 * 
 * Created on August 2, 2018, 8:32 PM
 */

#include "SimilarityMatrices.h"

#include <exception>

using namespace std;
using TAG = SimilarityMatrix::TAG;

/*****************************************************************************
 *                              SimilarityMatrix                             *
 *****************************************************************************/

SimilarityMatrix::SimilarityMatrix() {
    resize(0, 3);
}

SimilarityMatrix::SimilarityMatrix(size_t nrows) {
    resize(nrows, 3);
}

SimilarityMatrix::SimilarityMatrix(size_t nrows, size_t ncols) {
    resize(nrows, ncols);
}

SimilarityMatrix::SimilarityMatrix(const SimilarityMatrix& orig) {
    *this = orig;
}

SimilarityMatrix::~SimilarityMatrix() {
}

void
SimilarityMatrix::setOrderTag(TAG tag) {
    order_tag_ = tag;
    ordered_ = true;
}

bool
SimilarityMatrix::isOrdered() {
    return (ordered_);
}

TAG
SimilarityMatrix::getOrderTag() {
    return (order_tag_);
}

void
SimilarityMatrix::print(ostream & os) const {
    
    printSize(os);
    if (ordered_) cout << "Ordered by the column: " << order_tag_ << endl;

    size_t O = size1();
    size_t P = size2();

    for (size_t p = 0; p < P; p++) {
        os << "\t[ ," << p << "]";
    }
    os << endl;

    for (size_t o = 0; o < O; o++) {
        os << "[" << o << ", ]\t";

        for (size_t p = 0; p < P; p++) {
            os << (*this)(o, p) << "\t";
        }
        os << endl;

    }
    os << endl;
}

void
SimilarityMatrix::printSize(ostream & os) const {
    os << "Similarity Matrix shape = "
            << "[" << size1() << "]"
            << "[" << size2() << "]" << endl;
}

ostream & operator<<(ostream & os,
        const SimilarityMatrix& bv) {
    bv.print(os);
    return os;
}

SimilarityMatrix &
        SimilarityMatrix::operator=(const SimilarityMatrix& right) {
    if (this == &right) return *this;

    boost::numeric::ublas::matrix<double>::operator=(right);
    order_tag_ = right.order_tag_;
    ordered_ = right.ordered_;

    return *this;
}

/*****************************************************************************
 *                            SimilarityMatrice                              *
 *****************************************************************************/
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