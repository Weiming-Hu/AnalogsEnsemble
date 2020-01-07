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
    virtual ~AnEn();

    virtual void compute(const Forecasts & forecasts,
            const Observations & observations,
            const Times & test_times, const Times & search_times) const = 0;

protected:
    AnEnDefaults::Verbose verbose_;
};

#endif /* ANEN_H */

