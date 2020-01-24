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
    Time(std::size_t);
    Time(const Time &);
    virtual ~Time();

    Time & operator=(const Time & rhs);
    Time & operator=(std::size_t rhs);
    Time operator+(const Time & rhs) const;
    bool operator<(const Time &) const;

    // This is where we store the actual data as std::size_t.
    std::size_t timestamp;
    
    void print(std::ostream & os) const;
    friend std::ostream& operator<<(std::ostream& os, Time const & obj);
    
    // They are static members because unit and origin should be
    // consistent throughout the program.
    //
    static std::string _unit;
    static std::string _origin;
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

    /**
     * Retrieve the associated index with a Time object.
     * @param time A Time object
     * @return an index
     */
    std::size_t getIndex(const Time & time) const;

    void print(std::ostream & os) const;
    friend std::ostream& operator<<(std::ostream& os, Times const & obj);
};

#endif /* TIME_H */

