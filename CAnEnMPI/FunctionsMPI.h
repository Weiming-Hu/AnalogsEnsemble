/* 
 * File:   FunctionsMPI.h
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on March, 2020, 11:10 AM
 */


#ifndef FUNCTIONSMPI_H
#define FUNCTIONSMPI_H

#include <mpi.h>
#include <vector>

#include "Forecasts.h"
#include "Observations.h"

namespace FunctionsMPI {

    int getStartIndex(int total, int num_procs, int rank);
    int getEndIndex(int total, int num_procs, int rank);
    int getSubTotal(int grand_total, int num_procs, int rank);

    void scatterObservations(const Observations & send, Observations & recv, int num_procs, int rank);
    void scatterForecasts(const Forecasts & send, Forecasts & recv, int num_procs, int rank);
    void scatterBasicData(const BasicData & send, BasicData & recv, int num_procs, int rank);
    void scatterArray(const Array4D & send, Array4D & recv, int num_procs, int rank);

    void broadcastVector(const std::vector<bool> & send, std::vector<bool> & recv, int rank);
    void broadcastVector(const std::vector<std::size_t> & send, std::vector<std::size_t> & recv, int rank);

    void collectAnEn(int num_procs, int rank);
};

#endif /* FUNCTIONSMPI_H */
