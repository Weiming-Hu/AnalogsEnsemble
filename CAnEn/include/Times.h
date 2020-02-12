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
    
    static const std::string _unit;
    static const std::string _origin;
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
    Times() = default;
    Times(const Times &) = default;
    virtual ~Times() = default;
    
    void push_back(const Time &);

    /**
     * Retrieve the associated index with a Time object.
     * @param time A Time object
     * @return an index
     */
    std::size_t getIndex(const Time & time) const;
    const Time & getTime(std::size_t index) const;
    
    void getTimestamps(std::vector<std::size_t> & timestamps) const;

    void print(std::ostream & os) const;
    friend std::ostream& operator<<(std::ostream& os, Times const & obj);
    
    /**
     * Slice times based on the start and end times. The input strings should 
     * follow
     * 
     * - (1) the delimited format: YYYY-mm-dd HH:MM:SS.SSS
     * - (2) the non delimited iso format: YYYYmmddTHHMMSS
     * 
     * Note that the letter 'T' in the non delimited iso format is a literal letter.
     * 
     * For more documentation, please go to
     * https://www.boost.org/doc/libs/1_72_0/doc/html/date_time/posix_time.html
     * 
     * @param start The string or size_t for start time.
     * @param end The string or size_t for end time.
     * @param sliced_times The Times object to store the sliced times.
     * @param iso_string Whether the strings use the ISO format.
     */
    void operator()(const std::string &, const std::string &, Times &, bool iso_string = false) const;
    void operator()(size_t, size_t, Times &) const;
};

#endif /* TIME_H */

