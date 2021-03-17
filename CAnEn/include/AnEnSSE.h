/* 
 * File:   AnEnSSE.h
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <cervone@psu.edu>
 *
 * Created on January 29, 2020, 5:08 PM
 */


#ifndef ANENSSE_H
#define ANENSSE_H

#include "AnEnIS.h"

/**
 * \class AnEnSSE
 * 
 * \brief AnEnSSE is an implementation of the abstrct class AnEn for the 
 * search space extension algorithm.
 * 
 */
class AnEnSSE : public AnEnIS {
public:
    AnEnSSE();
    AnEnSSE(const AnEnSSE& orig);
    AnEnSSE(const Config & config);
    
    virtual ~AnEnSSE();

    // Using AnEnIS wrapper function compute
    using AnEnIS::compute;

    /**
     * Overloads AnEnIS::compute with test and search indices
     */
    virtual void compute(const Forecasts & forecasts,
            const Observations & observations,
            std::vector<std::size_t> & fcsts_test_index,
            std::vector<std::size_t> & fcsts_search_index) override;

    virtual void print(std::ostream &) const override;
    friend std::ostream& operator<<(std::ostream&, const AnEnSSE &);
    
    AnEnSSE & operator=(const AnEnSSE & rhs);

    /*
     * Member variable getter functions
     */
    std::size_t num_nearest() const;
    double distance() const;
    bool extend_obs() const;
    bool save_sims_station_index() const;
    bool exclude_closest_location() const;
    const Array4DPointer & sims_station_index() const;
    const Functions::Matrix & search_stations_index() const;
    
    /**
     * This variable defines the index of similarity station index in the
     * similarity array.
     */
    static const std::size_t _SIM_STATION_INDEX;
    
    /**
     * This is the default value for similarity array
     */
    static const std::array<double, 4> _INIT_ARR_VALUE;

protected:
    
    /**
     * The number of nearest neighbors to search
     */
    std::size_t num_nearest_;

    /**
     * A distance threshold when finding nearest neighbors
     */
    double distance_;

    /**
     * Whether analog member observations should come from its current station
     * or the searched station.
     */
    bool extend_obs_;
    
    /**
     * Whether to save the search station indices for each similarity
     */
    bool save_sims_station_index_;

    /**
     * Whether to exclude the current station from the search stations.
     * This is helpful if users want to search through every other
     * nearby stations rather than itself (e.g. the closest station).
     */
    bool exclude_closest_location_;

    /**
     * The array to store search station indices.
     */
    Array4DPointer sims_station_index_;

    /**
     * A table to record the search stations for each test station. Each row is
     * a set of search station indices for a particular test station.
     */
    Functions::Matrix search_stations_index_;

    virtual void preprocess_(const Forecasts & forecasts,
            const Observations & observations,
            std::vector<std::size_t> & fcsts_test_index,
            std::vector<std::size_t> & fcsts_search_index) override;
    
    virtual void allocateMemory_(const Forecasts & forecasts,
            const std::vector<std::size_t> & fcsts_test_index,
            const std::vector<std::size_t> & fcsts_search_index) override;

    /**
     * This is the function to sort the similarity vector based on the 
     * similarity metric from the array of length 4.
     * 
     * This function is static because it is called by the sorting algorithm
     */
    static bool _simsSort_(const std::array<double, 4> &, const std::array<double, 4> &);
    
    virtual void setMembers_(const Config &) override;

    virtual void checkSave_() const override;

    virtual void checkNumberOfMembers_(std::size_t num_search_times_index) override;

    /**************************************************************************
     *                          Template Functions                            *
     **************************************************************************/

    template <std::size_t len>
    void saveAnalogs_(const SimsVec<len> & sims_arr, const Observations & observations,
            std::size_t station_i, std::size_t test_time_i, std::size_t flt_i);
    template <std::size_t len>
    void saveSimsStationIndex_(const SimsVec<len> & sims_arr,
            std::size_t station_i, std::size_t test_time_i, std::size_t flt_i);
};

#include "AnEnSSE.tpp"

#endif /* ANENSSE_H */

