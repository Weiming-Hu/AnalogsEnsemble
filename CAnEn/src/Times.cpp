/* 
 * File:   Time.cpp
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

#include <iterator>
#include <sstream>
#include <cmath>

using namespace std;

static const int MULTIPLIER = 10000;

std::string Time::origin = "1970-01-01";
std::string Time::unit = "seconds";

Time::Time() 
{}

Time::Time(double val) 
{
    timestamp = val;
}


Time::Time(const Time & rhs) {
    if (this != &rhs) {
        timestamp = rhs.timestamp;
    }
}

Time::~Time() 
{}


Time & Time::operator=(const Time & rhs) {
    if (this != &rhs) {
        timestamp = rhs.timestamp;
    }

    return *this;
}

Time & Time::operator=(double rhs) {
        timestamp = rhs;
    return *this;
}


bool
Time::operator<(const Time & rhs) const {
    return (timestamp < rhs.timestamp);
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

Times::Times()  {
}

Times::~Times() {
}

Times & Times::operator=(const Times & rhs) {
    if (this != &rhs) {
        clear();
        insert(rhs.begin(), rhs.end());
    }

    return *this;
}

void
Times::print(ostream &os) const {
    os << "[Time] size: " << size() << endl;
    ostream_iterator<Time> element_itr(os, ", ");
    copy(begin(), end(), element_itr);
    cout << endl;
}

ostream&
operator<<(ostream& os, Times const & obj) {
    obj.print(os);
    return os;
}