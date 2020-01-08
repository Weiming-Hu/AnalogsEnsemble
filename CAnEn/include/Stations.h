/* 
 * File:   Stations.h
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <cervone@psu.edu>
 *
 * Created on April 17, 2018, 10:41 PM
 */

#ifndef STATIONS_H
#define STATIONS_H

#include <string>
#include <iostream>

#include "AnEnDefaults.h"
#include "BmDim.h"

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
    Station(double, double, std::string name = AnEnDefaults::_NAME);

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
 * \brief Stations class stores Station objects.
 * 
 * Stations class support the following features:
 * 1. Station should be unique in Stations;
 * 2. Station objects are kept in sequence of insertion, and 
 * has random access;
 * 3. Station are accessible via Station ID.
 */
class Stations : public BmType<Station> {
public:
    Stations();
    virtual ~Stations();
    
    void print(std::ostream &) const;
    friend std::ostream& operator<<(std::ostream&, Stations const &);
};

#endif /* STATIONS_H */

