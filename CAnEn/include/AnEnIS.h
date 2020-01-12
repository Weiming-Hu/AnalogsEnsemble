/* 
 * File:   AnEnIS.h
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <cervone@psu.edu>
 *
 * Created on January 8, 2020, 11:40 AM
 */

#ifndef ANENIS_H
#define ANENIS_H

#include "AnEn.h"
#include "boost/multi_array.hpp"

template <class T>
using Array4D = boost::multi_array<T, 4>;

class AnEnIS : public AnEn {
public:
    AnEnIS();
    AnEnIS(const AnEnIS& orig);
    AnEnIS(bool operational,
            bool time_overlap_check = AnEnDefaults::_CHECK_TIME_OVERLAP,
            bool save_sims = AnEnDefaults::_SAVE_SIMS,
            AnEnDefaults::Verbose verbose = AnEnDefaults::_VERBOSE);
    virtual ~AnEnIS();

    void compute(const Forecasts & forecasts,
            const Observations & observations,
            const Times & test_times,
            const Times & search_times) override;

protected:
    std::vector<double> weights_;
    std::vector<double> circulars_;
    Array4D<double> sds_;
    Array4D<double> similarityMetric_;
    Array4D<size_t> similarityIndex_;

    void computeSds_(const Forecasts & forecasts,
            const std::vector<size_t> & times_fixed_index,
            const std::vector<size_t> & times_running_index = {});
    void computeSd_(const Forecasts & forecasts,
            const std::vector<size_t> & times_index,
            size_t time_i, size_t par_i, size_t sta_i, size_t flt_i,
            size_t count = 0);
};

#endif /* ANENIS_H */