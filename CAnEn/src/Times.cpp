/* 
 * File:   Time.cpp
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <cervone@psu.edu>
 *
 * Created on April 18, 2018, 12:33 AM
 * 
 *  "`-''-/").___..--''"`-._
 * (`6_ 6  )   `-.  (     ).`-.__.`)   WE ARE ...
 * (_Y_.)'  ._   )  `._ `. ``-..-'    PENN STATE!
 *   _ ..`--'_..-_/  /--'_.' ,' 
 * (il),-''  (li),'  ((!.-'
 *
 *        Geoinformatics and Earth Observation Laboratory (http://geolab.psu.edu)
 *        Department of Geography and Institute for CyberScience
 *        The Pennsylvania State University
 */

#include "Times.h"
#include <iterator>
#include <sstream>

namespace anenTime {

    using namespace std;

    /***************************************************************************
     *                               Times                                     *
     **************************************************************************/

    Times::Times() {
        stringstream ss;
        ss << _ORIGIN_YEAR << "-" << _ORIGIN_MONTH << "-" << _ORIGIN_DAY;
        origin_ = ss.str();
    }

    Times::Times(string unit) :
    unit_(unit) {
        stringstream ss;
        ss << _ORIGIN_YEAR << "-" << _ORIGIN_MONTH << "-" << _ORIGIN_DAY;
        origin_ = ss.str();
    }

    Times::Times(string unit, string origin) :
    unit_(unit), origin_(origin) {
    }

    Times::~Times() {
    }
    
    Times & Times::operator=(const Times & rhs) {
        if (this != &rhs) {
            multiIndexTimes::operator=(rhs);
            
            unit_ = rhs.unit_;
            origin_ = rhs.origin_;
        }
        
        return *this;
    }

    size_t
    Times::getTimeIndex(double timestamp) const {

        // Find the timestamp in value-ordered index
        const multiIndexTimes::index<by_value>::type&
                times_by_value = get<by_value>();

        auto it_value = times_by_value.find(timestamp);

        if (it_value != times_by_value.end()) {

            // Project the value-based ordered to insertion sequence
            const auto it_insert = project<by_insert>(it_value);

            // Get the insertion sequence index iterator
            const multiIndexTimes::index<by_insert>::type&
                    times_by_insert = get<by_insert>();

            // Compute the distance
            return (distance(times_by_insert.begin(), it_insert));

        } else {
            throw out_of_range("Error: Can't find the index for time " +
                    to_string((long double) timestamp));
        }
    }

    void
    Times::print(ostream &os) const {
        os << "[Time] size: " << size() << endl;
        ostream_iterator<size_t> element_itr(os, ", ");
        copy(begin(), end(), element_itr);
        cout << endl;
    }

    ostream&
    operator<<(ostream& os, Times const & obj) {
        obj.print(os);
        return os;
    }

    /***************************************************************************
     *                                 FLTs                                    *
     **************************************************************************/

    void
    FLTs::print(ostream &os) const {
        os << "[FLT] size: " << size() << endl;
        ostream_iterator< int > element_itr(os, ", ");
        copy(begin(), end(), element_itr);
        cout << endl;
    }

    ostream&
    operator<<(ostream& os, FLTs const & obj) {
        obj.print(os);
        return os;
    }
}
