/* 
 * File:   Time.h
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <cervone@psu.edu>
 *
 * Created on April 18, 2018, 12:33 AM
 */

#ifndef TIME_H
#define TIME_H

//#include "By.h"

#include <string>
#include <iostream>
#include <set>

//#ifndef BOOST_NO_AUTO_PTR
//#define BOOST_NO_AUTO_PTR
//#endif
//
//#include <boost/multi_index/tag.hpp>
//#include <boost/multi_index/mem_fun.hpp>
//#include <boost/multi_index/identity.hpp>
//#include <boost/multi_index_container.hpp>
//#include <boost/multi_index/global_fun.hpp>
//#include <boost/multi_index/hashed_index.hpp>
//#include <boost/multi_index/random_access_index.hpp>

/**
 * Although times are implemented as double container, the precision is not
 * exactly the same as double precious. This is because of two reasons:
 * 
 * - Double values are generally not recommended to be hashed because
 * hash values are used to look up the exact element, and double values are
 * too precise to be hashed;
 * - Sometimes when reading from NetCDF files, values can be float and double.
 * If exact match is desired, values from float will never be able to match
 * double values because of the difference in precision;
 * 
 * Therefore, this function is provided to define a somewhat less precise
 * precision level for times, as a balance between performance and precision.
 * 
 * The function simply multiply the double value with a multiplier and then
 * disregard the decimals.
 * 
 * @param ori The original double value.
 * @return The multiplied and truncated size_t value.
 */
//size_t roundPrecision(const double& ori);

class Time  {

public:
    Time();
    Time( double );
    Time( const Time & );
    
    virtual ~Time();

//  This is where we store the actual data
    double timestamp;
    
    Time & operator=(const Time & rhs);
    Time & operator=(double rhs);
    bool operator<(const Time &) const;
 
    // Static they are the same for each object
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

protected:
};

/**
 * \class FLTs
 * 
 * \brief FLTs class is used to store time information for prediction forecast
 * lead times (FLTs). If a temperature forecast on January 1st, 2028 contains
 * predictions at 00h, 06h, 12h, 18h on that day, this forecast has 4 FLTs,
 * which are 0, 6, 12, 18.
 * 
 * FLTs class supports the following features:
 * 1. Timestamps are unique in FLTs;
 * 2. Timestamps are kept in sequence of insertion, and have random access;
 * 3. Timestamps are accessible via values.
 */
//class FLTs : public Times {
//    void print(std::ostream & os) const;
//    friend std::ostream& operator<<(std::ostream& os, FLTs const & obj);
//};

#endif /* TIME_H */

