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

namespace anenPar {

    using namespace std;

    /***************************************************************************
     *                              Parameter                                  *
     **************************************************************************/

    size_t Parameter::_static_ID_ = 0;

    Parameter::Parameter() {
        setID_();
    }

    Parameter::Parameter(string name) :
    name_(name) {
        setID_();
    }

    Parameter::Parameter(string name, double weight) :
    name_(name), weight_(weight){
        setID_();
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
    Parameter::compare(const Parameter & rhs) const {

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
        const multiIndexParameters::index<by_ID>::type &
                parameters_by_ID = get<by_ID>();

        auto it_ID = parameters_by_ID.find(parameter_ID);

        if (it_ID != parameters_by_ID.end()) {

            // Project the ID-based order to insertion sequence
            auto it_insert = project<by_insert>(it_ID);

            // Get the insertion sequence index iterator
            const multiIndexParameters::index<by_insert>::type&
                    parameters_by_insert = get<by_insert>();

            // Compute the distance
            return (distance(parameters_by_insert.begin(), it_insert));
        } else {
            throw out_of_range("Can't find the parameter IDs "
                    + to_string(parameter_ID));
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
}
