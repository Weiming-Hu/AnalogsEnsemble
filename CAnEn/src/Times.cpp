/* 
 * File:   Times.cpp
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <cervone@psu.edu>
 *
 * Created on April 18, 2018, 12:33 AM
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

#include "Times.h"
#include "Config.h"

#include <stdexcept>
#include <sstream>

using namespace std;

std::string Time::_origin = Config::_ORIGIN;
std::string Time::_unit = Config::_UNIT;

Time::Time() : timestamp(Config::_TIME) {
}

Time::Time(size_t val) {
    timestamp = val;
}

Time::Time(const Time & rhs) {
    if (this != &rhs) {
        timestamp = rhs.timestamp;
    }
}

Time::~Time() {
}

Time &
Time::operator=(const Time & rhs) {
    if (this != &rhs) timestamp = rhs.timestamp;
    return *this;
}

Time &
Time::operator=(size_t rhs) {
    timestamp = rhs;
    return *this;
}

Time
Time::operator+(const Time & rhs) const {
    return Time(timestamp + rhs.timestamp);
}

bool
Time::operator<(const Time & rhs) const {
    return (timestamp < rhs.timestamp);
}

void
Time::print(ostream &os) const {
    os << timestamp << endl;
}

ostream&
operator<<(ostream& os, Time const & obj) {
    obj.print(os);
    return os;
}

/***************************************************************************
 *                               Times                                     *
 **************************************************************************/

Times::Times() {
}

Times::~Times() {
}

size_t
Times::getIndex(const Time & time) const {
    auto it = right.find(time);
    if (it == right.end()) {
        ostringstream msg;
        msg << "Time not found: " << time;
        throw range_error(msg.str());
    }
    return (it->second);
}

const Time &
Times::getTime(size_t index) const {
    return this->left[index].second;
}


void
Times::print(ostream &os) const {
    os << "[Times] size: " << size() << endl;

    for (left_const_iterator it = left.begin(); it < left.end(); it++) {
        os << "[" << it->first << "] " << it->second;
    }

    return;
}

ostream&
operator<<(ostream& os, Times const & obj) {
    obj.print(os);
    return os;
}