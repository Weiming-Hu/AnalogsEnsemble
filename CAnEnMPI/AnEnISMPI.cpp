/* 
 * File:   AnEnISMPI.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on March, 2020, 11:10 AM
 */

#include "AnEnISMPI.h"
#include "Functions.h"
#include "FunctionsMPI.h"
#include "ForecastsPointer.h"
#include "ObservationsPointer.h"

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

    // Scatter forecasts by stations
    ForecastsPointer proc_forecasts;
    FunctionsMPI::scatterForecasts(forecasts, proc_forecasts, num_procs, world_rank);

    // Scatter observations by stations
    ObservationsPointer proc_observations;
    FunctionsMPI::scatterObservations(observations, proc_observations, num_procs, world_rank);

    // Broadcast test and search
    vector<size_t> proc_test_index, proc_search_index;
    FunctionsMPI::broadcastVector(fcsts_test_index, proc_test_index, world_rank);
    FunctionsMPI::broadcastVector(fcsts_search_index, proc_search_index, world_rank);

    if (world_rank == 0) {
        // This is a master

        // Preprocess to allocate memory
        preprocess_(forecasts, observations, fcsts_test_index, fcsts_search_index);

    } else {
        // This is a worker

        // Compute analogs
        AnEnIS::compute(proc_forecasts, proc_observations, fcsts_test_index, fcsts_search_index);
    }

    // Collect members in AnEnIS
    FunctionsMPI::collectAnEn(num_procs, world_rank);

    return;
}
