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
#include "Array4DPointer.h"

#include <unordered_map>

class AnEnIS : public AnEn {
public:
    AnEnIS();
    AnEnIS(const AnEnIS& orig);
    AnEnIS(const Config &);

    virtual ~AnEnIS();

    virtual void compute(const Forecasts & forecasts,
            const Observations & observations,
            const Times & test_times,
            const Times & search_times) override;
    virtual void compute(const Forecasts & forecasts,
            const Observations & observations,
            std::vector<std::size_t> & fcsts_test_index,
            std::vector<std::size_t> & fcsts_search_index) override;

    const Array4DPointer & getSimsValue() const;
    const Array4DPointer & getSimsTimeIndex() const;
    const Array4DPointer & getAnalogsValue() const;
    const Array4DPointer & getAnalogsTimeIndex() const;

    virtual void print(std::ostream &) const override;
    friend std::ostream& operator<<(std::ostream&, const AnEnIS &);

    AnEnIS & operator=(const AnEnIS & rhs);

protected:
    std::size_t num_analogs_;
    std::size_t num_sims_;
    std::size_t obs_var_index_;
    std::size_t max_par_nan_;
    std::size_t max_flt_nan_;
    std::size_t flt_radius_;
    
    bool save_analogs_;
    bool save_analogs_time_index_;
    bool save_sims_;
    bool save_sims_time_index_;
    bool operation_;
    bool quick_sort_;
    bool prevent_search_future_;

    /**
     * The standard deviation array has the following dimensions
     * 
     * [Parameters][Stations][FLTs][Times]
     */
    Array4DPointer sds_;

    /**
     * The time index map is used in operational mode. The key is
     * the test time and the the value is the corresponding index
     * of the time dimension in the standard deviation array.
     */
    std::unordered_map<std::size_t, std::size_t> sds_time_index_map_;

    /**
     * Arrays for storing similarity information
     */
    Array4DPointer sims_metric_;
    Array4DPointer sims_time_index_;

    /**
     * Arrays for storing analog information
     */
    Array4DPointer analogs_value_;
    Array4DPointer analogs_time_index_;

    /**
     * Matrix for the time index table from forecasts to observations
     */
    Functions::Matrix obs_index_table_;
    
    virtual void setConfig_(const Config &) override;

    virtual void preprocess_(const Forecasts & forecasts,
            const Observations & observations,
            std::vector<std::size_t> & fcsts_test_index,
            std::vector<std::size_t> & fcsts_search_index);

    virtual double computeSimMetric_(const Forecasts & forecasts,
            std::size_t sta_search_i, std::size_t sta_test_i,
            std::size_t flt_i, std::size_t time_test_i, std::size_t time_search_i,
            const std::vector<double> & weights, const std::vector<bool> & circulars);
    
    virtual void computeSds_(const Forecasts & forecasts,
            const std::vector<std::size_t> & times_fixed_index,
            const std::vector<std::size_t> & times_accum_index = {});

    virtual void setSdsTimeMap_(const std::vector<std::size_t> & times_accum_index);

    virtual void checkIndexRange_(const Forecasts & forecasts,
            const std::vector<std::size_t> & fcsts_test_index,
            const std::vector<std::size_t> & fcsts_search_index) const;

    virtual void checkConsistency_(const Forecasts & forecasts,
            const Observations & observations) const;
    
    virtual void checkSave_() const;
};

#endif /* ANENIS_H */
