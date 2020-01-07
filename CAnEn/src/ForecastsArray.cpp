/* 
 * File:   ForecastsArray.cpp
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <cervone@psu.edu>
 * 
 * Created on January 2, 2020, 5:55 PM
 */

#include "ForecastsArray.h"
#include "colorTexts.h"
#include "Functions.h"

using namespace std;

ForecastsArray::ForecastsArray() {
    data_ = boost::multi_array<double, 4> (
            boost::extents[0][0][0][0],
            boost::fortran_storage_order());
}

ForecastsArray::ForecastsArray(const Parameters & parameters,
        const Stations & stations, const Times & times, const Times & flts) :
Forecasts(parameters, stations, times, flts) {
    data_ = boost::multi_array<double, 4> (
            boost::extents[0][0][0][0],
            boost::fortran_storage_order());
}

ForecastsArray::ForecastsArray(const ForecastsArray & orig) : Forecasts(orig) {
    if (this != &orig) {
        data_ = orig.data_;
    }
}

ForecastsArray::~ForecastsArray() {
}

size_t
ForecastsArray::size() const {
    return (data_.size());
}

const double*
ForecastsArray::getValuesPtr() const {
    return( data_.data());
}

double*
ForecastsArray::getValuesPtr() {
    return( data_.data());
}

void
ForecastsArray::setDimensions(const Parameters & parameters,
        const Stations & stations,
        const Times & times,
        const Times & flts) {

    parameters_ = parameters;
    stations_ = stations;
    times_ = times;
    flts_ = flts;

    // Update dimensions and allocation memory
    updateDataDims_(true);
}



void
ForecastsArray::updateDataDims_(bool initialize_values) {
    try {
         data_.resize(boost::extents
                [parameters_.size()][stations_.size()][times_.size()][flts_.size()]);
    } catch (bad_alloc & e) {
        cerr << BOLDRED << "Error: insufficient memory while resizing the"
                << " array4D to hold " << getParameters().size() << "x"
                << getFLTs().size() << "x" << getStations().size() << "x"
                << getTimes().size() << " double values." << endl;
        throw;
    }

    if (initialize_values)
        fill_n(data_.data(), data_.num_elements(), NAN);

    return;
}

double
ForecastsArray::getValue(size_t parameter_index, size_t station_index,
        size_t time_index, size_t flt_index) const {
    return (data_[parameter_index][station_index][time_index][flt_index]);
}


void
ForecastsArray::setValue(double val, size_t parameter_index,
        size_t station_index, size_t time_index, size_t flt_index) {
    data_[parameter_index][station_index][time_index][flt_index] = val;
}

void ForecastsArray::printShape(std::ostream & os) const {
    Forecasts::print(os);
    os << "ForecastsArray Shape = ";
    for (size_t i = 0; i < 4; i++) os << "[" << data_.shape()[i] << "]";
    os << endl;
    return;
}

void ForecastsArray::print(std::ostream & os) const {
    printShape(os);
    Functions::print(os, data_);
    return;
}

ostream &
operator<<(ostream & os, const ForecastsArray& obj) {
    obj.print(os);
    return os;
}