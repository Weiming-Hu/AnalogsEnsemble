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

class Time  {
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


//
//
///**
// * Base class for Times
// */
//using multiIndexTimes = boost::multi_index_container<
//
//        // This is the base class
//        Time,
//
//        boost::multi_index::indexed_by<
//
//        // Order by insertion sequence
//        boost::multi_index::random_access<
//        boost::multi_index::tag<By::insert> >,
//
//        // Order by value
//        boost::multi_index::hashed_unique<
//        boost::multi_index::tag<By::value>,
//        boost::multi_index::global_fun<
//        const double&, size_t, &roundPrecision> > > >;

/**
 * \class Times
 * 
 * \brief Times class is used to store time information for predictions. By 
 * default this is the number of seconds from the origin January 1st, 1970.
 * This can be customized by changing the default setting of origin and unit.
 * 
 * Times class supports the following features:
 * 1. Timestamps are unique in Times;
 * 2. Timestamps are kept in sequence of insertion, and have random access;
 * 3. Timestamps are accessible via values.
 */
class Times : public std::set< Time > {
public:
    Times();
    virtual ~Times();

    Times & operator=(const Times & rhs);
    
    //size_t getTimeIndex(double timestamp) const;

    void print(std::ostream & os) const;
    friend std::ostream& operator<<(std::ostream& os, Times const & obj);
};

#endif /* TIME_H */

