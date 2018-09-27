/* 
 * File:   Forecasts.cpp
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <cervone@psu.edu>
 *
 * Created on April 18, 2018, 12:40 AM
 * 
 *  "`-''-/").___..--''"`-._
 * (`6_ 6  )   `-.  (     ).`-.__.`)   WE ARE ...
 * (_Y_.)'  ._   )  `._ `. ``-..-'    PENN STATE!
 *   _ ..`--'_..-_/  /--'_.' ,' 
 * (il),-''  (li),'  ((!.-'
 *
 *        Geoinformatics and Earth Observation Laboratory (http://geolab.psu.edu)
 *        Department of Geography and Institute for CyberScience
 *        The Pennsylvania State University
 */

#include "Forecasts.h"
#include "colorTexts.h"

#include <cmath>
#include <memory>

using namespace std;

/*******************************************************************************
 *                            Forecasts                                        *
 ******************************************************************************/
Forecasts::Forecasts() {
}

Forecasts::Forecasts(anenPar::Parameters parameters,
        anenSta::Stations stations, anenTime::Times time, anenTime::FLTs flt) :
parameters_(parameters), stations_(stations),
times_(time), flts_(flt) {
}

Forecasts::~Forecasts() {
}

size_t
Forecasts::getParametersSize() const {
    return ( parameters_.size());
}

size_t
Forecasts::getStationsSize() const {
    return ( stations_.size());
}

size_t
Forecasts::getTimesSize() const {
    return ( times_.size());
}

size_t
Forecasts::getFLTsSize() const {
    return ( flts_.size());
}

anenPar::Parameters const &
Forecasts::getParameters() const {
    return parameters_;
}

anenSta::Stations const &
Forecasts::getStations() const {
    return stations_;
}

anenTime::Times const &
Forecasts::getTimes() const {
    return times_;
}

anenTime::FLTs const &
Forecasts::getFLTs() const {
    return flts_;
}

void
Forecasts::setFlts(anenTime::FLTs flts) {
    this->flts_ = flts;
}

void
Forecasts::setParameters(anenPar::Parameters parameters) {
    this->parameters_ = parameters;
}

void
Forecasts::setStations(anenSta::Stations stations) {
    this->stations_ = stations;
}

void
Forecasts::setTimes(anenTime::Times times) {
    this->times_ = times;
}

void
Forecasts::print(ostream &os) const {
    os << "[Forecasts] size: [" <<
            getParametersSize() << ", " <<
            getStationsSize() << ", " <<
            getTimesSize() << ", " <<
            getFLTsSize() << "]" << endl << endl;
    os << parameters_ << endl;
    os << stations_ << endl;
    os << times_ << endl << endl;
    os << flts_ << endl;
}

ostream&
operator<<(ostream& os, Forecasts const & obj) {
    obj.print(os);
    return os;
}

/*******************************************************************************
 *                            Forecasts_array                                  *
 ******************************************************************************/
constexpr double Forecasts_array::_DEFAULT;

Forecasts_array::Forecasts_array() {
}

Forecasts_array::Forecasts_array(anenPar::Parameters parameters,
        anenSta::Stations stations, anenTime::Times times, anenTime::FLTs flts) :
Forecasts(parameters, stations, times, flts) {
    updateDataDims();
}

Forecasts_array::Forecasts_array(anenPar::Parameters parameters,
        anenSta::Stations stations, anenTime::Times times, anenTime::FLTs flts,
        const vector<double>& vals) :
Forecasts(parameters, stations, times, flts) {
    updateDataDims();
    setValues(vals);
}

Forecasts_array::~Forecasts_array() {
}

Array4D const &
Forecasts_array::data() const {
    return (data_);
}

Array4D &
Forecasts_array::data() {
    return (data_);
}

double
Forecasts_array::getValueByIndex(size_t parameter_index, size_t station_index,
        size_t time_index, size_t flt_index) const {
    return (data_[parameter_index][station_index][time_index][flt_index]);
}

const double*
Forecasts_array::getValues() const {
    return (data_.data());
}

double
Forecasts_array::getValueByID(std::size_t parameter_ID, std::size_t station_ID,
        double timestamp, double flt) const {
    size_t parameter_index = parameters_.getParameterIndex(parameter_ID);
    size_t station_index = stations_.getStationIndex(station_ID);

    size_t i_time = times_.getTimeIndex(timestamp), 
            i_flt = flts_.getTimeIndex((flt));
    return (data_[parameter_index][station_index][i_time][i_flt]);
}

void
Forecasts_array::setValue(double val, size_t parameter_index,
        size_t station_index, size_t time_index, size_t flt_index) {
    data_[parameter_index][station_index][time_index][flt_index] = val;
}

void
Forecasts_array::setValue(double val, std::size_t parameter_ID,
        std::size_t station_ID, double timestamp, double flt) {
    size_t parameter_index = parameters_.getParameterIndex(parameter_ID);
    size_t station_index = stations_.getStationIndex(station_ID);

    size_t i_time = times_.getTimeIndex(timestamp), 
            i_flt = flts_.getTimeIndex((flt));
    data_[parameter_index][station_index][i_time][i_flt] = val;
}

void
Forecasts_array::setValues(const vector<double> & vals) {

    if (data_.num_elements() != vals.size()) {
        string message = "length of forecasts container (";
        message.append(to_string((long long) data_.num_elements()));
        message.append(") != length of input (");
        message.append(to_string(vals.size()));
        message.append(")!");
        throw length_error(message);
    }

    data_.setDataFromVector(vals,
            getParametersSize(), getStationsSize(),
            getTimesSize(), getFLTsSize());
}

void
Forecasts_array::updateDataDims() {
    try {
        data_.myresize(getParametersSize(), getStationsSize(),
                getTimesSize(), getFLTsSize());
    } catch (bad_alloc & e) {
        cout << BOLDRED << "Error: insufficient memory while resizing the"
                << " array4D to hold " << getFLTsSize() << "x"
                << getParametersSize() << "x" << getStationsSize() << "x"
                << getTimesSize() << " double values." << endl;
        throw;
    }

    fill_n(data_.data(), data_.num_elements(), _DEFAULT);
}

size_t
Forecasts_array::getDataLength() const {
    return (getParametersSize() * getStationsSize() *
            getFLTsSize() * getTimesSize());
}

void
Forecasts_array::print(ostream &os) const {
    Forecasts::print(os);
    os << endl;
    os << data_;
}

ostream &
operator<<(ostream & os, const Forecasts_array& obj) {
    obj.print(os);
    return os;
}
