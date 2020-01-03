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
#include <iterator>

using namespace std;

/***************************************************************************
 *                              Parameter                                  *
 **************************************************************************/

Parameter::Parameter() :
name_(_DEFAULT_NAME), weight_(_DEFAULT_WEIGHT), circular_(_DEFAULT_CIRCULAR) {
}


Parameter::Parameter(string name, double weight, bool circular) :
name_(name), weight_(weight), circular_(circular) {
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
        weight_ = rhs.getWeight();
        circular_ = rhs.getCircular();
    }

    return *this;
}

bool
Parameter::operator==(const Parameter& rhs) const {
    if (name_ != rhs.getName()) return false;
    if (weight_ != rhs.getWeight()) return false;
    if (circular_ != rhs.getCircular()) return false;

    return true;
}

bool
Parameter::operator<(const Parameter& rhs) const {
    return ( name_ < rhs.getName() );
}


void
Parameter::setName(string name) {
    name_ = name;
}

void
Parameter::setWeight(double weight) {
    weight_ = weight;
}

void
Parameter::setCircular(bool circular) {
    circular_ = circular;
}

string
Parameter::getName() const {
    return name_;
}

double
Parameter::getWeight() const {
    return weight_;
}

bool
Parameter::getCircular() const {
    return circular_;
}

void
Parameter::print(ostream &os) const {
    os << "[Parameter] Name: " << name_
            << ", weight: " << weight_ << ", circular: " << circular_
            << endl;
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

Parameter const &
Parameters::getParameterByName(string name) const {

    const multiIndexParameters::index<By::name>::type &
            parameters_by_name = get<By::name>();

    auto it_name = parameters_by_name.find(name);

    if (it_name != parameters_by_name.end()) {
        return (*it_name);
    } else {
        throw out_of_range("Can't find the parameter with name " + name);
    }
}

void
Parameters::print(ostream & os) const {
    os << "[Parameters] size: " << size() << endl;
    copy(begin(), end(), ostream_iterator<Parameter>(os));
}

ostream&
operator<<(ostream& os, Parameters const & obj) {
    obj.print(os);
    return os;
}
