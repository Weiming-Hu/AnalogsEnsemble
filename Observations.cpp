/* 
 * File:   Observations.cpp
 * Author: Weiming Hu (weiming@psu.edu)
 * 
 * Created on May 4, 2018, 11:27 AM
 */

#include "Observations.h"
#include "colorTexts.h"

#include <stdexcept>
#include <algorithm>

using namespace std;

/*******************************************************************************
 *                               Observations                                  *
 ******************************************************************************/
Observations::Observations() {
}

Observations::Observations(anenPar::Parameters parameters,
        anenSta::Stations stations, anenTime::Times times) :
parameters_(parameters),
stations_(stations),
times_(times) {
}

Observations::~Observations() {
}

size_t
Observations::getParameterIndex(size_t parameter_ID) const {
    return (parameters_.getParameterIndex(parameter_ID));
}

size_t
Observations::getStationIndex(size_t station_ID) const {
    return (stations_.getStationIndex(station_ID));
}

size_t
Observations::getTimeIndex(double timestamp) const {
    return (times_.getTimeIndex(timestamp));
};

size_t
Observations::get_parameters_size() const {
    return (parameters_.size());
}

size_t
Observations::get_stations_size() const {
    return (stations_.size());
}

size_t
Observations::get_times_size() const {
    return (times_.size());
}

anenPar::Parameters const &
Observations::getParameters() const {
    return parameters_;
}

anenSta::Stations const &
Observations::getStations() const {
    return stations_;
}

anenTime::Times const &
Observations::getTimes() const {
    return times_;
}

void
Observations::setParameters(anenPar::Parameters parameters) {
    this->parameters_ = parameters;
}

void
Observations::setStations(anenSta::Stations stations) {
    this->stations_ = stations;
}

void
Observations::setTimes(anenTime::Times times) {
    this->times_ = times;
}

void
Observations::print(ostream& os) const {
    os << "[Observations] size: [" <<
            parameters_.size() << ", " <<
            stations_.size() << ", " <<
            times_.size() << "]" << endl << endl
            << parameters_ << endl
            << stations_ << endl
            << times_ << endl;
}

ostream &
operator<<(ostream& os, const Observations & obj) {
    obj.print(os);
    return os;
}

/*******************************************************************************
 *                         Observations_array                                  *
 ******************************************************************************/

Observations_array::Observations_array() {
}

Observations_array::Observations_array(anenPar::Parameters parameters,
        anenSta::Stations stations, anenTime::Times times) :
Observations(parameters, stations, times) {
    updateDataDims();
}

Observations_array::Observations_array(
        anenPar::Parameters parameters, anenSta::Stations stations, anenTime::Times times,
        const vector<double> & vals) :
Observations_array(parameters, stations, times) {
    setValues(vals);
}

Observations_array::~Observations_array() {
}

boost::multi_array<double, 3> const &
Observations_array::getValues() const {
    return (data_);
};

const double*
Observations_array::data() const {
    return (data_.data());
}

double
Observations_array::getValueByIndex(size_t parameter_index,
        size_t station_index, size_t time_index) const {
    return (data_[parameter_index][station_index][time_index]);
}

double
Observations_array::getValueByID(size_t parameter_ID, size_t station_ID,
        double timestamp) const {
    size_t parameter_index = parameters_.getParameterIndex(parameter_ID);
    size_t station_index = stations_.getStationIndex(station_ID);
    size_t time_index = times_.getTimeIndex(timestamp);
    return (data_[parameter_index][station_index][time_index]);
}

void
Observations_array::setValue(double val, size_t parameter_index,
        size_t station_index, size_t time_index) {
    data_[parameter_index][station_index][time_index] = val;
}

void
Observations_array::setValue(double val, size_t parameter_ID,
        size_t station_ID, double timestamp) {
    size_t parameter_index = parameters_.getParameterIndex(parameter_ID);
    size_t station_index = stations_.getStationIndex(station_ID);
    size_t time_index = times_.getTimeIndex(timestamp);
    data_[parameter_index][station_index][time_index] = val;
}

void
Observations_array::setValues(const vector<double>& vals) {

    // Check the length of data
    if (data_.num_elements() != vals.size()) {
        string message = "length of observations container (";
        message.append(to_string(data_.num_elements()));
        message.append(") != length of input (");
        message.append(to_string(vals.size()));
        message.append(")!");
        throw length_error(message);
    }

    data_.assign(vals.begin(), vals.end());
}

void
Observations_array::updateDataDims() {
    
    try {
        data_.resize(boost::extents
            [parameters_.size()][stations_.size()][times_.size()]);
    } catch (bad_alloc & e) {
        cout << BOLDRED << "Error: insufficient memory while resizing the"
                << " array4D to hold " << parameters_.size() << "x"
                << stations_.size() << "x" << times_.size()
                << " double values." << endl;
        throw e;
    }   
}

void
Observations_array::print(ostream& os) const {

    // Base class print function
    Observations::print(os);
    os << endl;

    // Print data in derived class
    os << "Array Shape = ";
    for (size_t i = 0; i < 3; i++) {
        os << "[" << data_.shape()[i] << "]";
    }
    os << endl;

    auto sizes = data_.shape();

    for (size_t m = 0; m < sizes[0]; m++) {

        os << "[ " << m << ", , ]" << endl;

        for (size_t p = 0; p < sizes[2]; p++) {
            os << "\t[,," << p << "]";
        }
        os << endl;

        for (size_t o = 0; o < sizes[1]; o++) {
            os << "[," << o << ",]\t";

            for (size_t p = 0; p < sizes[2]; p++) {
                os << data_[m][o][p] << "\t";
            }
            os << endl;

        }
        os << endl;

    }
    os << endl;
}

ostream &
operator<<(ostream& os, const Observations_array& obj) {
    obj.print(os);
    return os;
}