/* 
 * File:   Forecasts.cpp
 * Author: guido
 *
 * Created on April 18, 2018, 12:40 AM
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

#include "Forecasts.h"


/*******************************************************************************
 *                            Forecasts                                        *
 ******************************************************************************/

Forecasts::Forecasts(Parameters parameters, Stations stations, Times time, FLTs flt) :
parameters_(parameters), 
stations_(stations),
times_(time), 
flts_(flt)
{
    size_parameters_ = parameters_.size();
    size_stations_ = stations_.size();
    size_time_ = times_.size();
    size_flt_ = flts_.size();
            
}

Forecasts::Forecasts() {
    std::cerr << "The default constructor of Forecasts should not be called!" << std::endl;
}

Forecasts::~Forecasts() {
}


void
Forecasts::print(std::ostream &os) const {
    os << "[Forecasts] size: [" << 
            size_parameters_ << ", " <<
            size_stations_ << ", " <<
            size_time_ << ", " <<
            size_flt_ << "]" << std::endl << std::endl;
    os << parameters_ << std::endl;
    os << stations_ << std::endl;
    os << times_ << std::endl<< std::endl;
    os << flts_ << std::endl;
}

std::ostream&
operator<<(std::ostream& os, Forecasts const & obj) {
    obj.print(os);
    return os;
}



/*******************************************************************************
 *                            Forecasts_array                                  *
 ******************************************************************************/

Forecasts_array::Forecasts_array(Parameters parameters, Stations stations, Times time, FLTs flt) :
Forecasts(parameters, stations, time,flt) 
{            
    data_.myresize(size_parameters_, size_stations_, size_time_, size_flt_);

}



Forecasts_array::~Forecasts_array() {
}

bool 
Forecasts_array::setValue(double value, int p, int s, int t, int f) {
    data_[p][s][t][f] = value;
    return(true);
}

double 
Forecasts_array::getValue(int p, int s, int t, int f) const {
    return( data_[p][s][t][f] );
}


void
Forecasts_array::print(std::ostream &os) const {
    Forecasts::print(os);
    os << std::endl;
    os << data_ << endl;
}
