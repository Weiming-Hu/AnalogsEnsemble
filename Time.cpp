/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Time.cpp
 * Author: guido
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

#include "Time.h"


Time::Time() {
    origin_ = "1970-01-01";
    unit_ = "seconds";
}


Time::~Time() {
}


void
Time::print(std::ostream &os) const {
  os << "[Time] size: " << size() << std::endl;
  std::ostream_iterator< int > element_itr( os, ", " );
  copy( begin(), end(), element_itr );

}

std::ostream&
operator<<(std::ostream& os, Time const & obj) {
    obj.print(os);
    return os;
}


FLT::FLT() {
    unit_ = "seconds";
}

FLT::~FLT() {
    
}


void
FLT::print(std::ostream &os) const {
  os << "[FLT] size: " << size() << std::endl;
  std::ostream_iterator< int > element_itr( os, ", " );
  copy( begin(), end(), element_itr );

}

std::ostream&
operator<<(std::ostream& os, FLT const & obj) {
    obj.print(os);
    return os;
}


