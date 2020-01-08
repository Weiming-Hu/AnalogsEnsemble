/* 
 * File:   Time.h
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <cervone@psu.edu>
 *
 * Created on April 18, 2018, 12:33 AM
 */

#ifndef TIME_H
#define TIME_H

#include <string>
#include <iostream>

#include "boost/bimap/vector_of.hpp"
#include "boost/bimap/set_of.hpp"
#include "boost/bimap.hpp"

class Time {
public:
    Time();
    Time(double);
    Time(const Time &);
    virtual ~Time();

    // This is where we store the actual data
    double timestamp;

    Time & operator=(const Time & rhs);
    Time & operator=(double rhs);
    bool operator<(const Time &) const;

    // They are static members because unit and origin should be
    // consistent throughout the program.
    //
    static std::string unit;
    static std::string origin;

    void print(std::ostream & os) const;
    friend std::ostream& operator<<(std::ostream& os, Time const & obj);
};

using BmTimes = boost::bimap<boost::bimaps::vector_of<size_t>, boost::bimaps::set_of<Time> >;

/**
 * \class Times
 * 
 * \brief Times class is used to store Time. It is a bidirectional map
 * implemented from Boost so that it provides fast translation from and to
 * its underlying Time object.
 * 
 * Times class supports the following features:
 * 1. Time is unique;
 * 2. Time objects are kept in sequence of insertion, and have random access;
 * 3. Index of a time object can be quickly retrieved using Time.
 */
class Times : public BmTimes {
public:
    Times();
    virtual ~Times();

    void print(std::ostream & os) const;
    friend std::ostream& operator<<(std::ostream& os, Times const & obj);
};

#endif /* TIME_H */

