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
#include "boost/lambda/lambda.hpp"

// Reference for Boost Geometry
// https://www.boost.org/doc/libs/1_68_0/libs/geometry/doc/html/index.html
//
#include "boost/geometry.hpp"
#include "boost/geometry/geometries/point.hpp"
#include <boost/geometry/geometries/ring.hpp>
#include "boost/geometry/index/rtree.hpp"

#include <iterator>
#include <algorithm>

namespace anenSta {
    using namespace std;
    namespace bg = boost::geometry;
    namespace bgi = boost::geometry::index;

    /**************************************************************************
     *                            Station                                     *
     **************************************************************************/

    size_t Station::_static_ID_ = 0;

    Station::Station() {
        setID_();
    }

    Station::Station(string name) :
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
    Station::operator!=(const Station & rhs) const {
        bool result = !(*this == rhs); // Reuse equals operator
        return result;
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
            throw out_of_range("Can't find the station ID "
                    + to_string(station_ID));
        }
    }

    vector<size_t>
    Stations::getStationsIndex(const vector<size_t> & stations_ID) const {
        auto & stations_by_ID = this->get<by_ID>();
        auto & stations_by_insert = this->get<by_insert>();
        vector<size_t> stations_index(stations_ID.size());

        transform(stations_ID.begin(), stations_ID.end(), stations_index.begin(),
                [&stations_by_ID, &stations_by_insert, this](size_t id) {
                    auto it_ID = stations_by_ID.find(id);
                    if (it_ID != stations_by_ID.end()) {
                        return (distance(stations_by_insert.begin(),
                                this->project<by_insert>(it_ID)));
                    } else {
                        throw out_of_range("Can't find the station ID "
                                + to_string(id));
                    }
                });

        return (stations_index);
    }

    vector<size_t>
    Stations::getStationsIdBySquare(size_t i_main, double half_edge) const {

        using namespace boost::lambda;

        // Get the main station
        auto & stations_by_insert = this->get<by_insert>();
        Station main_station(stations_by_insert[i_main]);
        double main_station_x = main_station.getX(),
                main_station_y = main_station.getY();

        // Order the stations by x
        auto & stations_by_x = this->get<by_x>();
        auto stations_range_x = stations_by_x.range(
                main_station_x - half_edge <= boost::lambda::_1,
                boost::lambda::_1 <= main_station_x + half_edge);

        // Put the stations subset by x into a new stations container
        Stations stations_subset;
        stations_subset.insert(stations_subset.end(),
                stations_range_x.first, stations_range_x.second);

        // Remove the main station
        stations_subset.get<by_ID>().erase(main_station.getID());

        // Order the subset station by y
        auto & stations_by_y = stations_subset.get<by_y>();
        auto stations_range_y = stations_by_y.range(
                main_station_y - half_edge <= boost::lambda::_1,
                boost::lambda::_1 <= main_station_y + half_edge);

        // Record the ID of stations that are left in the container
        vector<size_t> search_stations_ID;
        for_each(stations_range_y.first, stations_range_y.second,
                [&search_stations_ID](const Station & station) {
                    search_stations_ID.push_back(station.getID());
                });

        return (search_stations_ID);
    }

    vector<size_t>
    Stations::getStationsIdByDistance(size_t i_main, double radius) const {

        // Get the main station
        auto & stations_by_insert = this->get<by_insert>();
        Station main_station(stations_by_insert[i_main]);
        double main_station_x = main_station.getX(),
                main_station_y = main_station.getY();

        // Use the range function to get a square area of the stations
        vector<size_t> search_stations_ID,
                search_stations_ID_by_square =
                getStationsIdBySquare(i_main, radius);

        vector<double> distances(search_stations_ID_by_square.size());
        auto & stations_by_ID = this->get<by_ID>();

        // We compare the distance squared rather than the actual distance
        // for a better performance
        //
        radius *= radius;

        // With in the subset stations, compute distances extensively
        // and select the ones within the radius
        //
        for (size_t i = 0; i < distances.size(); i++) {
            Station station(
                    *(stations_by_ID.find(search_stations_ID_by_square[i])));
            distances[i] =
                    pow(main_station_x - station.getX(), 2) +
                    pow(main_station_y - station.getY(), 2);

            if (distances[i] <= radius) {
                search_stations_ID.push_back(
                        search_stations_ID_by_square[i]);
            }
        }

        return (search_stations_ID);
    }

    vector<size_t>
    Stations::getNearestStationsId(size_t i_main,
            size_t num_stations, double threshold) const {
        using point = bg::model::point<double, 2, bg::cs::cartesian>;
        using value = pair<point, size_t>;

        bgi::rtree< value, bgi::dynamic_rstar > rtree(
                bgi::dynamic_rstar(this->size()));

        // Get the main station
        auto & stations_by_insert = this->get<by_insert>();
        Station main_station(stations_by_insert[i_main]);
        double main_station_x = main_station.getX(),
                main_station_y = main_station.getY();

        // Build the R-tree
        for (size_t i = 0; i < stations_by_insert.size(); i++) {
            point pt(stations_by_insert[i].getX(),
                    stations_by_insert[i].getY());
            rtree.insert(make_pair(pt, stations_by_insert[i].getID()));
        }

        // Remove the main station
        rtree.remove(make_pair(point(main_station_x, main_station_y),
                main_station.getID()));

        // KNN request
        vector<value> results_points;
        if (isnan(threshold)) {
            rtree.query(bgi::nearest(point(main_station_x, main_station_y),
                    num_stations), back_inserter(results_points));
        } else {
            threshold *= threshold;
            rtree.query(bgi::nearest(point(main_station_x, main_station_y),
                    num_stations) && bgi::satisfies(
                    [&main_station_x, &main_station_y, &threshold]
                    (value const& p) {
                        return (pow(main_station_x - bg::get<0>(p.first), 2) +
                                pow(main_station_y - bg::get<1>(p.first), 2) <
                                threshold); }), back_inserter(results_points));
        }

        // Convert value to ID
        vector<size_t> results_ID(results_points.size());
        for (size_t i = 0; i < results_points.size(); i++) {

            results_ID[i] = results_points[i].second;
        }

        return (results_ID);
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