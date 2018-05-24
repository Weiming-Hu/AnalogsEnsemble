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

/*******************************************************************************
 *                            Forecasts                                        *
 ******************************************************************************/

Forecasts::Forecasts(Parameters parameters, Stations stations, Times time, FLTs flt) :
parameters_(parameters),
stations_(stations),
times_(time),
flts_(flt) {
    //size_parameters_ = parameters_.size();
    //size_stations_ = stations_.size();
    //size_time_ = times_.size();
    //size_flt_ = flts_.size();

}

Forecasts::Forecasts() {
    std::cerr << "The default constructor of Forecasts should not be called!" << std::endl;
}

Forecasts::Forecasts(Forecasts const & rhs) {
    *this = rhs;
}

Forecasts::~Forecasts() {
}

Forecasts &
        Forecasts::operator=(Forecasts const & rhs) {

    if (this != &rhs) {
        parameters_ = rhs.parameters_;
    }
    return *this;
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
Forecasts::getTime() const {
    return times_;
}

FLTs const & 
Forecasts::getFLTs() const {
    return flts_;
}


void
Forecasts::print(std::ostream &os) const {
    os << "[Forecasts] size: [" <<
            get_parameters_size() << ", " <<
            get_stations_size() << ", " <<
            get_times_size() << ", " <<
            get_flts_size() << "]" << std::endl << std::endl;
    os << parameters_ << std::endl;
    os << stations_ << std::endl;
    os << times_ << std::endl << std::endl;
    os << flts_ << std::endl;
}

std::ostream&
operator<<(std::ostream& os, Forecasts const & obj) {
    obj.print(os);
    return os;
}

/*******************************************************************************
 *                            Forecasts_array                                  *
 ******************************************************************************/

Forecasts_array::Forecasts_array(Parameters parameters, Stations stations, Times time, FLTs flt) :
Forecasts(parameters, stations, time, flt) {
    data_.myresize(get_parameters_size(), get_stations_size(), get_times_size(), get_flts_size());

}

Forecasts_array::Forecasts_array(Forecasts_array const & rhs) {
    *this = rhs;
}

Forecasts_array::~Forecasts_array() {
}

Forecasts_array &
        Forecasts_array::operator=(Forecasts_array const & rhs) {

    if (this != &rhs) {
        data_ = rhs.getValues();
    }
    return *this;
}

Array4D const &
Forecasts_array::getValues() const {
    return ( data_);
}

bool
Forecasts_array::setValue(double value, int p, int s, int t, int f) {
    data_[p][s][t][f] = value;
    return (true);
}

double
Forecasts_array::getValue(int p, int s, int t, int f) const {
    return ( data_[p][s][t][f]);
}

size_t
Forecasts_array::get_parameters_size() const {
    return ( parameters_.size());
}

size_t
Forecasts_array::get_stations_size() const {
    return ( stations_.size());
}

size_t
Forecasts_array::get_times_size() const {
    return ( times_.size());
}

size_t
Forecasts_array::get_flts_size() const {
    return ( flts_.size());
}

void
Forecasts_array::print(std::ostream &os) const {
    Forecasts::print(os);
    os << std::endl;
    os << data_ << endl;
}
