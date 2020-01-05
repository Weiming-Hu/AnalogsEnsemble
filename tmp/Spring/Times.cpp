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
#include "AnEnDefault.h"

#include <iterator>
#include <sstream>
#include <cmath>

using namespace std;

static const int MULTIPLIER = 10000;

size_t roundPrecision(const double& ori) {
    return (std::round(ori * MULTIPLIER));
}

/***************************************************************************
 *                               Times                                     *
 **************************************************************************/

Times::Times() : unit_(AnEnDefault::_UNIT) , origin_(AnEnDefault::_ORIGIN) {
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
    const multiIndexTimes::index<By::value>::type&
            times_by_value = get<By::value>();

    auto it_value = times_by_value.find(
            roundPrecision(timestamp));

    if (it_value != times_by_value.end()) {

        // Project the value-based ordered to insertion sequence
        const auto it_insert = project<By::insert>(it_value);

        // Get the insertion sequence index iterator
        const multiIndexTimes::index<By::insert>::type&
                times_by_insert = get<By::insert>();

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
    ostream_iterator<double> element_itr(os, ", ");
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
    ostream_iterator<double> element_itr(os, ", ");
    copy(begin(), end(), element_itr);
    cout << endl;
}

ostream&
operator<<(ostream& os, FLTs const & obj) {
    obj.print(os);
    return os;
}

