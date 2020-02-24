/* 
 * File:   Forecasts.cpp
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <weiming@psu.edu>
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

Forecasts::Forecasts() : Array4D(), BasicData() {
}

Forecasts::Forecasts(const Forecasts & orig) : Array4D(orig), BasicData(orig) {
    if (this != &orig) {
        flts_ = orig.flts_;
    }
}

Forecasts::Forecasts(const Parameters & parameters, const Stations & stations,
        const Times & times, const Times & flts) :
Array4D(), BasicData(parameters, stations, times), flts_(flts) {
}

Forecasts::~Forecasts() {
}

Times const &
Forecasts::getFLTs() const {
    return flts_;
}

Times &
Forecasts::getFLTs() {
    return flts_;
}

size_t
Forecasts::getFltTimeStamp(size_t index) const {
    return (flts_.left[index].second.timestamp);
}

size_t
Forecasts::getFltTimeIndex(size_t timestamp) const {
    return getFltTimeIndex(Time(timestamp));
}

size_t
Forecasts::getFltTimeIndex(Time const & time) const {
    return flts_.getIndex(time);
}

void
Forecasts::print(ostream &os) const {
    os << "[Forecasts] size: [" <<
            parameters_.size() << ", " <<
            stations_.size() << ", " <<
            times_.size() << ", " <<
            flts_.size() << "]" << endl;
    BasicData::print(os);
    os << flts_;
    return;
}

ostream&
operator<<(ostream& os, Forecasts const & obj) {
    obj.print(os);
    return os;
}
