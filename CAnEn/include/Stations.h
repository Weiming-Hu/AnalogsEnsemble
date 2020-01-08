/* 
 * File:   Stations.h
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <cervone@psu.edu>
 *
 * Created on April 17, 2018, 10:41 PM
 */

#ifndef STATIONS_H
#define STATIONS_H

//#ifndef BOOST_NO_AUTO_PTR
//#define BOOST_NO_AUTO_PTR
//#endif

//#include <boost/multi_index/random_access_index.hpp>
//#include <boost/multi_index/hashed_index_fwd.hpp>
//#include <boost/multi_index/ordered_index.hpp>
//#include <boost/multi_index/hashed_index.hpp>
//#include <boost/multi_index_container.hpp>
//#include <boost/multi_index/identity.hpp>
//#include <boost/multi_index/mem_fun.hpp>
//#include <boost/multi_index/tag.hpp>
#include "boost/numeric/ublas/matrix.hpp"

#include <vector>
#include <string>
#include <cmath>
#include <iostream>

//#include "By.h"
#include "AnEnDefaults.h"

#include "boost/bimap/vector_of.hpp"
#include "boost/bimap/set_of.hpp"
#include "boost/bimap.hpp"
/**
 * \class Station
 * 
 * \brief Class Station stores station information including name, X, and 
 * Y. Each Station Object is assigned with an unique ID. This ID can be
 * useful for station retrieval.
 * 
 */
class Station final {
public:
    Station();
    Station(Station const &);
    Station(std::string, double x = AnEnDefaults::_X, double = AnEnDefaults::_Y);

    virtual ~Station();

    Station & operator=(const Station &);
    bool operator==(const Station &) const;
    bool operator!=(const Station &) const;
    bool operator<(const Station &) const;

    std::string getName() const;  
    
    double getY() const;
    double getX() const;

    void print(std::ostream &) const;
    friend std::ostream& operator<<(std::ostream&, Station const &);

private:
    std::string name_;
    double x_;
    double y_;
    bool location_initialized_;
    
};

using BmStations = boost::bimap<boost::bimaps::vector_of<size_t>, boost::bimaps::set_of<Station> >;

/**
 * Base class for Stations
 */
//using multiIndexStations = boost::multi_index_container<
//        Station,
//
//        boost::multi_index::indexed_by<
//
//        // Order by insertion
//        boost::multi_index::random_access<
//        boost::multi_index::tag<By::insert> >,
//
//        // Order by name
//        boost::multi_index::hashed_non_unique<
//        boost::multi_index::tag<By::name>,
//        boost::multi_index::const_mem_fun<
//        Station, std::string, &Station::getName> >,
//
//        // Order by x
//        boost::multi_index::ordered_non_unique<
//        boost::multi_index::tag<By::x>,
//        boost::multi_index::const_mem_fun<
//        Station, double, &Station::getX> >,
//
//        // Order by y
//        boost::multi_index::ordered_non_unique<
//        boost::multi_index::tag<By::y>,
//        boost::multi_index::const_mem_fun<
//        Station, double, &Station::getY> >
//        > >;

/**
 * \class Stations
 * 
 * \brief Stations class stores Station objects.
 * 
 * Stations class support the following features:
 * 1. Station should be unique in Stations;
 * 2. Station objects are kept in sequence of insertion, and 
 * has random access;
 * 3. Station are accessible via Station ID.
 */
class Stations : public BmStations {
public:
    Stations();
    virtual ~Stations();


    /**
     * Get nearby station IDs using a square buffer.
     * 
     * @param main_station_x The X of main (center) station.
     * @param main_station_y The Y of main (center) station.
     * @param half_edge The half size of the edge of the square.
     * @return A vector of nearby station IDs.
     */
//    std::vector<std::size_t> getStationsIdBySquare(double main_station_x,
//            double main_station_y, double half_edge) const;

    /**
     * Get nearby station IDs using distance.
     * 
     * @param main_station_x The X of main (center) station.
     * @param main_station_y The Y of main (center) station.
     * @param radius The buffer radius.
     * @return A vector of nearby station IDs.
     */
//    std::vector<std::size_t> getStationsIdByDistance(double main_station_x,
//            double main_station_y, double radius) const;


    /**
     * Get k-nearest station index matrix. The extra constraints are also
     * considered from Euclidean distances and the test/search station tags.
     * 
     * This is a similar function to Stations::getNearestStationsId but that
     * this function gets nearest stations for multiple target stations
     * at once to avoid building the r-tree multiple times. And it takes extra
     * constraints from test/search station tags.
     * 
     * @param i_search_stations AnEn::SearchStationMatrix to store the
     * search station indices for each test station. Each row represents the
     * search station indices for the current test station.
     * @param test_stations The test station container.
     * @param distance The distance to limit the neighbor search.
     * @param num_nearest_stations Number of nearest neighbors.
     * @param test_station_tags A vector for test station tags.
     * @param search_station_tags A vector for search station tags.
     * @return 
     */
//    void getNearestStationsIndex(
//            boost::numeric::ublas::matrix<double> & i_search_stations,
//            const Stations & test_stations,
//            double threshold, size_t num_nearest_stations,
//            const std::vector<size_t> & test_station_tags,
//            const std::vector<size_t> & search_station_tags) const;

    /**
     * Check whether x and y are provided for all stations.
     * @return A boolean.
     */
    //bool haveXY() const;

    void print(std::ostream &) const;
    friend std::ostream& operator<<(std::ostream&, Stations const &);
};

#endif /* STATIONS_H */

