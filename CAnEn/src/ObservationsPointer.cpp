/* 
 * File:   ObservationsPointer.cpp
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <weiming@psu.edu>
 * 
 * Created on January 27, 2020, 5:39 PM
 */

#include "ObservationsPointer.h"
#include <stdexcept>

using namespace std;

const size_t ObservationsPointer::_DIM_PARAMETER = 0;
const size_t ObservationsPointer::_DIM_STATION = 1;
const size_t ObservationsPointer::_DIM_TIME = 2;

ObservationsPointer::ObservationsPointer() : Observations() {
    fill_n(dims_, 3, 0);
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

ObservationsPointer::ObservationsPointer(
        const Parameters & parameters, const Stations & stations, const Times & times) :
Observations(parameters, stations, times), allocated_(false) {
    allocateMemory_();
}

ObservationsPointer::~ObservationsPointer() {
    if (allocated_) delete [] data_;
}

size_t
ObservationsPointer::num_elements() const {
    return dims_[_DIM_PARAMETER] * dims_[_DIM_STATION] * dims_[_DIM_TIME];
}

void
ObservationsPointer::initialize(double value) {
    if (data_ != nullptr) fill_n(data_, num_elements(), value);
}

const double*
ObservationsPointer::getValuesPtr() const {
    return data_;
}

double*
ObservationsPointer::getValuesPtr() {
    return data_;
}

void
ObservationsPointer::setDimensions(
        const Parameters& parameters,
        const Stations& stations,
        const Times& times) {

    // Set members in the parent class
    setMembers(parameters, stations, times);

    // Allocate memory
    allocateMemory_();

    return;
}

double
ObservationsPointer::getValue(
        size_t parameter_index, size_t station_index, size_t time_index) const {
    return data_[toIndex_(parameter_index, station_index, time_index)];
}

void
ObservationsPointer::setValue(double val,
        size_t parameter_index, size_t station_index, size_t time_index) {

    data_[toIndex_(parameter_index, station_index, time_index)] = val;
    return;
}

void
ObservationsPointer::subset(Observations & observations_subset) const {

    // Get dimension variables
    const Parameters & parameters_subset = observations_subset.getParameters();
    const Stations & stations_subset = observations_subset.getStations();
    const Times & times_subset = observations_subset.getTimes();

    // Copy values
    subset_data_(parameters_subset, stations_subset, times_subset, observations_subset);

    return;
}

void
ObservationsPointer::subset(const Parameters & parameters, const Stations & stations, const Times & times,
        Observations & observations_subset) const {

    // Alocate memory and set dimensions
    observations_subset.setDimensions(parameters, stations, times);

    subset_data_(parameters, stations, times, observations_subset);
    return;
}

void
ObservationsPointer::print(std::ostream & os) const {
    Observations::print(os);

    os << "[Data] shape [" << dims_[_DIM_PARAMETER] << "," << dims_[_DIM_STATION]
            << "," << dims_[_DIM_TIME] << "]" << std::endl;

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

size_t
ObservationsPointer::toIndex_(size_t dim0, size_t dim1, size_t dim2) const {
    // Convert dimension indices to position offset by column-major
    return dim0 + dims_[_DIM_PARAMETER] * (dim1 + dims_[_DIM_STATION] * dim2);
}

void
ObservationsPointer::allocateMemory_() {

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

void
ObservationsPointer::subset_data_(const Parameters & parameters, const Stations & stations, const Times & times,
        Observations & observations_subset) const {

    // Get the indices for dimensions to subset
    vector<size_t> parameters_index, stations_index, times_index;

    parameters_.getIndices(parameters, parameters_index);
    stations_.getIndices(stations, stations_index);
    times_.getIndices(times, times_index);

    parameters_.getIndices(parameters, parameters_index);
    stations_.getIndices(stations, stations_index);
    times_.getIndices(times, times_index);

    // Copy values
    double * values_ptr = observations_subset.getValuesPtr();
    subset_data_(parameters_index, stations_index, times_index, values_ptr);

    return;
}

void ObservationsPointer::subset_data_(
        const vector<size_t> & dim0_indices,
        const vector<size_t> & dim1_indices,
        const vector<size_t> & dim2_indices,
        double * p_subset) const {

    // Make sure the indices are sorted
    bool sorted = is_sorted(dim0_indices.begin(), dim0_indices.end()) &&
            is_sorted(dim1_indices.begin(), dim1_indices.end()) &&
            is_sorted(dim2_indices.begin(), dim2_indices.end());
    if (!sorted) throw runtime_error("Subset indices must be sorted in ascension order");

    // Get pointers to data
    const double * p_ori = getValuesPtr();

    size_t from_pos, offset;
    size_t last_pos = 0;

    // Loop through the data in column-major
    for (size_t dim2_i : dim2_indices)
        for (size_t dim1_i : dim1_indices)
            for (size_t dim0_i : dim0_indices) {

                // Calculate position offset
                from_pos = toIndex_(dim0_i, dim1_i, dim2_i);
                offset = from_pos - last_pos;

                // Advance the original data pointer
                p_ori = p_ori + offset;

                // Copy this value
                *p_subset = *p_ori;

                // Advance the subset data pointer
                p_subset++;

                // Update last position
                last_pos = from_pos;
            }

    return;
}
