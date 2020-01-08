/* 
 * File:   Times.h
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <cervone@psu.edu>
 *
 * Created on April 18, 2018, 12:33 AM
 */

#ifndef TIME_H
#define TIME_H

#include <string>
#include <iostream>

#include "BmDim.h"

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

/**
 * \class Times
 * 
 * \brief Times class is used to store Time. It is a bidirectional map
 * implemented from Boost so that it provides fast translation from and to
 * its underlying Time object.
 * 
 * Times class supports the following features:
 * 1. Time is unique;
 * 2. Time objects are kept in sequence of insertion and have random access;
 * 3. Index of a Time object can be quickly retrieved using Time.
 * 
 * Time objects in Times are assumed to be sorted in ascension order.
 */
class Times : public BmType<Time> {
public:
    Times();
    virtual ~Times();

    size_t getIndex(const Time &) const;

    void print(std::ostream & os) const;
    friend std::ostream& operator<<(std::ostream& os, Times const & obj);
};

#endif /* TIME_H */

