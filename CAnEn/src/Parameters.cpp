/* 
 * File:   Parameters.cpp
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <cervone@psu.edu>
 *
 * Created on April 18, 2018, 12:18 AM
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

#include "Parameters.h"

#include <stdexcept>
#include <sstream>

using namespace std;

/***************************************************************************
 *                              Parameter                                  *
 **************************************************************************/

Parameter::Parameter() : name_(Config::_NAME), circular_(Config::_CIRCULAR) {
}

Parameter::Parameter(string name, bool circular) :
name_(name), circular_(circular) {
}

Parameter::Parameter(const Parameter& rhs) {
    *this = rhs;
}

Parameter::~Parameter() {
}

Parameter &
        Parameter::operator=(const Parameter& rhs) {

    if (this != &rhs) {
        name_ = rhs.getName();
        circular_ = rhs.getCircular();
    }

    return *this;
}

bool
Parameter::operator==(const Parameter& rhs) const {
    if (name_ != rhs.getName()) return false;

    return true;
}

bool
Parameter::operator<(const Parameter& rhs) const {
    return ( name_ < rhs.getName());
}

void
Parameter::setName(string name) {
    name_ = name;
}

void
Parameter::setCircular(bool circular) {
    circular_ = circular;
}

string
Parameter::getName() const {
    return name_;
}

bool
Parameter::getCircular() const {
    return circular_;
}

void
Parameter::print(ostream &os) const {
    os << "[Parameter] Name: " << name_
            << ", circular: " << (circular_ ? "Yes" : "No");
}

ostream&
operator<<(ostream& os, Parameter const & obj) {
    obj.print(os);
    return os;
}

/***************************************************************************
 *                              Parameters                                 *
 **************************************************************************/

Parameters::Parameters() {
}

Parameters::~Parameters() {
}

size_t
Parameters::getIndex(const Parameter & parameter) const {
    auto it = right.find(parameter);
    if (it == right.end()) {
        ostringstream msg;
        msg << "Parameter not found: " << parameter;
        throw range_error(msg.str());
    }
    return (it->second);
}

const Parameter &
Parameters::getParameter(size_t index) const {
    return this->left[index].second;
}

void
Parameters::getCirculars(vector<bool> & circulars) const {
    circulars.resize(size());
    for (size_t i = 0; i < size(); ++i) {
        circulars[i] = this->left[i].second.getCircular();
    }
    return;
}

void
Parameters::getNames(vector<string>& names) const {
    names.resize(size());
    
    const auto & end = left.end();
    for (auto it = left.begin(); it != end; ++it)
        names[it->first] = it->second.getName();
    return;
}

void
Parameters::print(ostream & os) const {
    os << "[Parameters] size: " << size() << endl;

    for (left_const_iterator it = left.begin(); it < left.end(); it++) {
        os << "[" << it->first << "] " << it->second << endl;
    }

    return;
}

ostream&
operator<<(ostream& os, Parameters const & obj) {
    obj.print(os);
    return os;
}
