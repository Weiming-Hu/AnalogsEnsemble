/* 
 * File:   AnEn.cpp
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <cervone@psu.edu>
 * 
 * Created on January 7, 2020, 2:07 PM
 */

#include "AnEn.h"

using namespace AnEnDefaults;

AnEn::AnEn() : operational_(_OPERATIONAL),
check_search_future_(_CHECK_SEARCH_FUTURE),
save_sims_(_SAVE_SIMS),
verbose_(_VERBOSE) {
}

AnEn::AnEn(const AnEn& orig) {
    if (this != &orig) {
        operational_ = orig.operational_;
        check_search_future_ = orig.check_search_future_;
        save_sims_ = orig.save_sims_;
        verbose_ = orig.verbose_;
    }
}

AnEn::AnEn(bool operational, bool check_search_future,
        bool save_sims, Verbose verbose) :
operational_(operational),
check_search_future_(check_search_future),
save_sims_(save_sims),
verbose_(verbose) {
}

AnEn::~AnEn() {
}

