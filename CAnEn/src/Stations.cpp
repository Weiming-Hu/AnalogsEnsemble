/* 
 * File:   Stations.cpp
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <cervone@psu.edu>
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

#include <stdexcept>
#include <sstream>
#include <algorithm>

#if defined(_OPENMP)
#include <omp.h>
#endif

using namespace std;

/**************************************************************************
 *                            Station                                     *
 **************************************************************************/
Station::Station() :
x_(Config::_X), y_(Config::_Y), name_(Config::_NAME) {
}

Station::Station(double x, double y, string name) :
x_(x), y_(y), name_(name) {
}

Station::Station(Station const & rhs) {
    *this = rhs;
}

Station::~Station() {
}

Station &
        Station::operator=(const Station & rhs) {

    if (this != &rhs) {
        name_ = rhs.getName();
        x_ = rhs.getX();
        y_ = rhs.getY();
    }

    return *this;
}

bool
Station::operator==(const Station & rhs) const {
    if (name_ != rhs.getName()) return false;
    if (x_ != rhs.getX()) return false;
    if (y_ != rhs.getY()) return false;

    return (true);
}

bool
Station::operator!=(const Station & rhs) const {
    bool result = !(*this == rhs); // Reuse equals operator
    return result;
}

bool
Station::operator<(const Station & rhs) const {
    if (x_ != rhs.x_) return (x_ < rhs.x_);
    if (y_ != rhs.y_) return (y_ < rhs.y_);
    return (name_ < rhs.getName());
}

string
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
Station::print(ostream &os) const {
    os << "[Station] Name: " << name_ << ", x: " << x_ << ", y: " << y_;
}

ostream &
operator<<(ostream& os, Station const & obj) {
    obj.print(os);
    return os;
}


/**************************************************************************
 *                            Stations                                    *
 **************************************************************************/

void
Stations::push_back(const Station & station) {
    BmType<Station>::push_back(value_type(size(), station));
    return;
}

void
Stations::shiftLongitudes() {

    // Get coordinates and names
    vector<string> names;
    vector<double> lon, lat;

    getNames(names);
    getCoordinates(lon, lat);

    // Shift longitudes
    // If longitudes are bigger than 180, minus 360
    for_each(lon.begin(), lon.end(), [](double &x) {
            if (x > 360 || x < 0) throw runtime_error("Longitudes exceed limit [0, 360] during shifting");
            if (x > 180) x -= 360;
            return;});

    // Create new Stations
    resize(0);
    for (size_t i = 0; i < names.size(); ++i) {
        Station station(lon[i], lat[i], names[i]);
        push_back(station);
    }

    return;
}

size_t
Stations::getIndex(const Station & station) const {
    auto it = right.find(station);
    if (it == right.end()) {
        ostringstream msg;
        msg << "Station not found: " << station;
        throw range_error(msg.str());
    }
    return it->second;
}

void
Stations::getIndices(const Stations & stations, vector<size_t> & indices) const {
    
    size_t num_stations = stations.size();
    indices.resize(num_stations);

    for (size_t i = 0; i < num_stations; ++i) {
        indices[i] = getIndex(stations.getStation(i));
    }

    return;
}

const Station &
Stations::getStation(size_t index) const {
    return this->left[index].second;
}

void
Stations::getCoordinates(vector<double> & xs, vector<double> & ys) const {
    
    // Resize the vectors
    xs.resize(size());
    ys.resize(size());
    
    // Copy coordinates from stations to vectors
    const auto & end = left.end();
    for (auto it = left.begin(); it != end; ++it) {
        xs[it->first] = it->second.getX();
        ys[it->first] = it->second.getY();
    }
    
    return;
}

void
Stations::getNames(vector<string> & names) const {

    // Resize the vectors
    names.resize(size());

    // Copy coordinates from stations to the name vector
    const auto & end = left.end();
    for (auto it = left.begin(); it != end; ++it) {
        names[it->first] = it->second.getName();
    }

    return;
}

void
Stations::print(ostream & os) const {
    os << "[Stations] size: " << size() << endl;

    for (left_const_iterator it = left.begin(); it < left.end(); it++) {
        os << "[" << it->first << "] " << it->second << endl;
    }

    return;
}

ostream &
operator<<(ostream& os, Stations const & obj) {
    obj.print(os);
    return os;
}
