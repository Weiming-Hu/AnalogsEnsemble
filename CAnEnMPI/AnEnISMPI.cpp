/* 
 * File:   AnEnISMPI.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on March, 2020, 11:10 AM
 */

#include "AnEnISMPI.h"

using namespace std;

AnEnISMPI::AnEnISMPI() : AnEnIS() {
}

AnEnISMPI::AnEnISMPI(const AnEnISMPI & orig) : AnEnIS(orig) {
}

AnEnISMPI::AnEnISMPI(const Config & config) : AnEnIS(config) {
}

AnEnISMPI::~AnEnISMPI() {
}

void
AnEnISMPI::compute(const Forecasts & forecasts, const Observations & observations,
            vector<size_t> & fcsts_test_index, vector<size_t> & fcsts_search_index) {

    // Get the process ID and the number of worker processes
    int world_rank, num_procs, num_workers;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    // The number of workers is the number of processes minus 1 because process #0 is the master
    num_workers = num_procs - 1;

    if (num_procs == 1) {
        cerr << "Error: This is an MPI program. You need to launch this program with an MPI launcher, e.g. mpirun." << endl;
        MPI_Finalize();
        exit(1);
    }

    if (num_workers == 1) {
        cerr << "Error: To take advantage of MPI, at least 3 processes (1 master + 2 workers) should be created. "
            << "It is, however, better to have more worker processes." << endl;
        MPI_Finalize();
        exit(1);
    }

    if (world_rank == 0) {

        // Broadcast
        // - forecasts
        // - observations
        // - test indices (entire copy)
        // - search indices (entire copy)
        //
        broadcastByStations_();

        // Wait until all processes are done
        
        // Collect results
        // - sds_
        // - sims_metric_
        // - sims_time_index
        // - analogs_value_
        // - analogs_time_index_
        //
        collectByStations_();
        
    } else {
        
        // Broadcast
        // - forecasts
        // - observations
        // - test indices (entire copy)
        // - search indices (entire copy)
        //
        broadcastByStations_();

        // Compute analogs
        AnEnIS::compute(forecasts, observations, fcsts_test_index, fcsts_search_index);
        
        // Collect results
        // - sds_
        // - sims_metric_
        // - sims_time_index
        // - analogs_value_
        // - analogs_time_index_
        //
        collectByStations_();
    }

    return;
}

void
AnEnISMPI::broadcastByStations_() const {
    return;
}

void
AnEnISMPI::collectByStations_() const {
    return;
}
