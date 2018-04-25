/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Stations.cpp
 * Author: guido
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
#include <cmath>

Stations::Stations() {
}

Stations::~Stations() {
}

Station const & 
Stations::operator [] ( int ID ) const {
    
    for ( Stations::const_iterator citer = begin() ;
          citer != end() ;
          citer++ ) {
        
        if ( citer->getID() == ID ) { return *citer; }
    }
    
    return ( *end());
}


Station const & 
Stations::operator [] ( Station const & station ) const {
    Stations::const_iterator citer  = find(station);
       
    return  *citer; 
}



std::vector<int>
Stations::getNearbyStations(Station const & station) const {

    Stations::const_iterator citer = find(station);
    std::vector<int> ret;

    if (citer != end()) {

        advance(citer, -1);
        ret.push_back(citer->getID());
        advance(citer, 1);
        ret.push_back(citer->getID());

        advance(citer, 1);
        ret.push_back(citer->getID());

    }

    return (ret);
}

std::vector<int>
Stations::getNearbyStations( int val ) const {

    Station const & station = (*this)[val];
    
    return(getNearbyStations(station));
}





void
Stations::print(std::ostream & os) const {
    os << "[Stations] size: " << size() << std::endl;
    std::ostream_iterator< Station > element_itr(os, "");
    copy(begin(), end(), element_itr);
}

std::ostream& operator<<(std::ostream& os, Stations const & obj) {
    obj.print(os);
    return os;
}


/*  Station */


unsigned int Station::_static_ID_ = 0;

Station::Station() {
    name_ = "NO NAME";
    y_ = NAN;
    x_ = NAN;

    set_ID_();
}

Station::Station(Station const & rhs) {
    *this = rhs;
}

Station::Station(std::string name, double x, double y) :
name_(name), x_(x), y_(y) {

    set_ID_();
}

Station::~Station() {
}

Station &
        Station::operator=(const Station & rhs) {
    // Do not compare the internal IDs

    if (this == &rhs) return *this;

    name_ = rhs.getName();
    x_ = rhs.getX();
    y_ = rhs.getY();
    ID_ = rhs.getID();


    return *this;
}

bool
Station::operator==(const Station & rhs) const {
    // Do not compare the internal IDs

    if (this == &rhs) return true;

    if (name_ != rhs.getName()) {
        return false;
    }
    if (x_ != rhs.getX()) {
        return false;
    }
    if (y_ != rhs.getY()) {
        return false;
    }

    return (true);
}

bool
Station::operator<(const Station & rhs) const {
    return ID_ < rhs.getID();
}

void
Station::set_ID_() {
    ID_ = Station::_static_ID_;
    Station::_static_ID_++;
}

//void
//Station::setName(std::string name) {
//    name_ = name;
//}
//
//void
//Station::setY(double y) {
//    y_ = y;
//}
//
//void
//Station::setX(double x) {
//    x_ = x;
//}

int
Station::getID() const {
    return ID_;
}

std::string
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
Station::print(std::ostream &os) const {
    os << "[Station] ID: " << ID_ << ", Name: " << name_ << ", x: " << x_ << ", y: " << y_ << std::endl;
}

std::ostream&
operator<<(std::ostream& os, Station const & obj) {
    obj.print(os);
    return os;
}

