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

class AnEnSSE : public AnEnIS {
public:
    AnEnSSE();
    AnEnSSE(const AnEnSSE& orig);
    AnEnSSE(const Config & config);
    
    virtual ~AnEnSSE();

    virtual void compute(const Forecasts & forecasts,
            const Observations & observations,
            std::vector<std::size_t> & fcsts_test_index,
            std::vector<std::size_t> & fcsts_search_index) override;

    const Array4DPointer & getSimsStationIndex() const;

    virtual void print(std::ostream &) const override;
    friend std::ostream& operator<<(std::ostream&, const AnEnSSE &);
    
    AnEnSSE & operator=(const AnEnSSE & rhs);

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
     * The array to store search station indices.
     */
    Array4DPointer sims_station_index_;

    /**
     * A table to record the search stations for each test station. Each row is
     * a set of search station indices for a particular test station.
     */
    
    // TODO: Do we need a boost matrix
    boost::numeric::ublas::matrix<size_t> search_stations_;
    
    virtual void setConfig_(const Config &) override;
    
    virtual void preprocess_(const Forecasts & forecasts,
            const Observations & observations,
            std::vector<std::size_t> & fcsts_test_index,
            std::vector<std::size_t> & fcsts_search_index) override;

    virtual void checkSave_() const override;

};

#endif /* ANENSSE_H */

