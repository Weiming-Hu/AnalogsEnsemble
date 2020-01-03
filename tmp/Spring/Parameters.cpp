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

size_t Parameter::_static_ID_ = 0;

static const string _DEFAULT_NAME = "UNDEFINED";
static const double _DEFAULT_WEIGHT = NAN;
static const bool _DEFAULT_CIRCULAR = false;


/***************************************************************************
 *                              Parameter                                  *
 **************************************************************************/

Parameter::Parameter() {
    setID_();
    name_ = _DEFAULT_NAME;
    weight_ = _DEFAULT_WEIGHT;
    circular_ = _DEFAULT_CIRCULAR;
}

Parameter::Parameter(string name) :
name_(name) {
    setID_();
    weight_ = _DEFAULT_WEIGHT;
    circular_ = _DEFAULT_CIRCULAR;
}

Parameter::Parameter(string name, double weight) :
name_(name), weight_(weight) {
    setID_();
    circular_ = _DEFAULT_CIRCULAR;
}

Parameter::Parameter(string name, double weight, bool circular) :
name_(name), weight_(weight), circular_(circular) {
    setID_();
}

Parameter::Parameter(string name, bool circular) :
name_(name), circular_(circular) {
    setID_();
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
        ID_ = rhs.getID();
    }

    return *this;
}

bool
Parameter::operator==(const Parameter& right) const {
    return (ID_ == right.getID());
}

bool
Parameter::operator<(const Parameter& right) const {
    return (ID_ < right.getID());
}

bool
Parameter::literalCompare(const Parameter & rhs) const {

    if (name_ != rhs.getName()) return false;
    if (weight_ != rhs.getWeight()) return false;
    if (circular_ != rhs.getCircular()) return false;

    return true;
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

size_t
Parameter::getID() const {
    return ID_;
}

size_t
Parameter::getStaticID() {
    return Parameter::_static_ID_;
}

void
Parameter::print(ostream &os) const {
    os << "[Parameter] ID: " << ID_ << ", name: " << name_
            << ", weight: " << weight_ << ", circular: " << circular_
            << endl;
}

ostream&
operator<<(ostream& os, Parameter const & obj) {
    obj.print(os);
    return os;
}

void
Parameter::setID_() {
    ID_ = Parameter::_static_ID_;
    Parameter::_static_ID_++;
}

/***************************************************************************
 *                              Parameters                                 *
 **************************************************************************/

Parameters::Parameters() {
}

Parameters::~Parameters() {
}

size_t
Parameters::getParameterIndex(size_t parameter_ID) const {

    // Find the parameter ID in ID-based index
    const multiIndexParameters::index<By::ID>::type &
            parameters_by_ID = get<By::ID>();

    auto it_ID = parameters_by_ID.find(parameter_ID);

    if (it_ID != parameters_by_ID.end()) {

        // Project the ID-based order to insertion sequence
        auto it_insert = project<By::insert>(it_ID);

        // Get the insertion sequence index iterator
        const multiIndexParameters::index<By::insert>::type&
                parameters_by_insert = get<By::insert>();

        // Compute the distance
        return (distance(parameters_by_insert.begin(), it_insert));
    } else {
        throw out_of_range("Can't find the parameter IDs "
                + to_string((long long) parameter_ID));
    }
}

Parameter
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
