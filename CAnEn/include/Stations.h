/* 
 * File:   Stations.h
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <cervone@psu.edu>
 *
 * Created on April 17, 2018, 10:41 PM
 */

#ifndef BOOST_NO_AUTO_PTR
#define BOOST_NO_AUTO_PTR
#endif

#include <boost/multi_index/random_access_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/tag.hpp>

#include <vector>
#include <string>
#include <cmath>
#include <iostream>

#ifndef STATIONS_H
#define STATIONS_H

/**
 * The anenSta name space is created for classes Station and Stations.
 */
namespace anenSta {

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
        Station(std::string);
        Station(double, double);
        Station(std::string, double, double);
        Station(Station const &);

        virtual ~Station();

        Station & operator=(const Station &);
        bool operator==(const Station &) const;
        bool operator!=(const Station &) const;
        bool operator<(const Station &) const;
        bool compare(const Station &) const;

        std::string getName() const;
        std::size_t getID() const;
        double getY() const;
        double getX() const;

        void setName(std::string name);
        void setX(double x);
        void setY(double y);

        void print(std::ostream &) const;
        friend std::ostream& operator<<(std::ostream&, Station const &);

        static std::size_t _countIDs() {
            return _static_ID_;
        };

    private:
        void setID_();

        /**
         * This is a unique identifier that is used to keep track of stations.
         */
        std::size_t ID_;

        std::string name_ = "UNDEFINED";
        double x_ = NAN;
        double y_ = NAN;

        /**
         * Static variable for serial number identification.
         */
        static size_t _static_ID_;
    };

    struct by_insert {
        /**
         * The tag for insertion sequence indexing
         */
    };

    struct by_ID {
        /** 
         * The tag for ID-based indexing
         */
    };

    struct by_x {
        /** 
         * The tag for the order based on x
         */
    };

    struct by_y {
        /** 
         * The tag for the order based on y
         */
    };

    /**
     * Base class for Stations
     */
    using multiIndexStations = boost::multi_index_container<
            Station,

            boost::multi_index::indexed_by<

            // Order by insertion
            boost::multi_index::random_access<
            boost::multi_index::tag<by_insert> >,

            // Order by ID
            boost::multi_index::hashed_unique<
            boost::multi_index::tag<by_ID>,
            boost::multi_index::const_mem_fun<
            Station, std::size_t, &Station::getID> >,

            // Order by x
            boost::multi_index::ordered_non_unique<
            boost::multi_index::tag<by_x>,
            boost::multi_index::const_mem_fun<
            Station, double, &Station::getX> >,

            // Order by y
            boost::multi_index::ordered_non_unique<
            boost::multi_index::tag<by_y>,
            boost::multi_index::const_mem_fun<
            Station, double, &Station::getY> >
            > >;

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
    class Stations : public multiIndexStations {
    public:
        Stations();
        virtual ~Stations();

        std::size_t getStationIndex(std::size_t station_ID) const;
        std::vector<std::size_t> getStationsIndex(
                const std::vector<std::size_t> & stations_ID) const;

        std::vector<std::size_t> getStationsIdBySquare(double main_station_x,
                double main_station_y, double half_edge) const;

        std::vector<std::size_t> getStationsIdByDistance(double main_station_x,
                double main_station_y, double radius) const;

        std::vector<std::size_t> getNearestStationsId(double main_station_x,
                double main_station_y, std::size_t num_stations,
                double threshold = 0) const;

        void print(std::ostream &) const;
        friend std::ostream& operator<<(std::ostream&, Stations const &);
    };
}

#endif /* STATIONS_H */

