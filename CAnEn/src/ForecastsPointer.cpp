/* 
 * File:   ForecastsPointer.h
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <cervone@psu.edu>
 *
 * Created on January 27, 2020, 5:39 PM
 */

#include "ForecastsPointer.h"

using namespace std;

const size_t ForecastsPointer::_DIM_PARAMETER = 0;
const size_t ForecastsPointer::_DIM_STATION = 1;
const size_t ForecastsPointer::_DIM_TIME = 2;
const size_t ForecastsPointer::_DIM_FLT = 3;

ForecastsPointer::ForecastsPointer() {
    data_ = nullptr;
    allocated_ = false;
}

ForecastsPointer::ForecastsPointer(const ForecastsPointer& orig) :
Forecasts(orig) {

    // Copy dimensions
    memcpy(dims_, orig.dims_, 4 * sizeof (size_t));

    // Copy values
    data_ = new double [orig.num_elements()];
    memcpy(data_, orig.data_, sizeof (double) * orig.num_elements());

    allocated_ = true;
}

ForecastsPointer::ForecastsPointer(
        const Parameters & parameters, const Stations & stations,
        const Times & times, const Times & flts) :
Forecasts(parameters, stations, times, flts), allocated_(false) {
    allocateMemory_();
}

ForecastsPointer::~ForecastsPointer() {
    if (allocated_) delete [] data_;
}

size_t
ForecastsPointer::num_elements() const {
    return dims_[_DIM_PARAMETER] * dims_[_DIM_STATION] * dims_[_DIM_TIME] * dims_[_DIM_FLT];
}

const double*
ForecastsPointer::getValuesPtr() const {
    return data_;
}

double*
ForecastsPointer::getValuesPtr() {
    return data_;
}

void
ForecastsPointer::setDimensions(
        const Parameters & parameters, const Stations & stations,
        const Times & times, const Times & flts) {

    // Set members in the parent class
    parameters_ = parameters;
    stations_ = stations;
    times_ = times;
    flts_ = flts;
    
    // Allocate data
    allocateMemory_();
    return;
}

double
ForecastsPointer::getValue(
        size_t parameter_index, size_t station_index,
        size_t time_index, size_t flt_index) const {

    vector4 indices{parameter_index, station_index, time_index, flt_index};
    return data_[toIndex(indices)];
}

void ForecastsPointer::setValue(double val,
        size_t parameter_index, size_t station_index,
        size_t time_index, size_t flt_index) {

    vector4 indices{parameter_index, station_index, time_index, flt_index};
    data_[toIndex(indices)] = val;
    return;
}

size_t
ForecastsPointer::toIndex(const vector4 & indices) const {
    // Convert dimension indices to position offset by column-major
    return indices[_DIM_PARAMETER] + dims_[_DIM_PARAMETER] *
            (indices[_DIM_STATION] + dims_[_DIM_STATION] *
            (indices[_DIM_TIME] + dims_[_DIM_TIME] * indices[_DIM_FLT]));
}

void
ForecastsPointer::allocateMemory_() {

    // Set dimensions
    dims_[_DIM_PARAMETER] = parameters_.size();
    dims_[_DIM_STATION] = stations_.size();
    dims_[_DIM_TIME] = times_.size();
    dims_[_DIM_FLT] = flts_.size();

    // Allocate memory for underlying data structure
    if (allocated_) delete [] data_;
    data_ = new double [num_elements()];

    allocated_ = true;
    return;
}

void
ForecastsPointer::print(std::ostream & os) const {
    Forecasts::print(os);

    os << "[Data] size: " << num_elements() << std::endl;

    for (size_t l = 0; l < dims_[_DIM_PARAMETER]; ++l) {
        for (size_t m = 0; m < dims_[_DIM_STATION]; ++m) {
            cout << "[" << l << "," << m << ",,]" << endl;

            for (size_t p = 0; p < dims_[_DIM_FLT]; ++p) os << "\t[,,," << p << "]";
            os << endl;

            for (size_t o = 0; o < dims_[_DIM_TIME]; ++o) {
                os << "[,," << o << ",]\t";

                for (size_t p = 0; p < dims_[_DIM_FLT]; ++p) {
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
operator<<(std::ostream & os, const ForecastsPointer & obj) {
    obj.print(os);
    return os;
}