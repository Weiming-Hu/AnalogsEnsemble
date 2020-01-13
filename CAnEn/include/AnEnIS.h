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
#include "Functions.h"

using Array4D = boost::multi_array<double, 4>;

class AnEnIS : public AnEn {
public:
    AnEnIS();
    AnEnIS(const AnEnIS& orig);
    AnEnIS(size_t num_members,
            bool operational = AnEnDefaults::_OPERATIONAL,
            bool check_time_overlap = AnEnDefaults::_CHECK_TIME_OVERLAP,
            bool save_sims = AnEnDefaults::_SAVE_SIMS,
            AnEnDefaults::Verbose verbose = AnEnDefaults::_VERBOSE,
            size_t obs_var_index = 0,
            bool quick_sort = true,
            bool save_sims_index = false,
            bool save_analogs_index = false,
            size_t num_sims = 0,
            double max_par_nan = NAN,
            double max_flt_nan = NAN,
            size_t flt_radius = 1);
    
    virtual ~AnEnIS();

    void compute(const Forecasts & forecasts,
            const Observations & observations,
            const Times & test_times,
            const Times & search_times) override;
    void compute(const Forecasts & forecasts,
            const Observations & observations,
            const std::vector<size_t> & fcsts_test_index,
            const std::vector<size_t> & fcsts_search_index);
    
    const Array4D & getSimsValue() const;
    const Array4D & getSimsIndex() const;
    const Array4D & getAnalogsValue() const;
    const Array4D & getAnalogsIndex() const;

    void print(std::ostream &) const;
    friend std::ostream& operator<<(std::ostream&, const AnEnIS &);

protected:
    size_t num_analogs_;
    size_t obs_var_index_;
    bool quick_sort_;
    bool save_sims_index_;
    bool save_analogs_index_;
    size_t num_sims_;
    double max_par_nan_;
    double max_flt_nan_;
    size_t flt_radius_;

    /**
     * [Times][Parameters][Stations][FLTs]
     */
    Array4D sds_;

    /**
     * Arrays for storing similarity information
     */
    Array4D simsIndex_;
    Array4D simsMetric_;

    /**
     * Arrays for storing analog information
     */
    Array4D analogsIndex_;
    Array4D analogsValue_;
    
    /**
     * Matrix for the time index table from forecasts to observations
     */
    Functions::Matrix obsIndexTable_;

    /**
     * A vector of arrays consisting of 
     * [similarity value, forecast time index, observation time index]
     */
    std::vector< std::array<double, 3> > simsArr_;

    double computeSim_(const Forecasts & forecasts, size_t sta_i,
            double flt_i, size_t time_test_i, size_t time_search_i,
            const std::vector<double> & weights,
            const std::vector<bool> & circulars);

    void computeSds_(const Forecasts & forecasts,
            const std::vector<size_t> & times_fixed_index,
            size_t running_pos_start = 0);
    void computeSd_(const Forecasts & forecasts,
            const std::vector<size_t> & times_index,
            size_t time_i, size_t par_i, size_t sta_i, size_t flt_i,
            size_t count = 0);
    
    // TODO: running standard deviation
};

#endif /* ANENIS_H */