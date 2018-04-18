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

void 
Stations::print(std::ostream & os) const {  
    os << "[Stations] size: " << size() << std::endl;
  std::ostream_iterator< Station > element_itr( os, "" );
  copy( begin(), end(), element_itr );
}

std::ostream& operator<<(std::ostream& os, Stations const & obj)
{
    obj.print(os);
    return os;
}




/*  Station */


Station::Station() {
    ID_ = -9999;
    latitude_ = NAN;
    longitude_ = NAN;
}


Station::Station(int ID_, double longitude_, double latitude_) :
ID_(ID_), longitude_(longitude_), latitude_(latitude_) {
}

Station::~Station() {
}


void
Station::SetID(int ID) {
    ID_ = ID;
}

void
Station::SetLatitude(double latitude) {
    latitude_ = latitude;
}

void
Station::SetLongitude(double longitude) {
    longitude_ = longitude;
}

int
Station::GetID() const {
    return ID_;
}

double
Station::GetLatitude() const {
    return latitude_;
}

double
Station::GetLongitude() const {
    return longitude_;
}

void
Station::print(std::ostream &os) const {
    os << "[Station] ID: " << ID_ << ", longitude: " << longitude_ << ", latitude: " << latitude_ << std::endl;
}

std::ostream&
operator<<(std::ostream& os, Station const & obj) {
    obj.print(os);
    return os;
}

