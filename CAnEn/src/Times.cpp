/* 
 * File:   Times.cpp
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <weiming@psu.edu>
 *
 * Created on April 18, 2018, 12:33 AM
 * 
 *  "`-''-/").___..--''"`-._
 * (_Y_.)'  ._   )  `._ `. ``-..-'    PENN STATE!
 * (`6_ 6  )   `-.  (     ).`-.__.`)   WE ARE ...
 *   _ ..`--'_..-_/  /--'_.' ,' 
 * (il),-''  (li),'  ((!.-'
 *
 *        Geoinformatics and Earth Observation Laboratory (http://geolab.psu.edu)
 *        Department of Geography and Institute for CyberScience
 *        The Pennsylvania State University
 */

#include <stdexcept>
#include <sstream>

#include "Times.h"
#include "Config.h"
#include "Functions.h"

#include "boost/date_time/posix_time/posix_time.hpp"

using namespace std;


const string Time::_unit = "seconds";
const string Time::_origin = "1970-01-01 00:00:00";

Time::Time() : timestamp(Config::_TIME) {
}

Time::Time(size_t val) {
    timestamp = val;
}

Time::Time(const string & str, bool iso_string) {
    timestamp = Functions::toSeconds(str, _origin, iso_string);
}

Time::Time(const string & str, const string & origin, bool iso_string) {
    timestamp = Functions::toSeconds(str, origin, iso_string);
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
Time::operator==(const Time & rhs) const {
    return timestamp == rhs.timestamp;
}

bool
Time::operator<(const Time & rhs) const {
    return (timestamp < rhs.timestamp);
}

bool
Time::operator>(const Time & rhs) const {
    return (timestamp > rhs.timestamp);
}

bool
Time::operator<=(const Time & rhs) const {
    return (timestamp <= rhs.timestamp);
}

bool
Time::operator>=(const Time & rhs) const {
    return (timestamp >= rhs.timestamp);
}

string
Time::toString() const {

#ifdef _DISABLE_NON_HEADER_BOOST
    /*
     * Please note that if _DISABLE_NON_HEADER_BOOST is defined, this function
     * will be unavailable because it depends on the non-header portion of boost.
     * The R package BH only provides header-only boost so this function won't work.
     * I'm aware of the R package RcppBDT but, since this function is never used 
     * in R routines, I can simply disable this function and avoid an extra dependency.
     */
    throw runtime_error("Time::toString is disabled because it depends on non-header boost libraries");
#else
    // Create POSIX time using the original time
    boost::posix_time::ptime mytime = boost::posix_time::time_from_string(Time::_origin);
    
    // Create a time duration for the offset
    boost::posix_time::time_duration offset = boost::posix_time::seconds(timestamp);
    
    // Increment my time
    mytime += offset;
    
    // Convert to string
    return(boost::posix_time::to_simple_string(mytime));
#endif
}

void
Time::print(ostream &os) const {
    os << timestamp;
}

ostream&
operator<<(ostream& os, Time const & obj) {
    obj.print(os);
    return os;
}

/***************************************************************************
 *                               Times                                     *
 **************************************************************************/

void
Times::push_back(const Time & time) {
    BmType<Time>::push_back(value_type(size(), time));
    return;
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

void
Times::getIndices(const Times & times, vector<size_t> & indices) const {
    size_t num_times = times.size();
    indices.resize(num_times);

    for (size_t i = 0; i < num_times; ++i) {
        indices[i] = getIndex(times.getTime(i));
    }

    return;
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
        os << "[" << it->first << "] " << it->second << endl;
    }

    return;
}

ostream&
operator<<(ostream& os, Times const & obj) {
    obj.print(os);
    return os;
}

void
Times::operator()(const Time & start, const Time & end, Times & sliced_times) const {
    return operator()(start.timestamp, end.timestamp, sliced_times);
}

void
Times::operator()(const string & start, const string & end,
        Times & sliced_times, bool iso_string) const {

    // Convert string to POSIXct time
    size_t start_time = Functions::toSeconds(start, Time::_origin, iso_string);
    size_t end_time = Functions::toSeconds(end, Time::_origin, iso_string);
    return operator()(start_time, end_time, sliced_times);
}

void
Times::operator()(size_t start_time, size_t end_time, Times & sliced_times) const {

    sliced_times.clear();

    const auto & end = left.end();
    for (auto it = left.begin(); it != end; ++it) {
        size_t current_time = it->second.timestamp;
        if (current_time >= start_time && current_time <= end_time) {
            sliced_times.push_back(current_time);
        }
    }

    return;
}
