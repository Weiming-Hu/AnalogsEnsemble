/* 
 * File:   Observations.cpp
 * Author: Weiming Hu (weiming@psu.edu)
 * 
 * Created on May 4, 2018, 11:27 AM
 */

#include "ObservationsArray.h"
#include "colorTexts.h"
#include "Functions.h"

using namespace std;

ObservationsArray::ObservationsArray() {
    data_ = boost::multi_array<double, 3> (
            boost::extents[0][0][0],
            boost::fortran_storage_order());
}

ObservationsArray::ObservationsArray(Parameters parameters,
        Stations stations, Times times) :
Observations(parameters, stations, times) {
    data_ = boost::multi_array<double, 3> (
            boost::extents[0][0][0],
            boost::fortran_storage_order());
    updateDataDims_();
}


ObservationsArray::~ObservationsArray() {
}

size_t
ObservationsArray::size() const {
    return (data_.size());
}

const double*
ObservationsArray::getValuesPtr() const {
    return( data_.data());
}

double*
ObservationsArray::getValuesPtr() {
    return( data_.data());
}

void
ObservationsArray::setDimensions(const Parameters & parameters,
        const Stations & stations,
        const Times & times) {

    parameters_ = parameters;
    stations_ = stations;
    times_ = times;

    // Update dimensions and allocation memory
    updateDataDims_(true);
}

double
ObservationsArray::getValue(size_t parameter_index,
        size_t station_index, size_t time_index) const {
    return (data_[parameter_index][station_index][time_index]);
}


void
ObservationsArray::setValue(double val, size_t parameter_index,
        size_t station_index, size_t time_index) {
    data_[parameter_index][station_index][time_index] = val;
}


void
ObservationsArray::updateDataDims_(bool initialize_values) {

    try {
        data_.resize(boost::extents
                [parameters_.size()][stations_.size()][times_.size()]);
    } catch (bad_alloc & e) {
        cerr << BOLDRED << "Error: insufficient memory while resizing the"
                << " array4D to hold " << parameters_.size() << "x"
                << stations_.size() << "x" << times_.size()
                << " double values." << endl;
        throw;
    }
    
    if (initialize_values)
        fill_n(data_.data(), data_.num_elements(), NAN);
}

void
ObservationsArray::printShape(std::ostream & os) const {
    Observations::print(os);
    os << "ObservationsArray Shape = ";
    for (size_t i = 0; i < 3; i++) os << "[" << data_.shape()[i] << "]";
    os << endl;
    return;
}

void ObservationsArray::print(std::ostream & os) const {
    printShape(os);
    Functions::print(os, data_);
    return;
}


ostream &
operator<<(ostream& os, const ObservationsArray& obj) {
    obj.print(os);
    return os;
}
