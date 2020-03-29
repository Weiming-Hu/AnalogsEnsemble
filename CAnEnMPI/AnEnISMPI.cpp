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

#if defined(_OPENMP)
#include <omp.h>
#endif

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
        cerr << "Error: This is an MPI program. You need to launch this program with an MPI launcher, e.g. mpirun or mpiexec." << endl;
        MPI_Finalize();
        exit(1);
    }

    if (num_workers == 1) {
        cerr << "Error: To take advantage of MPI, at least 3 processes (1 master + 2 workers) should be created. "
            << "It is, however, better to have more worker processes." << endl;
        MPI_Finalize();
        exit(1);
    }

    if (world_rank == 0 && verbose_ >= Verbose::Progress) cout << "Start AnEnIS generation with MPI ..." << endl;

    // Scatter forecasts by stations
    if (world_rank == 0 && verbose_ >= Verbose::Detail) cout << "Master process is scattering forecasts to workers ..." << endl;
    ForecastsPointer proc_forecasts;
    FunctionsMPI::scatterForecasts(forecasts, proc_forecasts, num_procs, world_rank, verbose_);
    if (world_rank == 0 && verbose_ >= Verbose::Detail) cout << "Forecasts have been scattered to workers." << endl;

    // Scatter observations by stations
    if (world_rank == 0 && verbose_ >= Verbose::Detail) cout << "Master process is scattering observations to workers ..." << endl;
    ObservationsPointer proc_observations;
    FunctionsMPI::scatterObservations(observations, proc_observations, num_procs, world_rank, verbose_);
    if (world_rank == 0 && verbose_ >= Verbose::Detail) cout << "Observations have been scattered to workers." << endl;

    // Broadcast test and search
    vector<size_t> proc_test_index, proc_search_index;
    FunctionsMPI::broadcastVector(fcsts_test_index, proc_test_index, world_rank, verbose_);
    FunctionsMPI::broadcastVector(fcsts_search_index, proc_search_index, world_rank, verbose_);

    if (world_rank == 0) {
        // This is a master

        // Preprocess to allocate memory
        preprocess_(forecasts, observations, fcsts_test_index, fcsts_search_index);

        /*
         * Progress messages output
         */
        if (verbose_ >= Verbose::Detail) {
            cout << "********** AnEn Configuration Summary (Master) **********" << endl;
            print(cout);
            cout << "*********** End of AnEn Configuration Summary **********" << endl;
        }

    } else {
        // This is a worker

#if defined(_OPENMP)
        // Explicitly disable dynamic teams
        omp_set_dynamic(0);

        // Use 4 threads for all consecutive parallel regions
        omp_set_num_threads(1);
#endif
        // Compute analogs
        //
        // Now the computation is serial because it is assumed that there are as many processes as possible.
        //
        AnEnIS::compute(proc_forecasts, proc_observations, proc_test_index, proc_search_index);
    }

    // Collect members in AnEnIS
    if (world_rank == 0 && verbose_ >= Verbose::Detail) cout << "Master process is receiving analog results from workers ..." << endl;
    gather_(num_procs, world_rank);
    if (world_rank == 0 && verbose_ >= Verbose::Detail) cout << "Analog results have been collected at master." << endl;

    return;
}

void
AnEnISMPI::gather_(int num_procs, int rank) {

    FunctionsMPI::gatherArray(sds_, 1, num_procs, rank, verbose_);
    if (save_analogs_) FunctionsMPI::gatherArray(analogs_value_, 0, num_procs, rank, verbose_);
    if (save_analogs_time_index_) FunctionsMPI::gatherArray(analogs_time_index_, 0, num_procs, rank, verbose_);
    if (save_sims_) FunctionsMPI::gatherArray(sims_metric_, 0, num_procs, rank, verbose_);
    if (save_sims_time_index_) FunctionsMPI::gatherArray(sims_time_index_, 0, num_procs, rank, verbose_);

    return;
}

