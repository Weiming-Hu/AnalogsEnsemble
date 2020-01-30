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
#include "Array4D.h"

#include <unordered_map>

class AnEnIS : public AnEn {
public:
    AnEnIS();
    AnEnIS(const AnEnIS& orig);
    AnEnIS(std::size_t num_members,
            bool operational = AnEnDefaults::_OPERATIONAL,
            bool prevent_search_future = AnEnDefaults::_PREVENT_SEARCH_FUTURE,
            bool save_sims = AnEnDefaults::_SAVE_SIMS,
            AnEnDefaults::Verbose verbose = AnEnDefaults::_VERBOSE,
            std::size_t obs_var_index = AnEnDefaults::_OBS_VAR_INDEX,
            bool quick_sort = AnEnDefaults::_QUICK_SORT,
            bool save_sims_index = AnEnDefaults::_SAVE_SIMS_INDEX,
            bool save_analogs_index = AnEnDefaults::_SAVE_ANALOGS_INDEX,
            std::size_t num_sims = AnEnDefaults::_NUM_SIMS,
            std::size_t max_par_nan = AnEnDefaults::_MAX_PAR_NAN,
            std::size_t max_flt_nan = AnEnDefaults::_MAX_FLT_NAN,
            std::size_t flt_radius = AnEnDefaults::_FLT_RADIUS);

    virtual ~AnEnIS();

    virtual void compute(const Forecasts & forecasts,
            const Observations & observations,
            const Times & test_times,
            const Times & search_times) override;
    virtual void compute(const Forecasts & forecasts,
            const Observations & observations,
            std::vector<std::size_t> & fcsts_test_index,
            std::vector<std::size_t> & fcsts_search_index) override;

    const Array4D & getSimsValue() const;
    const Array4D & getSimsIndex() const;
    const Array4D & getAnalogsValue() const;
    const Array4D & getAnalogsIndex() const;

    void print(std::ostream &) const;
    friend std::ostream& operator<<(std::ostream&, const AnEnIS &);
    AnEnIS & operator=(const AnEnIS & rhs);

protected:
    bool quick_sort_;
    bool save_sims_index_;
    bool save_analogs_index_;

    std::size_t obs_var_index_;
    std::size_t num_sims_;
    std::size_t num_analogs_;
    std::size_t max_par_nan_;
    std::size_t max_flt_nan_;
    std::size_t flt_radius_;
    
    /**
     * The standard deviation array has the following dimensions
     * 
     * [Parameters][Stations][FLTs][Times]
     */
    Array4D sds_;
    
    /**
     * The time index map is used in operational mode. The key is
     * the test time and the the value is the corresponding index
     * of the time dimension in the standard deviation array.
     */
    std::unordered_map<std::size_t, std::size_t> sds_time_index_map_;

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


    double computeSimMetric_(const Forecasts & forecasts, 
            std::size_t sta_search_i, std::size_t sta_test_i,
            std::size_t flt_i, std::size_t time_test_i, std::size_t time_search_i,
            const std::vector<double> & weights,
            const std::vector<bool> & circulars);
    
     virtual void preprocess_(const Forecasts & forecasts,
            const Observations & observations,
            std::vector<std::size_t>  & fcsts_test_index,
            std::vector<std::size_t> & fcsts_search_index);

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
            const std::vector<std::size_t> & times_fixed_index,
            const std::vector<std::size_t> & times_accum_index = {});
    
    void setSdsTimeMap_(const std::vector<std::size_t> & times_accum_index);

    void checkIndexRange_(const Forecasts & forecasts,
            const std::vector<std::size_t> & fcsts_test_index,
            const std::vector<std::size_t> & fcsts_search_index) const;
    
    void checkConsistency_(const Forecasts & forecasts,
            const Observations & observations) const;
};

#endif /* ANENIS_H */
