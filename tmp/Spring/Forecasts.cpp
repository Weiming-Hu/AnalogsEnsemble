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
#include "colorTexts.h"

#include <cmath>
#include <memory>

using namespace std;

Forecasts::Forecasts() {
}

Forecasts::Forecasts(const Parameters & parameters, const Stations & stations,
        const Times & times, const FLTs & flts) : BasicData(parameters, stations, times),
flts_(flts) {
//    parameters_ = parameters;
//    stations_ = stations;
//    times_ = times;

}

Forecasts::Forecasts(const Forecasts & orig) : BasicData(orig) {
    if (this != &orig) {
        flts_ = orig.flts_;
    }
}

Forecasts::~Forecasts() {
}

FLTs const &
Forecasts::getFLTs() const {
    return flts_;
}

FLTs &
Forecasts::getFLTs() {
    return flts_;
}

void
Forecasts::print(ostream &os) const {
    os << "[Forecasts] size: [" <<
            parameters_.size() << ", " <<
            stations_.size() << ", " <<
            times_.size() << ", " <<
            flts_.size() << "]" << endl << endl;
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
