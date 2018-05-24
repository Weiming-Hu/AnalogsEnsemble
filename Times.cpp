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

#include "Times.h"
#include <iterator>


Times::Times() {
    origin_ = "1970-01-01";
    unit_ = "seconds";
}

Times::Times(std::string unit) :
unit_(unit)
{
    origin_ = "1970-01-01";
}

Times::Times(std::string unit, std::string origin) :
unit_(unit), origin_(origin)
{
}


Times::~Times() {
}


void
Times::print(std::ostream &os) const {
  os << "[Time] size: " << size() << std::endl;
  std::ostream_iterator< int > element_itr( os, ", " );
  copy( begin(), end(), element_itr );

}

std::ostream&
operator<<(std::ostream& os, Times const & obj) {
    obj.print(os);
    return os;
}

//
//FLTs::FLTs() {
//    unit_ = "seconds";
//}
//
//FLTs::~FLTs() {
//    
//}
//
//
//void
//FLTs::print(std::ostream &os) const {
//  os << "[FLT] size: " << size() << std::endl;
//  std::ostream_iterator< int > element_itr( os, ", " );
//  copy( begin(), end(), element_itr );
//
//}
//
std::ostream&
operator<<(std::ostream& os, FLTs const & obj) {
    obj.print(os);
    return os;
}


