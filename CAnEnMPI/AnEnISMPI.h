/* 
 * File:   AnEnISMPI.h
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on March, 2020, 11:10 AM
 */


#ifndef AnEnISMPI_H
#define AnEnISMPI_H

#include "AnEnIS.h"
#include <mpi.h>

/**
 * \class AnEnISMPI
 * 
 * \brief AnEnISMPI provides the functionality to perform AnEnIS with MPI
 */
class AnEnISMPI : public AnEnIS {

public:
    AnEnISMPI();
    AnEnISMPI(const AnEnISMPI& orig);
    AnEnISMPI(const Config &);

    virtual ~AnEnISMPI();

    // Using AnEnIS wrapper function compute
    using AnEnIS::compute;

    /**
     * Overloads AnEnIS::compute with test and search indices
     */
    virtual void compute(const Forecasts & forecasts,
            const Observations & observations,
            std::vector<std::size_t> & fcsts_test_index,
            std::vector<std::size_t> & fcsts_search_index) override;

private:
    void gather_(int num_procs, int rank);

    /**
     * Overloads AnEnIS::computeSds_ so that master process does not compute
     * any standard deviation. Only worker processes compute standard deviation.
     */
    virtual void computeSds_(const Forecasts & forecasts,
            const std::vector<std::size_t> & times_fixed_index,
            const std::vector<std::size_t> & times_accum_index = {}) override;
};

#endif /* AnEnISMPI_H */
