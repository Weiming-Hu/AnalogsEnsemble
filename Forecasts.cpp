/* 
 * File:   Forecasts.cpp
 * Author: guido
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

using namespace std;

/*******************************************************************************
 *                            Forecasts                                        *
 ******************************************************************************/

Forecasts::Forecasts(Parameters parameters, Stations stations, Times time, FLTs flt) :
parameters_(parameters),
stations_(stations),
times_(time),
flts_(flt) {
}

Forecasts::~Forecasts() {
}

size_t
Forecasts::get_parameters_size() const {
    return ( parameters_.size());
}

size_t
Forecasts::get_stations_size() const {
    return ( stations_.size());
}

size_t
Forecasts::get_times_size() const {
    return ( times_.size());
}

size_t
Forecasts::get_flts_size() const {
    return ( flts_.size());
}

Parameters const &
Forecasts::getParameters() const {
    return parameters_;
}

Stations const &
Forecasts::getStations() const {
    return stations_;
}

Times const &
Forecasts::getTimes() const {
    return times_;
}

FLTs const &
Forecasts::getFLTs() const {
    return flts_;
}

void
Forecasts::print(ostream &os) const {
    os << "[Forecasts] size: [" <<
            get_parameters_size() << ", " <<
            get_stations_size() << ", " <<
            get_times_size() << ", " <<
            get_flts_size() << "]" << endl << endl;
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

Forecasts_array::Forecasts_array(Parameters parameters, Stations stations, Times time, FLTs flt) :
Forecasts(parameters, stations, time, flt) {

    try {
        data_.myresize(get_parameters_size(), get_stations_size(),
                get_times_size(), get_flts_size());
    } catch (std::length_error & e) {
        cout << e.what() << endl;
        throw e;
    } catch (std::bad_alloc & e) {
        cout << e.what() << endl;
        throw e;
    }

}

Forecasts_array::~Forecasts_array() {
}

Array4D const &
Forecasts_array::getValues() const {
    return ( data_);
}

double
Forecasts_array::getValue(std::size_t parameter_ID, std::size_t station_ID,
        double timestamp, double flt) const {
    // TODO
    return NAN;
}

bool
Forecasts_array::setValue(double val, std::size_t parameter_ID, 
        std::size_t station_ID, double timestamp, double flt) {
    // TODO
    return false;
}

bool
Forecasts_array::setValues(const vector<double> & data) {

    if (data_.num_elements() != data.size()) {
        return false;
    }

    data_.getDataFromVector(data,
            get_parameters_size(), get_stations_size(),
            get_times_size(), get_flts_size());
    return true;
}

void
Forecasts_array::print(ostream &os) const {
    Forecasts::print(os);
    os << endl;
    os << data_ << endl;
}

ostream &
operator<<(ostream & os, const Forecasts_array& obj) {
    obj.print(os);
    return os;
}