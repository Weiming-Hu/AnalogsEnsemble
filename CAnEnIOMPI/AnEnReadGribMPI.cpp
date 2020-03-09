/* 
 * File:   AnEnReadGribMPI.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on March, 2020, 11:10 AM
 */

#include "AnEnReadGribMPI.h"
#include "ForecastsPointer.h"
#include "Functions.h"

#include <stdexcept>
#include <exception>

using namespace std;

AnEnReadGribMPI::AnEnReadGribMPI() {
    Config config;
    verbose_ = config.verbose;
    worker_verbose_ = config.worker_verbose;
}

AnEnReadGribMPI::AnEnReadGribMPI(const AnEnReadGribMPI& orig) : AnEnReadGrib(orig) {
    Config config;
    worker_verbose_ = config.worker_verbose;
}

AnEnReadGribMPI::AnEnReadGribMPI(Verbose master_verbose) : AnEnReadGrib(master_verbose) {
    Config config;
    worker_verbose_ = config.worker_verbose;
}

AnEnReadGribMPI::AnEnReadGribMPI(Verbose master_verbose, Verbose worker_verbose) : AnEnReadGrib(master_verbose) {
    worker_verbose_ = worker_verbose;
}

AnEnReadGribMPI::~AnEnReadGribMPI() {
}

void
AnEnReadGribMPI::readForecasts(Forecasts & forecasts,
        const std::vector<ParameterGrib> & grib_parameters,
        const std::vector<std::string> & files,
        const std::string & regex_str,
        unsigned long flt_unit_in_seconds,
        bool delimited,
        std::vector<int> stations_index) const {

    /*
     * Read forecast files in parallel with MPI
     *
     * MPI processes are created to read foreacst files simultaneously. So
     * it is preferred to have many forecast files to be distributed.
     *
     * The design is illustrated below:
     *
     * For the master process:
     *
     *   - Allocate enough memory to hold the entire forecasts.
     *   - Wait and collect data values from worker processes.
     *
     * For worker processes:
     *
     *   - Read forecast files that are assigned to the current process
     *   - Send forecast data values back to the master processes.
     */

    // Get the process ID and the number of worker processes
    int world_rank, num_children;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_children);

    if (num_children == 1) {
        cerr << "Error: Only one worker is created. To take advantage of MPI, at least "
            << "2 processes should be created. It is, however, better to have more worker processes." << endl;
        MPI_Finalize();
        exit(1);
    }

    if (world_rank == 0) {

        /*
         * I'm the master process
         */

        /*
         * Read forecasts meta information 
         */
        readForecastsMeta_(forecasts, grib_parameters, files, regex_str,
                flt_unit_in_seconds, delimited, stations_index);

        // All processes wait until the master finishes reading meta information
        MPI_Barrier(MPI_COMM_WORLD);

        if (verbose_ >= Verbose::Progress) cout << "Reading forecast in parallel ..." << endl
                << "There are " << files.size() << " files to read and " << num_children - 1
                << " workers." << endl;
        
    } else {

        /*
         * I'm a worker process
         */
        
        // All processes wait until the master finishes reading meta information
        MPI_Barrier(MPI_COMM_WORLD);

        // Determine what are the files assigned to the current worker process
        int num_workers = num_children - 1;
        int file_start_index, total_files;

        total_files = files.size() / num_workers;
        file_start_index = (world_rank - 1) * total_files;

        if (world_rank == num_children - 1) total_files = files.size() - file_start_index;
        vector<string> files_subset(files.begin() + file_start_index, files.begin() + file_start_index + total_files);

        // Read the assigned files
        ForecastsPointer forecasts_subset;
        AnEnReadGrib anen_read(worker_verbose_);
        anen_read.readForecasts(forecasts_subset, grib_parameters, files_subset,
                regex_str, flt_unit_in_seconds, delimited, stations_index);
        
    }

    MPI_Finalize();
    exit(0);
}

