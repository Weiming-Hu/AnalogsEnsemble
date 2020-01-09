/* 
 * File:   AnEn.h
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <cervone@psu.edu>
 * 
 * Created on January 7, 2020, 2:07 PM
 */

#ifndef ANEN_H
#define ANEN_H

#include "Forecasts.h"
#include "Observations.h"
#include "AnEnDefaults.h"

class AnEn {
public:
    AnEn();
    AnEn(const AnEn& orig);
    AnEn(bool operational,
            bool time_overlap_check = AnEnDefaults::_CHECK_TIME_OVERLAP,
            bool save_sims = AnEnDefaults::_SAVE_SIMS,
            AnEnDefaults::Verbose verbose = AnEnDefaults::_VERBOSE);
    virtual ~AnEn();

    virtual void compute(const Forecasts & forecasts,
            const Observations & observations,
            const Times & test_times, const Times & search_times) = 0;

protected:
    bool operational_;
    bool check_time_overlap_;
    bool save_sims_;
    AnEnDefaults::Verbose verbose_;
};

#endif /* ANEN_H */

