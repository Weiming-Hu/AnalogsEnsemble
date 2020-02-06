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
#include "Config.h"

/**
 * \class AnEn
 * 
 * \brief AnEn is an abstract class that defines the interface for implement
 * analog ensemble generation.
 */
class AnEn {
public:
    AnEn();
    AnEn(const AnEn& orig);
    AnEn(const Config &);
    
    virtual ~AnEn();
    
    virtual void compute(const Forecasts & forecasts,
            const Observations & observations,
            const Times & test_times, const Times & search_times) = 0;
    virtual void compute(const Forecasts & forecasts,
            const Observations & observations,
            std::vector<std::size_t> & fcsts_test_index,
            std::vector<std::size_t> & fcsts_search_index) = 0;
    
    virtual void print(std::ostream &) const;
    friend std::ostream& operator<<(std::ostream&, const AnEn &);
    
    AnEn & operator=(const AnEn & rhs);

protected:
    Verbose verbose_;

    virtual void setMembers_(const Config &);
};

#endif /* ANEN_H */

