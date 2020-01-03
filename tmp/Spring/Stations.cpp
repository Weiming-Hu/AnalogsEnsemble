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

// Reference for Boost Geometry
// https://www.boost.org/doc/libs/1_68_0/libs/geometry/doc/html/index.html
//
#include "boost/geometry.hpp"
#include "boost/geometry/geometries/point.hpp"
#include <boost/geometry/geometries/ring.hpp>
#include "boost/geometry/index/rtree.hpp"

#include "boost/lambda/lambda.hpp"

#include <iterator>
#include <algorithm>

#if defined(_OPENMP)
#include <omp.h>
#endif

using namespace std;

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

/**************************************************************************
 *                            Station                                     *
 **************************************************************************/


Station::Station() :
name_(_DEFAULT_STATION_NAME), x_(_DEFAULT_LOCATION), y_(_DEFAULT_LOCATION) {
}


Station::Station(string name, double x, double y) :
name_(name), x_(x), y_(y) {
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
  
    return(true);
}

bool
Station::operator!=(const Station & rhs) const {
    bool result = !(*this == rhs); // Reuse equals operator
    return result;
}

bool
Station::operator<(const Station & rhs) const {
    return (name_ < rhs.getName() );
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
    os << "[Station] Name: " << name_
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


//vector<size_t>
//Stations::getStationsIdBySquare(double main_station_x,
//        double main_station_y, double half_edge) const {
//
//    using namespace boost::lambda;
//
//    if (std::isnan(main_station_x) || std::isnan(main_station_y))
//        throw runtime_error("X and Y cannot be NA values (getStationsIdBySquare).");
//
//    // Order the stations by x
//    auto & stations_by_x = this->get<By::x>();
//    auto stations_range_x = stations_by_x.range(
//            main_station_x - half_edge <= boost::lambda::_1,
//            boost::lambda::_1 <= main_station_x + half_edge);
//
//    // Put the stations subset by x into a new stations container
//    Stations stations_subset;
//    stations_subset.insert(stations_subset.end(),
//            stations_range_x.first, stations_range_x.second);
//
//    // Order the subset station by y
//    auto & stations_by_y = stations_subset.get<By::y>();
//    auto stations_range_y = stations_by_y.range(
//            main_station_y - half_edge <= boost::lambda::_1,
//            boost::lambda::_1 <= main_station_y + half_edge);
//
//    // Record the ID of stations that are left in the container
//    vector<size_t> search_stations_ID;
//    for_each(stations_range_y.first, stations_range_y.second,
//            [&search_stations_ID](const Station & station) {
//                search_stations_ID.push_back(station.getID());
//            });
//
//    return (search_stations_ID);
//}

//vector<size_t>
//Stations::getStationsIdByDistance(double main_station_x,
//        double main_station_y, double radius) const {
//
//    if (std::isnan(main_station_x) || std::isnan(main_station_y))
//        throw runtime_error("X and Y cannot be NA values (getStationsIdByDistance).");
//
//    // Use the range function to get a square area of the stations
//    vector<size_t> search_stations_ID,
//            search_stations_ID_by_square =
//            getStationsIdBySquare(main_station_x, main_station_y, radius);
//
//    vector<double> distances(search_stations_ID_by_square.size());
//    auto & stations_by_ID = this->get<By::ID>();
//
//    // We compare the distance squared rather than the actual distance
//    // for a better performance
//    //
//    radius *= radius;
//
//    // With in the subset stations, compute distances extensively
//    // and select the ones within the radius
//    //
//    for (size_t i = 0; i < distances.size(); i++) {
//        Station station(
//                *(stations_by_ID.find(search_stations_ID_by_square[i])));
//        distances[i] =
//                pow(main_station_x - station.getX(), 2) +
//                pow(main_station_y - station.getY(), 2);
//
//        if (distances[i] <= radius) {
//            search_stations_ID.push_back(
//                    search_stations_ID_by_square[i]);
//        }
//    }
//
//    return (search_stations_ID);
//}


void
Stations::getNearestStationsIndex(
        boost::numeric::ublas::matrix<double> & i_search_stations,
        const Stations & test_stations,
        double threshold, size_t num_nearest_stations,
        const vector<size_t> & test_station_tags,
        const vector<size_t> & search_station_tags) const {

    // Sanity checks
    if (test_station_tags.size() != test_stations.size()) {
        throw runtime_error("The number of tags is different from the number of test stations.");
    }

    if (search_station_tags.size() != this->size()) {
        throw runtime_error("The number of tags is different from the number of search stations.");
    }

    if (i_search_stations.size2() < num_nearest_stations) {
        throw runtime_error("Not enough space in the matrix to store the neighbors.");
    }

    using point = bg::model::point<double, 2, bg::cs::cartesian>;
    using value = pair<point, size_t>;

    bgi::rtree< value, bgi::dynamic_rstar > rtree(
            bgi::dynamic_rstar(this->size()));

    // Build the R-tree
    const auto & search_stations_by_insert = this->get<By::insert>();
    for (size_t i = 0; i < search_stations_by_insert.size(); i++) {
        point pt(search_stations_by_insert[i].getX(),
                search_stations_by_insert[i].getY());

        // I associate each point with its index
        rtree.insert(make_pair(pt, i));
    }

    const auto & test_stations_by_insert = test_stations.get<By::insert>();

    threshold *= threshold;

#if defined(_OPENMP)
#pragma omp parallel for schedule(static) \
shared(i_search_stations, test_stations_by_insert, rtree, threshold,\
num_nearest_stations, test_station_tags, search_station_tags)
#endif
    for (size_t i_test = 0; i_test < test_stations_by_insert.size(); i_test++) {

        double main_station_x = test_stations_by_insert[i_test].getX(),
                main_station_y = test_stations_by_insert[i_test].getY();

        // Sanity check
        if (std::isnan(main_station_x) || std::isnan(main_station_y))
            throw runtime_error("X and Y cannot be NA values (getNearestStationsIndex).");

        // KNN request
        vector<value> results_points;

        if (threshold > 0) {

            // This complicate query deals with nearest neighbor, distance,
            // and station tags.
            //
            rtree.query(bgi::nearest(point(main_station_x, main_station_y),
                    num_nearest_stations) && bgi::satisfies(
                    [&main_station_x, &main_station_y, &threshold]
                    (value const& p) {
                        return (pow(main_station_x - bg::get<0>(p.first), 2) +
                                pow(main_station_y - bg::get<1>(p.first), 2) <
                                threshold); }) && bgi::satisfies(
                    [&test_station_tags, &search_station_tags, &i_test]
                    (value const& p) {
                        return (test_station_tags[i_test] == search_station_tags[p.second]);
                    }), back_inserter(results_points));
        } else {

            // This complicate query deals with nearest neighbor and station tags.
            rtree.query(bgi::nearest(point(main_station_x, main_station_y),
                    num_nearest_stations) && bgi::satisfies(
                    [&test_station_tags, &search_station_tags, &i_test]
                    (value const& p) {
                        return (test_station_tags[i_test] == search_station_tags[p.second]);
                    }), back_inserter(results_points));
        }

        // Assign search stations to matrix
        for (size_t i_search = 0; i_search < i_search_stations.size2() &&
                i_search < results_points.size(); i_search++) {
            i_search_stations(i_test, i_search) = results_points[i_search].second;
        }
    }

    return;
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
