/* 
 * File:   AnEnISMPI.h
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on March, 2020, 11:10 AM
 */


#ifndef AnEnISMPI_H
#define AnEnISMPI_H

#include <mpi.h>
#include "AnEnIS.h"

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

private:
    int getStartIndex_(int total, int rank) const;
    int getEndIndex_(int total, int rank) const;
    int getSubTotal(int total, int rank) const;
    void broadcastForecasts_(const Forecasts & send, Forecasts & recv, int rank) const;
    void broadcastVector_(const std::vector<double> & send, std::vector<double> & recv) const;
    void broadcastByStations_(const double *send, double *recv, int rank) const;
    void collectByStations_(const double *send, double *recv, int rank) const;
};

#endif /* AnEnISMPI_H */

