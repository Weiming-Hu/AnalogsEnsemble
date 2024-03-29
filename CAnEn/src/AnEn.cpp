/* 
 * File:   AnEn.cpp
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <weiming@psu.edu>
 * 
 * Created on January 7, 2020, 2:07 PM
 */

#include "AnEn.h"
#include "Functions.h"

using namespace std;

AnEn::AnEn() {
    Config config;
    setMembers_(config);
    profiler_.start();
}

AnEn::AnEn(const AnEn& orig) {
    *this = orig;
    profiler_.start();
}

AnEn::AnEn(const Config & config) {
    setMembers_(config);
    profiler_.start();
}

AnEn::~AnEn() {
}

const Profiler &
AnEn::getProfile() const {
    return profiler_;
}

void
AnEn::print(ostream & os) const {
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

void
AnEn::setMembers_(const Config & config) {
    // Copy the needed by this class from the configuration
    verbose_ = config.verbose;
    return;
}
