
/* 
 * File:   ObservationsPointer.cpp
 * Author: guido
 * 
 * Created on January 27, 2020, 5:39 PM
 */

#include "ObservationsPointer.h"
#include "FunctionsR.h"
#include "Functions.h"
#include "AnEnDefaults.h"
#include "boost/numeric/conversion/cast.hpp"

#include <sstream>
#include <stdexcept>

using namespace Rcpp;
using namespace boost;

ObservationsPointer::ObservationsPointer() {
    data_ = nullptr;
    allocated_ = false;
}

ObservationsPointer::ObservationsPointer(const ObservationsPointer & orig) : Observations(orig) {
    memcpy(dims_, orig.dims_, 3 * sizeof(int));
//    dims_[0] = orig.dims_[0];
//    dims_[1] = orig.dims_[1];
//    dims_[2] = orig.dims_[2];

    data_ = new double [orig.num_elements()];
    memcpy(data_, orig.data_, sizeof (double) * orig.num_elements());

    allocated_ = true;
}

ObservationsPointer::~ObservationsPointer() {
    if (allocated_) delete [] data_;
}

size_t ObservationsPointer::num_elements() const {
    return dims_[0] * dims_[1] * dims_[2]; 
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

    dims_[0] = parameters_.size();
    dims_[1] = stations_.size();
    dims_[2] = times_.size();

    // Allocate memory for underlying data structure
    // using the C++ idioms
    //
    size_t size = num_elements();

    if (allocated_) delete [] data_;
    data_ = new double [size];
    allocated_ = true;


    return;
}

double ObservationsPointer::getValue(
        size_t parameter_index, size_t station_index, size_t time_index) const {

    vector3 indices = {parameter_index, station_index, time_index};
    return data_[toIndex(indices)];
}

void ObservationsPointer::setValue(double val,
        size_t parameter_index, size_t station_index, size_t time_index) {

    vector3 indices = {parameter_index, station_index, time_index};
    data_[toIndex(indices)] = val;
    return;
}

size_t
ObservationsPointer::toIndex(const vector3 & indices) const {
      // Convert dimension indices to position offset by column-major
    return indices[0] + dims_[0] * indices[1] + dims_[0] * dims_[1] * indices[2];

}



void
ObservationsPointer::print(std::ostream & os) const {
    Observations::print(os);

    size_t count = num_elements();
    os << "[Data] size: " << count << std::endl;

    if (count > AnEnDefaults::_PREVIEW_COUNT) {
        os << Functions::format(data_, AnEnDefaults::_PREVIEW_COUNT, ",") << ", ...";
    } else {
        os << Functions::format(data_, count, ",");
    }
    os << std::endl;

    return;
}

std::ostream &
operator<<(std::ostream & os, const ObservationsPointer & obj) {
    obj.print(os);
    return os;
}