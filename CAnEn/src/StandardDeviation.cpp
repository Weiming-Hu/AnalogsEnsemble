/* 
 * File:   StandardDeviation.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 * 
 * Created on August 3, 2018, 3:32 PM
 */

#include "StandardDeviation.h"
#include "Functions.h"

using namespace std;

StandardDeviation::StandardDeviation() :
boost::multi_array<double, 3>(
boost::extents[0][0][0],
boost::fortran_storage_order()) {
}

StandardDeviation::StandardDeviation(const StandardDeviation& orig) :
boost::multi_array<double, 3>(
boost::extents[0][0][0], boost::fortran_storage_order()) {
    *this = orig;
}

StandardDeviation::StandardDeviation(size_t dim1, size_t dim2, size_t dim3) :
boost::multi_array<double, 3>(
boost::extents[0][0][0], boost::fortran_storage_order()) {
    StandardDeviation::extent_gen extents;
    resize(extents[dim1][dim2][dim3]);
}

StandardDeviation::~StandardDeviation(){
}

StandardDeviation&
        StandardDeviation::operator=(const StandardDeviation& right) {
    if (this != &right) boost::multi_array<double, 3>::operator=(right);
    return *this;
}

void
StandardDeviation::printShape(std::ostream & os) const {
    os << "StandardDeviation Array shape = ";
    for (size_t i = 0; i < 3; i++) os << "[" << shape()[i] << "]";
    os << endl;
    return;
}

ostream &
operator<<(ostream & os, const StandardDeviation & obj) {
    obj.printShape(os);
    Functions::print(os, obj);
    return os;
}