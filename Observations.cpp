/* 
 * File:   Observations.cpp
 * Author: Weiming Hu (weiming@psu.edu)
 * 
 * Created on May 4, 2018, 11:27 AM
 */

#include "Observations.h"

using namespace std;

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

size_t
Observations::get_parameters_size() const {
    return (parameters_.size());
}

size_t
Observations::get_stations_size() const {
    return (stations_.size());
}

size_t
Observations::get_times_size() const {
    return (times_.size());
}

Parameters const &
Observations::getParameters() const {
    return parameters_;
}

Stations const &
Observations::getStations() const {
    return stations_;
}

Times const &
Observations::getTimes() const {
    return times_;
}

void
Observations::print(ostream& os) const {
    os << "[Observations] size: [" <<
            parameters_.size() << ", " <<
            stations_.size() << ", " <<
            times_.size() << "]" << endl << endl
            << parameters_ << endl
            << stations_ << endl
            << times_ << endl;
}

ostream &
operator<<(ostream& os, const Observations & obj) {
    obj.print(os);
    return os;
}

/*******************************************************************************
 *                         Observations_array                                  *
 ******************************************************************************/


Observations_array::Observations_array(
        Parameters parameters, Stations stations, Times times) :
Observations(parameters, stations, times) {

    try {
        data_.resize(boost::extents[parameters_.size()][stations_.size()][times_.size()]);
    } catch (std::length_error & e) {
        cout << e.what() << endl;
        throw e;
    } catch (std::bad_alloc & e) {
        cout << e.what() << endl;
        throw e;
    }
}

Observations_array::~Observations_array() {
}

double
Observations_array::getValue(std::size_t parameter_ID, std::size_t station_ID, 
        double timestamp) const {
    //TODO
    return NAN;
}

bool
Observations_array::setValue(double val, std::size_t parameter_ID, 
        std::size_t station_ID, double timestamp) {
    // TODO
    return false;
}

bool
Observations_array::setValues(const vector<double> & data) {
    
    // Check the length of data
    if (data_.num_elements() != data.size()) {
        return false;
    }
    
    data_.assign(data.begin(), data.end());
    return true;
}

void
Observations_array::print(ostream& os) const {

    // Base class print function
    Observations::print(os);
    os << endl;

    // Print data in derived class
    os << "Array Shape = ";
    for (size_t i = 0; i < 3; i++) {
        os << "[" << data_.shape()[i] << "]";
    }
    os << endl;

    auto sizes = data_.shape();

    for (size_t m = 0; m < sizes[0]; m++) {

        os << "[ " << m << ", , ]" << endl;

        for (size_t p = 0; p < sizes[2]; p++) {
            os << "\t[,," << p << "]";
        }
        os << endl;

        for (size_t o = 0; o < sizes[1]; o++) {
            os << "[," << o << ",]\t";

            for (size_t p = 0; p < sizes[2]; p++) {
                os << data_[m][o][p] << "\t";
            }
            os << endl;

        }
        os << endl;

    }
    os << endl;
}

ostream &
operator<<(std::ostream& os, const Observations_array& obj) {
    obj.print(os);
    return os;
}