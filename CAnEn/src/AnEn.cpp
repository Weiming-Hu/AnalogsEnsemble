/* 
 * File:   AnEn.cpp
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <cervone@psu.edu>
 * 
 * Created on January 7, 2020, 2:07 PM
 */

#include "AnEn.h"
#include "Functions.h"

using namespace AnEnDefaults;

AnEn::AnEn() : operational_(_OPERATIONAL),
prevent_search_future_(_PREVENT_SEARCH_FUTURE),
save_sims_(_SAVE_SIMS),
verbose_(_VERBOSE) {
}

AnEn::AnEn(const AnEn& orig) {
    *this = orig;
}

AnEn::AnEn(bool operational, bool prevent_search_future,
        bool save_sims, Verbose verbose) :
operational_(operational),
prevent_search_future_(prevent_search_future),
save_sims_(save_sims),
verbose_(verbose) {
}

AnEn::~AnEn() {
}

void AnEn::print(std::ostream & os) const {

    os << "operational: " << operational_ << std::endl
            << "prevent searching future observations: " << prevent_search_future_ << std::endl
            << "save similarity: " << save_sims_ << std::endl
            << "verbose: " << Functions::vtoi(verbose_) << std::endl;
    return;
}

std::ostream&
operator<<(std::ostream& os, AnEn const & obj) {
    obj.print(os);
    return os;
}

AnEn &
AnEn::operator=(const AnEn& rhs) {
    if (this != &rhs) {
        operational_ = rhs.operational_;
        prevent_search_future_ = rhs.prevent_search_future_;
        save_sims_ = rhs.save_sims_;
        verbose_ = rhs.verbose_;
    }
    return *this;
}
