/* 
 * File:   Stations.h
 * Author: guido
 *
 * Created on April 17, 2018, 10:41 PM
 */

#include <vector>
#include <string>
#include <cmath>
#include <iostream>
#include <unordered_map>
#include <utility>

#ifndef STATIONS_H
#define STATIONS_H

class Station final {
public:
    Station();
    Station(std::string);
    Station(std::string, double, double);
    Station(Station const &);

    virtual ~Station();

    Station & operator=(const Station &);
    bool operator==(const Station &) const;
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

/**
 * Stations is a protected inheritance from 
 * std::unordered_map<std::size_t, Station> because certain functions, like
 * the insert function, have different behaviors.
 */
class Stations : protected std::unordered_map<std::size_t, Station> {
public:
    Stations();
    virtual ~Stations();

    std::vector<std::size_t> getNearbyStations(const Station &) const;
    std::vector<std::size_t> getNearbyStations(const std::size_t &) const;

    /**
     * Inserts a station into the map with the key set to its ID. By this way,
     * it is ensured that stations are unique in the map with their ID as 
     * the key.
     * 
     * @param station Station object.
     * @return std::pair<iterator, bool> object consisting of an iterator to 
     * the inserted element (or to the element that prevented the insertion) 
     * and a bool denoting whether the insertion took place.
     */
    std::pair<iterator, bool> insert(const Station & station) {
        std::pair<std::size_t, Station> p(station.getID(), station);
        return (std::unordered_map<std::size_t, Station>::insert(p));
    };

    std::size_t size() const std::noexcept override;
    void clear() std::noexcept override;

    /**
     * Gets the reference to the Station based on the ID. If no such 
     * element exists, an exception of type std::out_of_range is thrown.
     * 
     * @param key An size_t key.
     * @return A reference to Station object.
     */
    Station & at(const size_t & key);
    Station & at(const Station & station);
    const Station & at(const size_t & key) const;
    const Station & at(const Station & station) const;

    void print(std::ostream &) const;
    friend std::ostream& operator<<(std::ostream&, Stations const &);
};
#endif /* STATIONS_H */

