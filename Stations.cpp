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
#include <iterator>
#include <algorithm>

namespace anenSta {
    using namespace std;

    /**************************************************************************
     *                            Station                                     *
     **************************************************************************/

    size_t Station::_static_ID_ = 0;

    Station::Station() {
        setID_();
    }

    Station::Station(std::string name) :
    name_(name) {
        setID_();
    }

    Station::Station(string name, double x, double y) :
    name_(name), x_(x), y_(y) {
        setID_();
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
            ID_ = rhs.getID();
        }

        return *this;
    }

    bool
    Station::operator==(const Station & rhs) const {
        return (ID_ == rhs.getID());
    }

    bool
    Station::operator<(const Station & rhs) const {
        return ID_ < rhs.getID();
    }

    bool
    Station::compare(const Station & rhs) const {

        if (name_ != rhs.getName()) return false;
        if (x_ != rhs.getX()) return false;
        if (y_ != rhs.getY()) return false;

        return true;
    }

    void
    Station::setID_() {
        ID_ = Station::_static_ID_;
        Station::_static_ID_++;
    }

    size_t
    Station::getID() const {
        return ID_;
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
    Station::setName(string name) {
        this->name_ = name;
    }

    void
    Station::setX(double x) {
        this->x_ = x;
    }

    void
    Station::setY(double y) {
        this->y_ = y;
    }

    void
    Station::print(ostream &os) const {
        os << "[Station] ID: " << ID_ << ", Name: " << name_
                << ", x: " << x_ << ", y: " << y_
                << endl;
    }

    ostream &
    operator<<(ostream& os, Station const & obj) {
        obj.print(os);
        return os;
    }

    /**************************************************************************
     *                            Stations                                    *
     **************************************************************************/

    Stations::Stations() {
    }

    Stations::~Stations() {
    }
    
    size_t
    Stations::getStationIndex(size_t station_ID) const {
        
        // Find the station ID in ID-based index
        const multiIndexStations::index<by_ID>::type &
                stations_by_ID = get<by_ID>();

        auto it_ID = stations_by_ID.find(station_ID);

        if (it_ID != stations_by_ID.end()) {

            // Project the ID-based order to insertion sequence
            auto it_insert = project<by_insert>(it_ID);

            // Get the insertion sequence index iterator
            const multiIndexStations::index<by_insert>::type&
                    stations_by_insert = get<by_insert>();

            // Compute the distance
            return (distance(stations_by_insert.begin(), it_insert));
        } else {
            throw out_of_range("Can't find the station IDs "
                    + to_string(station_ID));
        }
    }

    vector<size_t>
    Stations::getNearbyStationsID(const size_t & ID) const {
        vector<size_t> nearby_stations {ID};
        return (nearby_stations);
    }

    void
    Stations::print(ostream & os) const {
        os << "[Stations] size: " << size() << endl;
        copy(begin(), end(), ostream_iterator<Station>(os));
    }

    ostream &
    operator<<(ostream& os, Stations const & obj) {
        obj.print(os);
        return os;
    }
}