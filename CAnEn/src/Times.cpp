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
#include "AnEnDefaults.h"
#include "colorTexts.h"

#include <stdexcept>
#include <sstream>

using namespace std;

std::string Time::_origin = AnEnDefaults::_ORIGIN;
std::string Time::_unit = AnEnDefaults::_UNIT;

Time::Time() : timestamp(AnEnDefaults::_TIME) {
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
        msg << BOLDRED << "Time not found: " << time << RESET;
        throw range_error(msg.str());
    }
    return (it->second);
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