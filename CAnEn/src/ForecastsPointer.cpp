/* 
 * File:   ForecastsPointer.h
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <weiming@psu.edu>
 *
 * Created on January 27, 2020, 5:39 PM
 */

#include "ForecastsPointer.h"
#include "Functions.h"

#include <stdexcept>
#include <algorithm>

#if defined(_OPENMP)
#include <omp.h>
#endif

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
    setMembers(parameters, stations, times);
    flts_ = flts;

    // Allocate data
    resize(parameters_.size(), stations_.size(), times_.size(), flts_.size());
    return;
}

void 
ForecastsPointer::windTransform(
        const string & name_u, const string & name_v,
        const string & name_spd, const string & name_dir) {

    // Convert from names to indices
    size_t u_index = parameters_.getIndex(name_u);
    size_t v_index = parameters_.getIndex(name_v);

    // Get existing names from parameters
    vector<string> names;
    parameters_.getNames(names);

    // Make sure the new names do not exist in parameters
    if (find(names.begin(), names.end(), name_spd) != names.end())
        throw runtime_error("New name for wind speed exists. Choose another name");
    if (find(names.begin(), names.end(), name_dir) != names.end())
        throw runtime_error("New name for wind direction exists. Choose another name");

    /* Because bidirectional map is immutable after creation for vector type.
     * I need to do a manual copy and substitute names for wind parameters.
     *
     * U ---> Wind speed
     * V ---> Wind direction
     */
    Parameters original = parameters_;
    parameters_.clear();

    for (size_t parameter_i = 0; parameter_i < original.size(); ++parameter_i) {
        Parameter parameter;
        if (parameter_i == u_index) {
            parameter.setName(name_spd);
        } else if (parameter_i == v_index) {
            parameter.setName(name_dir);
            parameter.setCircular(true);
        } else {
            parameter = original.getParameter(parameter_i);
        }

        parameters_.push_back(parameter);
    }

    /*
     * Carry out the actual calculation and replace values in data with 
     * wind speed and direction values.
     */
    size_t num_stations = stations_.size();
    size_t num_times = times_.size();
    size_t num_flts = flts_.size();

#if defined(_OPENMP)
#pragma omp parallel for default(none) schedule(static) collapse(3) \
shared(num_stations, num_times, num_flts, u_index, v_index)
#endif
    for (size_t station_i = 0; station_i < num_stations; ++station_i) {
        for (size_t time_i = 0; time_i < num_times; ++time_i) {
            for (size_t flt_i = 0; flt_i < num_flts; ++flt_i) {

                // Get values
                double u = getValue(u_index, station_i, time_i, flt_i);
                double v = getValue(v_index, station_i, time_i, flt_i);

                // Calculate and assign new values
                setValue(Functions::wind_speed(u, v), u_index, station_i, time_i, flt_i);
                setValue(Functions::wind_dir(u, v), v_index, station_i, time_i, flt_i);
            }
        }
    }

    return;
}

void
ForecastsPointer::subset(Forecasts& forecasts_subset) const {

    // Get dimension variables
    const Parameters & parameters_subset = forecasts_subset.getParameters();
    const Stations & stations_subset = forecasts_subset.getStations();
    const Times & times_subset = forecasts_subset.getTimes();
    const Times & flts_subset = forecasts_subset.getFLTs();

    subset_data_(parameters_subset, stations_subset, times_subset, flts_subset, forecasts_subset);
    return;
}

void
ForecastsPointer::subset(const Parameters & parameters, const Stations & stations, const Times& times, const Times& flts,
        Forecasts & forecasts_subset) const {

    // Allocate memory
    forecasts_subset.setDimensions(parameters, stations, times, flts);

    subset_data_(parameters, stations, times, flts, forecasts_subset);
    return;
}

void
ForecastsPointer::print(ostream & os) const {
    Forecasts::print(os);

    //    const size_t* dims = shape();

    os << "[Data] size: " << num_elements() << endl;

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

ostream &
operator<<(ostream & os, const ForecastsPointer & obj) {
    obj.print(os);
    return os;
}

void
ForecastsPointer::subset_data_(const Parameters & parameters, const Stations & stations, const Times & times, const Times & flts,
        Forecasts & forecasts_subset) const {

    // Get the indices for dimensions to subset
    vector<size_t> parameters_index, stations_index, times_index, flts_index;

    parameters_.getIndices(parameters, parameters_index);
    stations_.getIndices(stations, stations_index);
    times_.getIndices(times, times_index);
    flts_.getIndices(flts, flts_index);

    // Copy values
    Array4DPointer::subset(
            parameters_index, stations_index,
            times_index, flts_index, forecasts_subset);

    return;
}
