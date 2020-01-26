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
            bool prevent_search_future = AnEnDefaults::_PREVENT_SEARCH_FUTURE,
            bool save_sims = AnEnDefaults::_SAVE_SIMS,
            AnEnDefaults::Verbose verbose = AnEnDefaults::_VERBOSE);
    virtual ~AnEn();

    virtual void compute(const Forecasts & forecasts,
            const Observations & observations,
            const Times & test_times, const Times & search_times) = 0;
    virtual void compute(const Forecasts & forecasts,
            const Observations & observations,
            std::vector<std::size_t> fcsts_test_index,
            std::vector<std::size_t> fcsts_search_index) = 0;
    
    virtual void print(std::ostream &) const;
    friend std::ostream& operator<<(std::ostream&, const AnEn &);

protected:
    bool operational_;
    bool prevent_search_future_;
    bool save_sims_;
    AnEnDefaults::Verbose verbose_;
};

#endif /* ANEN_H */

