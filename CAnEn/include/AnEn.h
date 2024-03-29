/* 
 * File:   AnEn.h
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <weiming@psu.edu>
 * 
 * Created on January 7, 2020, 2:07 PM
 */

#ifndef ANEN_H
#define ANEN_H

#include "Observations.h"
#include "Forecasts.h"
#include "Profiler.h"
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
    
    /**
     * Computes the analog ensembles.
     * @param forecasts Forecasts
     * @param observations Observations
     * @param test_times Times for test
     * @param search_times Times for search
     */
    virtual void compute(const Forecasts & forecasts,
            const Observations & observations,
            const Times & test_times, const Times & search_times) = 0;
    
    /**
     * Computes the analog ensembles.
     * @param fcsts_test_index A vector of forecast test indices 
     * @param fcsts_search_index A vector of forecast search indices
     */
    virtual void compute(const Forecasts & forecasts,
            const Observations & observations,
            std::vector<std::size_t> & fcsts_test_index,
            std::vector<std::size_t> & fcsts_search_index) = 0;
    
    /**
     * Get the profiling information. The profiler is usually used to
     * time different components of the compute function.
     */
    virtual const Profiler & getProfile() const;

    virtual void print(std::ostream &) const;
    friend std::ostream& operator<<(std::ostream&, const AnEn &);
    
    AnEn & operator=(const AnEn & rhs);

#if defined(_ENABLE_AI)
    /**
     * Load a pretrained model for similarity definition and turn on
     * similarity calculation with the loaded model.
     */
    virtual void load_similarity_model(const std::string &) = 0;
#endif

protected:
    Verbose verbose_;
    Profiler profiler_;


    virtual void setMembers_(const Config &);
};

#endif /* ANEN_H */

