/* 
 * File:   Stations.h
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <weiming@psu.edu>
 *
 * Created on April 17, 2018, 10:41 PM
 */

#ifndef STATIONS_H
#define STATIONS_H

#include <iostream>

#include "Config.h"
#include "BmDim.h"

/**
 * \class Station
 * 
 * \brief Class Station stores station information including name, X, and 
 * Y. Each Station Object is assigned with an unique ID. This ID can be
 * useful for station retrieval.
 * 
 */
class Station {
public:
    Station();
    Station(Station const &);
    Station(double, double, std::string name = Config::_NAME);

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
    double x_;
    double y_;
    std::string name_;
};

/**
 * \class Stations
 * 
 * \brief Stations class stores Station objects. It is a bidirectional map
 * implemented from Boost so that it provides fast translation from and to
 * its underlying Station object.
 * 
 * Stations class support the following features:
 * 1. Station is unique;
 * 2. Station objects are kept in sequence of insertion and has random access;
 * 3. Index of a Station object can be quickly retrieved using Station.
 */
class Stations : public BmType<Station> {
public:
    Stations() = default;
    virtual ~Stations() = default;

    void push_back(const Station &);

    /**
     * Shift station longitudes from (0 ~ -360) to (-180 ~ 180)
     */
    void shiftLongitudes();

    std::size_t getIndex(const Station &) const;
    void getIndices(const Stations &, std::vector<std::size_t> &) const;
    const Station & getStation(size_t index) const;
    void getCoordinates(std::vector<double> &, std::vector<double> &) const;
    void getNames(std::vector<std::string> &) const;
    
    void print(std::ostream &) const;
    friend std::ostream& operator<<(std::ostream&, Stations const &);
};

#endif /* STATIONS_H */

