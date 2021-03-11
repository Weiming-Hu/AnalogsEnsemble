/* 
 * File:   AnEnSSEMS.h
 * Author: Weiming Hu <cervone@psu.edu>
 *
 * Created on Mar. 10, 2021, 17:13
 */

#ifndef ANENSSEMS_H
#define ANENSSEMS_H

#include "AnEnSSE.h"

/**
 * \class AnEnSSEMS
 *
 * \brief AnEnSSEMS is derived from the class AnEnSSE. It is different from the 
 * base class in how forecast and observation stations are matched. In the base
 * class, a same number of forecast and observation stations are expected from
 * input files. This requirement is relaxed in the derived implementation where
 * users can manually specify how forecast and observation stations should be
 * linked.
 */
class AnEnSSEMS : public AnEnSSE {
public:
    AnEnSSEMS();
    AnEnSSEMS(const AnEnSSEMS& orig);
    AnEnSSEMS(const Config & config);

    virtual ~AnEnSSEMS();

    // Using AnEnSSE wrapper function compute
    using AnEnSSE::compute;

    /**
     * Overload AnEnSSE::compute with test and serach indices
     */
    virtual void compute(const Forecasts & forecasts,
            const Observations & observations,
            std::vector<std::size_t> & fcsts_test_index,
            std::vector<std::size_t> & fcsts_search_index) override;

    virtual void compute(const Forecasts & forecasts,
            const Observations & observations,
            std::vector<std::size_t> & fcsts_test_index,
            std::vector<std::size_t> & fcsts_search_index,
            const std::vector<std::size_t> & match_obs_stations_with);

protected:
    std::vector<std::size_t> match_obs_stations_with_;

    virtual void checkConsistency_(const Forecasts & forecasts,
            const Observations & observations) const override;

    virtual void allocateMemory_(const Forecasts & forecasts,
            const std::vector<std::size_t> & fcsts_test_index,
            const std::vector<std::size_t> & fcsts_search_index) override;
};

#endif /* ANENSSEMS_H */

