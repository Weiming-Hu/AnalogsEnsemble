/* 
 * File:   AnEn.cpp
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <cervone@psu.edu>
 * 
 * Created on January 7, 2020, 2:07 PM
 */

#include "AnEn.h"

using namespace AnEnDefaults;

AnEn::AnEn() : operational_(AnEnDefaults::_OPERATIONAL),
        time_overlap_check_(AnEnDefaults::_CHECK_TIME_OVERLAP),
        verbose_(AnEnDefaults::_VERBOSE) {
}

AnEn::AnEn(const AnEn& orig) {
    if (this != &orig) {
        operational_ = orig.operational_;
        time_overlap_check_ = orig.time_overlap_check_;
        verbose_ = orig.verbose_;
    }
}

AnEn::~AnEn() {
}

