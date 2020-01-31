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
}

ForecastsPointer::ForecastsPointer(const ForecastsPointer& orig) :
Forecasts(orig) {
    data_ = orig.data_;
}

ForecastsPointer::ForecastsPointer(
        const Parameters & parameters, const Stations & stations,
        const Times & times, const Times & flts) :
Forecasts(parameters, stations, times, flts) {
    data_.resize(parameters_.size(), stations_.size(), times_.size(), flts_.size());
}

ForecastsPointer::~ForecastsPointer() {
}

size_t
ForecastsPointer::num_elements() const {
    return data_.num_elements();
}

const double*
ForecastsPointer::getValuesPtr() const {
    return data_.getValuesPtr();
}

double*
ForecastsPointer::getValuesPtr() {
    return data_.getValuesPtr();
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
    data_.resize(parameters_.size(), stations_.size(), times_.size(), flts_.size());
    return;
}

double
ForecastsPointer::getValue(
        size_t parameter_index, size_t station_index,
        size_t time_index, size_t flt_index) const {
    return data_.getValue(parameter_index, station_index, time_index, flt_index);
}

void ForecastsPointer::setValue(double val,
        size_t parameter_index, size_t station_index,
        size_t time_index, size_t flt_index) {
    data_.setValue(val, parameter_index, station_index, time_index, flt_index);
    return;
}

void
ForecastsPointer::print(std::ostream & os) const {
    Forecasts::print(os);
    
    const size_t* dims = data_.shape();

    os << "[Data] size: " << data_.num_elements() << std::endl;

    for (size_t l = 0; l < dims[_DIM_PARAMETER]; ++l) {
        for (size_t m = 0; m < dims[_DIM_STATION]; ++m) {
            cout << "[" << l << "," << m << ",,]" << endl;

            for (size_t p = 0; p < dims[_DIM_FLT]; ++p) os << "\t[,,," << p << "]";
            os << endl;

            for (size_t o = 0; o < dims[_DIM_TIME]; ++o) {
                os << "[,," << o << ",]\t";

                for (size_t p = 0; p < dims[_DIM_FLT]; ++p) {
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