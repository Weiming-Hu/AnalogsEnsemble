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

#if defined(_ENABLE_AI)
#include <torch/script.h>
#endif

/**
 * SimsVec is a vector of arrays. SimsVec is used to store all similarity
 * metrics and their corresponding indices for one particular test forecast
 * with all the search forecasts.
 * 
 * This is a template because the length of the array can change. The first
 * value is the similarity metric and the rest contains different index. In
 * AnEnIS, indices including observation time and forecast time indices are
 * saves, and therefore the length will later be set to 3.
 */
template <std::size_t len>
using SimsVec = std::vector< std::array<double, len> >;

/**
 * \class AnEnIS
 * 
 * \brief AnEnIS is an implementation of the class AnEn for the independent 
 * search algorithm.
 */
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

    virtual void print(std::ostream &) const override;
    friend std::ostream& operator<<(std::ostream&, const AnEnIS &);

    AnEnIS & operator=(const AnEnIS & rhs);
    
    /*
     * Member variable getter functions
     */
    std::size_t num_analogs() const;
    std::size_t num_sims() const;
    std::size_t obs_var_index() const;
    std::size_t max_par_nan() const;
    std::size_t max_flt_nan() const;
    std::size_t flt_radius() const;
    bool save_analogs() const;
    bool save_analogs_time_index() const;
    bool save_sims() const;
    bool save_sims_time_index() const;
    bool operation() const;
    bool quick_sort() const;
    bool prevent_search_future() const;
    const std::vector<double> & weights() const;
    const Array4DPointer & sds() const;
    const Array4DPointer & sims_metric() const;
    const Array4DPointer & sims_time_index() const;
    const Array4DPointer & analogs_value() const;
    const Array4DPointer & analogs_time_index() const;
    const Functions::Matrix & obs_time_index_table() const;

    /**
     * These variables define what the value is on different positions in the
     * similarity array.
     */
    static const std::size_t _SIM_VALUE_INDEX;
    static const std::size_t _SIM_FCST_TIME_INDEX;
    static const std::size_t _SIM_OBS_TIME_INDEX;

    /**
     * This is the default value for similarity array
     */
    static const std::array<double, 3> _INIT_ARR_VALUE;

#if defined(_ENABLE_AI)
    /**
     * Load a similarity model for AI inference.
     */
    virtual void load_similarity_model(const std::string &);
#endif

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
    
    std::vector<double> weights_;

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
    Functions::Matrix obs_time_index_table_;

    virtual void preprocess_(const Forecasts & forecasts,
            const Observations & observations,
            std::vector<std::size_t> & fcsts_test_index,
            std::vector<std::size_t> & fcsts_search_index);
    
    virtual void allocateMemory_(const Forecasts & forecasts,
            const std::vector<std::size_t> & fcsts_test_index,
            const std::vector<std::size_t> & fcsts_search_index);

    /**
     * This is the function to sort the similarity vector based on the 
     * similarity metric from the array of length 3.
     * 
     * This function is static because it is called by the sorting algorithm
     */
    static bool _simsSort_(const std::array<double, 3> &, const std::array<double, 3> &);

    virtual void setMembers_(const Config &) override;

    virtual void setSdsTimeMap_(const std::vector<std::size_t> & times_accum_index);

    virtual double computeSimMetric_(const Forecasts & forecasts,
            std::size_t sta_search_i, std::size_t sta_test_i,
            std::size_t flt_i, std::size_t time_test_i, std::size_t time_search_i,
            const std::vector<bool> & circulars);

    virtual void allocateSds_(const Forecasts & forecasts,
            const std::vector<std::size_t> & times_fixed_index,
            const std::vector<std::size_t> & times_accum_index = {});
    virtual void computeSds_(const Forecasts & forecasts,
            const std::vector<std::size_t> & times_fixed_index,
            const std::vector<std::size_t> & times_accum_index = {});

    virtual void checkIndexRange_(const Forecasts & forecasts,
            const std::vector<std::size_t> & fcsts_test_index,
            const std::vector<std::size_t> & fcsts_search_index) const;

    virtual void checkConsistency_(const Forecasts & forecasts,
            const Observations & observations) const;

    virtual void checkSave_() const;
    
    virtual void checkNumberOfMembers_(std::size_t num_search_times_index);
    
    /**************************************************************************
     *                          Template Functions                            *
     **************************************************************************/

    template <std::size_t len>
    void saveAnalogs_(const SimsVec<len> & sims_arr, const Observations & observations,
            std::size_t station_i, std::size_t test_time_i, std::size_t flt_i);
    template <std::size_t len>
    void saveAnalogsTimeIndex_(const SimsVec<len> & sims_arr,
            std::size_t station_i, std::size_t test_time_i, std::size_t flt_i);
    template <std::size_t len>
    void saveSims_(const SimsVec<len> & sims_arr,
            std::size_t station_i, std::size_t test_time_i, std::size_t flt_i);
    template <std::size_t len>
    void saveSimsTimeIndex_(const SimsVec<len> & sims_arr,
            std::size_t station_i, std::size_t test_time_i, std::size_t flt_i);

    /**************************************************************************
     *                           AI Integrations                              *
     **************************************************************************/
    bool use_AI_;

#if defined(_ENABLE_AI)
    torch::jit::script::Module similarity_model_;

    virtual double computeSimMetricAI_(const Forecasts & forecasts,
            std::size_t sta_search_i, std::size_t sta_test_i,
            std::size_t flt_i, std::size_t time_test_i, std::size_t time_search_i);
#endif
    
};

#include "AnEnIS.tpp"

#endif /* ANENIS_H */
