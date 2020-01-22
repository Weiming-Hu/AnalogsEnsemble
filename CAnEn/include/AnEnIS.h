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
            bool check_search_future = AnEnDefaults::_CHECK_SEARCH_FUTURE,
            bool save_sims = AnEnDefaults::_SAVE_SIMS,
            AnEnDefaults::Verbose verbose = AnEnDefaults::_VERBOSE,
            size_t obs_var_index = AnEnDefaults::_OBS_VAR_INDEX,
            bool quick_sort = AnEnDefaults::_QUICK_SORT,
            bool save_sims_index = AnEnDefaults::_SAVE_SIMS_INDEX,
            bool save_analogs_index = AnEnDefaults::_SAVE_ANALOGS_INDEX,
            size_t num_sims = AnEnDefaults::_NUM_SIMS,
            size_t max_par_nan = AnEnDefaults::_MAX_PAR_NAN,
            size_t max_flt_nan = AnEnDefaults::_MAX_FLT_NAN,
            size_t flt_radius = AnEnDefaults::_FLT_RADIUS);

    virtual ~AnEnIS();

    void compute(const Forecasts & forecasts,
            const Observations & observations,
            const Times & test_times,
            const Times & search_times) override;
    void compute(const Forecasts & forecasts,
            const Observations & observations,
            std::vector<size_t> fcsts_test_index,
            std::vector<size_t> fcsts_search_index) override;

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
    size_t max_par_nan_;
    size_t max_flt_nan_;
    size_t flt_radius_;

    /**
     * The standard deviation array has the following dimensions
     * 
     * [Parameters][Stations][FLTs][Times]
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


    double computeSimMetric_(const Forecasts & forecasts, size_t sta_i,
            size_t flt_i, size_t time_test_i, size_t time_search_i,
            const std::vector<double> & weights,
            const std::vector<bool> & circulars);

    /**
     * Compute standard deviation for search forecasts. When the operational
     * mode is true, it requires a valid input for times_accum_index for which
     * the running standard deviation are calculated. Note that this function
     * is very permissive on NAN values that it removes all NAN values during
     * the calculation.
     * 
     * @param forecasts Forecasts
     * @param times_fixed_index Time indices of forecasts for fixed-length
     * standard deviation calculation.
     * @param times_accum_index Time indices of forecasts for accumulative
     * standard deviation calculation.
     */
    void computeSds_(const Forecasts & forecasts,
            const std::vector<size_t> & times_fixed_index,
            const std::vector<size_t> & times_accum_index = {});

    void checkIndexRange_(const Forecasts & forecasts,
            const std::vector<size_t> & fcsts_test_index,
            const std::vector<size_t> & fcsts_search_index) const;
    
    void checkConsistency_(const Forecasts & forecasts,
            const Observations & observations) const;
};

#endif /* ANENIS_H */