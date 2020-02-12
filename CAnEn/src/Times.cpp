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
#include "Functions.h"

#include <stdexcept>
#include <sstream>

using namespace std;

const string Time::_unit = "seconds";
const string Time::_origin = "1970-01-01 00:00:00";

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
Times::getTimestamps(vector<size_t> & timestamps) const {
    timestamps.resize(size());
    const auto & end = left.end();
    
    for (auto it = left.begin(); it != end; ++it)
        timestamps[it->first] = it->second.timestamp;
    
    return;
}



void
Times::print(ostream &os) const {
    os << "[Times] size: " << size() << " origin: " << Time::_origin
        << " unit: " << Time::_unit << endl;

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

void
Times::operator()(const string & start, const string & end,
        Times & sliced_times, bool iso_string) const {

    // Convert string to POSIXct time
    size_t start_time = Functions::toSeconds(start, Time::_origin, iso_string);
    size_t end_time = Functions::toSeconds(end, Time::_origin, iso_string);
    return operator ()(start_time, end_time, sliced_times);
}


void
Times::operator()(size_t start_time, size_t end_time, Times & sliced_times) const {
    
    sliced_times.clear();
    size_t counter = 0, current_time;
    
    const auto & end = left.end();
    for (auto it = left.begin(); it != end; ++it) {
        current_time = it->second.timestamp;
        if (current_time >= start_time && current_time <= end_time) {
            sliced_times.push_back(value_type(counter, current_time));
            counter++;
        }
    }
    
    return;
}
