/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Parameters.cpp
 * Author: guido
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
#include <cmath>


Parameters::Parameters() {
}

Parameters::~Parameters() {
}

void 
Parameters::print(std::ostream & os) const {  
    os << "[Parameters] size: " << size() << std::endl;
  std::ostream_iterator< Parameter > element_itr( os, "" );
  copy( begin(), end(), element_itr );
}

std::ostream& operator<<(std::ostream& os, Parameters const & obj)
{
    obj.print(os);
    return os;
}




/*  Parameter */


Parameter::Parameter() {
    ID_ = "NOT DEFINED";
    weight_ = NAN;
    circular_ = false;
}


Parameter::Parameter(std::string ID, double weight, bool circular) :
ID_(ID), weight_(weight), circular_(circular) {
}


Parameter::~Parameter() {
}

void
Parameter::SetID(std::string ID) {
    ID_ = ID;
}

void
Parameter::SetWeight(double weight) {
    weight_ = weight;
}

void
Parameter::SetCircular(bool circular) {
    circular_ = circular;
}

std::string
Parameter::GetID() const {
    return ID_;
}

double
Parameter::GetWeight() const {
    return weight_;
}

bool
Parameter::GetCircular() const {
    return circular_;
}

void
Parameter::print(std::ostream &os) const {
    os << "[Parameter] ID: " << ID_ << ", weight: " << weight_ << ", circular: " << circular_ << std::endl;
}

std::ostream&
operator<<(std::ostream& os, Parameter const & obj) {
    obj.print(os);
    return os;
}

