/* 
 * File:   AnEn.cpp
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <cervone@psu.edu>
 * 
 * Created on January 7, 2020, 2:07 PM
 */

#include "AnEn.h"

using namespace AnEnDefaults;

AnEn::AnEn() : verbose_(AnEnDefaults::_VERBOSE) {
}

AnEn::AnEn(const AnEn& orig) {
    verbose_ = orig.verbose_;
}

AnEn::~AnEn() {
}

