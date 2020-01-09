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
check_time_overlap_(_CHECK_TIME_OVERLAP),
save_sims_(_SAVE_SIMS),
verbose_(_VERBOSE) {
}

AnEn::AnEn(const AnEn& orig) {
    if (this != &orig) {
        operational_ = orig.operational_;
        check_time_overlap_ = orig.check_time_overlap_;
        save_sims_ = orig.save_sims_;
        verbose_ = orig.verbose_;
    }
}

AnEn::AnEn(bool operational, bool time_overlap_check,
        bool save_sims, Verbose verbose) :
operational_(operational),
check_time_overlap_(time_overlap_check),
save_sims_(save_sims),
verbose_(verbose) {
}

AnEn::~AnEn() {
}

