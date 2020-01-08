/* 
 * File:   Stations.cpp
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <cervone@psu.edu>
 *
 * Created on April 17, 2018, 10:41 PM
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

#include "Stations.h"

#if defined(_OPENMP)
#include <omp.h>
#endif

using namespace std;

/**************************************************************************
 *                            Station                                     *
 **************************************************************************/
Station::Station() :
x_(AnEnDefaults::_X), y_(AnEnDefaults::_Y), name_(AnEnDefaults::_NAME) {
}

Station::Station(double x, double y, string name) :
x_(x), y_(y), name_(name) {
}

Station::Station(Station const & rhs) {
    *this = rhs;
}

Station::~Station() {
}

Station &
        Station::operator=(const Station & rhs) {

    if (this != &rhs) {
        name_ = rhs.getName();
        x_ = rhs.getX();
        y_ = rhs.getY();
    }

    return *this;
}

bool
Station::operator==(const Station & rhs) const {
    if (name_ != rhs.getName()) return false;
    if (x_ != rhs.getX()) return false;
    if (y_ != rhs.getY()) return false;
  
    return(true);
}

bool
Station::operator!=(const Station & rhs) const {
    bool result = !(*this == rhs); // Reuse equals operator
    return result;
}

bool
Station::operator<(const Station & rhs) const {
    return (name_ < rhs.getName() );
}

string
Station::getName() const {
    return (name_);
}

double
Station::getY() const {
    return y_;
}

double
Station::getX() const {
    return x_;
}

void
Station::print(ostream &os) const {
    os << "[Station] Name: " << name_
            << ", x: " << x_ << ", y: " << y_
            << endl;
}

ostream &
operator<<(ostream& os, Station const & obj) {
    obj.print(os);
    return os;
}


/**************************************************************************
 *                            Stations                                    *
 **************************************************************************/

Stations::Stations() {
}

Stations::~Stations() {
}

void
Stations::print(ostream & os) const {
    os << "[Stations] size: " << size() << endl;

    for (left_const_iterator it = left.begin(); it < left.end(); it++) {
        os << "[" << it->first << "] " << it->second;
    }

    return;
}

ostream &
operator<<(ostream& os, Stations const & obj) {
    obj.print(os);
    return os;
}
