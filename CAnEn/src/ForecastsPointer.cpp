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

ForecastsPointer::ForecastsPointer() : Forecasts(), Array4DPointer() {
}

ForecastsPointer::ForecastsPointer(const ForecastsPointer& orig) :
Forecasts(orig), Array4DPointer(orig) {
}

ForecastsPointer::ForecastsPointer(
        const Parameters & parameters, const Stations & stations,
        const Times & times, const Times & flts) :
Forecasts(parameters, stations, times, flts),
Array4DPointer(parameters.size(), stations.size(), times.size(), flts.size()) {
}

ForecastsPointer::~ForecastsPointer() {
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
    resize(parameters_.size(), stations_.size(), times_.size(), flts_.size());
    return;
}

void
ForecastsPointer::subset(Forecasts& forecasts_subset) const {

    // Get dimension variables
    const Parameters & parameters_subset = forecasts_subset.getParameters();
    const Stations & stations_subset = forecasts_subset.getStations();
    const Times & times_subset = forecasts_subset.getTimes();
    const Times & flts_subset = forecasts_subset.getFLTs();

    // Get the indices for dimensions to subset
    vector<size_t> parameters_index, stations_index, times_index, flts_index;

    parameters_.getIndices(parameters_subset, parameters_index);
    stations_.getIndices(stations_subset, stations_index);
    times_.getIndices(times_subset, times_index);
    flts_.getIndices(flts_subset, flts_index);

    // Copy values
    Array4DPointer::subset(
            parameters_index, stations_index,
            times_index, flts_index, forecasts_subset);
    
    return;
}

void
ForecastsPointer::print(std::ostream & os) const {
    Forecasts::print(os);

    //    const size_t* dims = shape();

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