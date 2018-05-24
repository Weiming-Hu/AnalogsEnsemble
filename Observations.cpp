/* 
 * File:   Observations.cpp
 * Author: Weiming Hu (weiming@psu.edu)
 * 
 * Created on May 4, 2018, 11:27 AM
 */

#include "Observations.h"

/*******************************************************************************
 *                               Observations                                  *
 ******************************************************************************/

Observations::Observations(
        Parameters parameters_, Stations stations_, Times times_) :
parameters_(parameters_),
stations_(stations_),
times_(times_) {
}

Observations::~Observations() {
}

void
Observations::print(std::ostream& os) const {
    os << "[Observations] size: [" <<
            parameters_.size() << ", " <<
            stations_.size() << ", " <<
            times_.size() << "]" << std::endl << std::endl
            << parameters_ << std::endl
            << stations_ << std::endl
            << times_ << std::endl;
}

std::ostream &
operator<<(std::ostream& os, Observations const & obj) {
    obj.print(os);
    return os;
}



/*******************************************************************************
 *                         Observations_array                                  *
 ******************************************************************************/


Observations_array::Observations_array(
        Parameters parameters, Stations stations, Times times) :
Observations(parameters, stations, times) {

    // TODO: memory check
    data_.resize(boost::extents
            [parameters_.size()][stations_.size()][times_.size()]);
}

Observations_array::~Observations_array() {
}

bool 
Observations_array::setValues( double* data ) {
    return false;
}


bool
Observations_array::setValue(double val, size_t i, size_t j, size_t k) {
    data_[i][j][k] = val;
    return (true);
}

double
Observations_array::getValue(size_t i, size_t j, size_t k) const {
    return (data_[i][j][k]);
}

void
Observations_array::print(std::ostream& os) const {

    // Base class print function
    Observations::print(os);
    os << std::endl;

    // Print data in derived class
    os << "Array Shape = ";
    for (size_t i = 0; i < 3; i++) {
        os << "[" << data_.shape()[i] << "]";
    }
    os << std::endl;

    auto sizes = data_.shape();

    for (size_t m = 0; m < sizes[0]; m++) {

        os << "[ " << m << ", , ]" << std::endl;

        for (size_t p = 0; p < sizes[2]; p++) {
            os << "\t[,," << p << "]";
        }
        os << std::endl;

        for (size_t o = 0; o < sizes[1]; o++) {
            os << "[," << o << ",]\t";

            for (size_t p = 0; p < sizes[2]; p++) {
                os << data_[m][o][p] << "\t";
            }
            os << std::endl;

        }
        os << std::endl;

    }
    os << std::endl;
}