/* 
 * File:   ObservationsPointer.cpp
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <cervone@psu.edu>
 * 
 * Created on January 27, 2020, 5:39 PM
 */

#include "ObservationsPointer.h"

using namespace std;

const size_t ObservationsPointer::_DIM_PARAMETER = 0;
const size_t ObservationsPointer::_DIM_STATION = 1;
const size_t ObservationsPointer::_DIM_TIME = 2;

ObservationsPointer::ObservationsPointer() {
    data_ = nullptr;
    allocated_ = false;
}

ObservationsPointer::ObservationsPointer(const ObservationsPointer & orig) :
Observations(orig) {

    // Copy dimensions
    memcpy(dims_, orig.dims_, 3 * sizeof (size_t));

    // Copy values
    data_ = new double [orig.num_elements()];
    memcpy(data_, orig.data_, sizeof (double) * orig.num_elements());

    allocated_ = true;
}

ObservationsPointer::~ObservationsPointer() {
    if (allocated_) delete [] data_;
}

size_t ObservationsPointer::num_elements() const {
    return dims_[_DIM_PARAMETER] * dims_[_DIM_STATION] * dims_[_DIM_TIME];
}

const double* ObservationsPointer::getValuesPtr() const {
    return data_;
}

double* ObservationsPointer::getValuesPtr() {
    return data_;
}

void ObservationsPointer::setDimensions(
        const Parameters& parameters,
        const Stations& stations,
        const Times& times) {

    // Set members in the parent class
    parameters_ = parameters;
    stations_ = stations;
    times_ = times;

    // Set dimensions
    dims_[_DIM_PARAMETER] = parameters_.size();
    dims_[_DIM_STATION] = stations_.size();
    dims_[_DIM_TIME] = times_.size();

    // Allocate memory for underlying data structure
    if (allocated_) delete [] data_;
    data_ = new double [num_elements()];

    allocated_ = true;

    return;
}

double ObservationsPointer::getValue(
        size_t parameter_index, size_t station_index, size_t time_index) const {

    vector3 indices{parameter_index, station_index, time_index};
    return data_[toIndex(indices)];
}

void ObservationsPointer::setValue(double val,
        size_t parameter_index, size_t station_index, size_t time_index) {

    vector3 indices{parameter_index, station_index, time_index};
    data_[toIndex(indices)] = val;
    return;
}

size_t
ObservationsPointer::toIndex(const vector3 & indices) const {
    // Convert dimension indices to position offset by column-major
    return indices[_DIM_PARAMETER] + dims_[_DIM_PARAMETER] *
            (indices[_DIM_STATION] + dims_[_DIM_STATION] * indices[_DIM_TIME]);
}

void
ObservationsPointer::print(std::ostream & os) const {
    Observations::print(os);

    os << "[Data] size: " << num_elements() << std::endl;

    for (size_t m = 0; m < dims_[_DIM_PARAMETER]; ++m) {
        cout << "[" << m << ",,]" << endl;
        for (size_t p = 0; p < dims_[_DIM_TIME]; ++p) os << "\t[,," << p << "]";
        os << endl;

        for (size_t o = 0; o < dims_[_DIM_STATION]; ++o) {
            os << "[," << o << ",]\t";

            for (size_t p = 0; p < dims_[_DIM_TIME]; ++p) {
                os << getValue(m, o, p) << "\t";
            }

            os << endl;
        }

        os << endl;
    }

    return;
}

std::ostream &
operator<<(std::ostream & os, const ObservationsPointer & obj) {
    obj.print(os);
    return os;
}