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
class AnEnISMPI: public AnEnIS {

public:
    AnEnISMPI();
    AnEnISMPI(const AnEnISMPI& orig);
    AnEnISMPI(const Config &);

    virtual ~AnEnISMPI();

    virtual void compute(const Forecasts & forecasts,
            const Observations & observations,
            std::vector<std::size_t> & fcsts_test_index,
            std::vector<std::size_t> & fcsts_search_index) override;
};

#endif /* AnEnISMPI_H */
