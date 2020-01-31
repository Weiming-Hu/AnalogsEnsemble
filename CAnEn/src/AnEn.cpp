/* 
 * File:   AnEn.cpp
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <cervone@psu.edu>
 * 
 * Created on January 7, 2020, 2:07 PM
 */

#include "AnEn.h"
#include "Functions.h"

using namespace std;

AnEn::AnEn() {
    Config config;
    setConfig_(config);
}

AnEn::AnEn(const AnEn& orig) {
    *this = orig;
}

AnEn::AnEn(const Config & config) {
    setConfig_(config);
}

AnEn::~AnEn() {
}

void AnEn::print(ostream & os) const {

    os << Config::_VERBOSE << ": " << Functions::vtoi(verbose_) << endl;
    return;
}

ostream &
operator<<(ostream& os, AnEn const & obj) {
    obj.print(os);
    return os;
}

AnEn &
AnEn::operator=(const AnEn& rhs) {
    
    if (this != &rhs) {
        verbose_ = rhs.verbose_;
    }
    
    return *this;
}

void AnEn::setConfig_(const Config & config) {
    // Copy the needed by this class from the configuration
    verbose_ = config.verbose;
    return;
}
