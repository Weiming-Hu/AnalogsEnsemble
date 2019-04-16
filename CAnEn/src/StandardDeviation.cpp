/* 
 * File:   StandardDeviation.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 * 
 * Created on August 3, 2018, 3:32 PM
 */

#include "StandardDeviation.h"


using namespace std;

StandardDeviation::StandardDeviation() :
boost::multi_array<double, 3>(
boost::extents[0][0][0],
boost::fortran_storage_order()) {
}

StandardDeviation::StandardDeviation(size_t dim1, size_t dim2, size_t dim3) :
boost::multi_array<double, 3>(
boost::extents[0][0][0],
boost::fortran_storage_order()) {
    StandardDeviation::extent_gen extents;
    resize(extents[dim1][dim2][dim3]);
}

StandardDeviation::StandardDeviation(const StandardDeviation& orig) {
    *this = orig;
}

StandardDeviation::~StandardDeviation(){
}

StandardDeviation&
        StandardDeviation::operator=(const StandardDeviation& right) {

    if (this == &right) return *this;
    
    boost::multi_array<double, 3>::operator=(right);
    
    return *this;
}

void
StandardDeviation::print(ostream& os) const {
    printSize(os);
    os << endl;

    size_t M = shape()[0];
    size_t O = shape()[1];
    size_t P = shape()[2];


    for (size_t m = 0; m < M; m++) {
        os << "[ " << m << ", , ]" << endl;

        for (size_t p = 0; p < P; p++) {
            os << "\t[, ," << p << "]";
        }
        os << endl;

        for (size_t o = 0; o < O; o++) {
            os << "[, " << o << ", ]\t";

            for (size_t p = 0; p < P; p++) {
                os << (*this)[m][o][p] << "\t";
            }
            os << endl;

        }
        os << endl;
    }
    os << endl;
}

void
StandardDeviation::printSize(ostream& os) const {
    os << "Standard Deviation Array shape = ";
    for (size_t i = 0; i < 3; i++) {
        os << "[" << shape()[i] << "]";
    }
    os << endl;
}

ostream &
operator<<(ostream & os, const StandardDeviation & bv) {
    bv.print(os);
    return os;
}

