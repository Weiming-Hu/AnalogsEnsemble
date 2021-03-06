/* 
 * File:   AnEnReadGribMPI.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on March, 2020, 11:10 AM
 */

#include "AnEnReadGribMPI.h"
#include "ForecastsPointer.h"
#include "Functions.h"

#include <cmath>
#include <stdexcept>
#include <exception>
#include <sstream>

using namespace std;

static const int MPI_TAG_START = 0;
static const int MPI_TAG_TOTAL = 1;
static const int MPI_TAG_ELEMENTS = 2;
static const int MPI_TAG_FCSTS = 3;

AnEnReadGribMPI::AnEnReadGribMPI() {
    Config config;
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

AnEnReadGribMPI::AnEnReadGribMPI(Verbose master_verbose, Verbose worker_verbose) :
AnEnReadGrib(master_verbose), worker_verbose_(worker_verbose) {
}

AnEnReadGribMPI::~AnEnReadGribMPI() {
}

void
AnEnReadGribMPI::readForecasts(Forecasts & forecasts,
        const vector<ParameterGrib> & grib_parameters,
        const vector<string> & files,
        const string & regex_str,
        unsigned long flt_unit_in_seconds,
        bool delimited,
        vector<int> stations_index) const {

    /*
     * Read forecast files in parallel with MPI
     *
     * MPI processes are created to read foreacst files simultaneously.
     * So it is preferred to have many forecast files to be distributed.
     *
     * The design is illustrated below:
     *
     * For the master process (process ID = 0):
     *
     *   - Allocate enough memory to hold the entire forecasts.
     *   - Wait and collect data values from worker processes.
     *
     * For worker processes (process ID > 0):
     *
     *   - Read forecast files that are assigned to the current process
     *   - Send forecast data values back to the master processes.
     */

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

    if (world_rank == 0) {

        /*
         * I'm the master process
         */

        /*
         * Read forecasts meta information 
         */
        readForecastsMeta_(forecasts, grib_parameters, files, regex_str,
                flt_unit_in_seconds, delimited, stations_index);

        if (verbose_ >= Verbose::Progress) cout << "Reading forecast in parallel ..." << endl
                << "There are " << files.size() << " files to read and " << num_workers
                << " workers." << endl;


        /*
         * Initialize variables to store values from worker processes
         */

        // This is the number of data values to be received from each process
        unsigned long num_process_elements;

        // This is the pointer to the data values to be received from each process
        double* p_value = nullptr;

        // These variables locate which files are read by a particular process
        int file_start_index, total_files;

        // These variables locate the indices of a file time in the entire forecasts
        // and the subset forecasts from each process
        //
        size_t proc_time_i, proc_flt_i, time_i, flt_i;

        Time proc_time, proc_flt;
        Times proc_times, proc_flts;

        const Times & forecast_times = forecasts.getTimes();
        const Times & forecast_flts = forecasts.getFLTs();

        // These variables are created for parsing file names
        boost::gregorian::date start_day(boost::gregorian::from_string(Time::_origin));
        boost::xpressive::sregex rex = boost::xpressive::sregex::compile(regex_str);

        // This is the pointer to the entire forecast data values
        double* p_forecasts = forecasts.getValuesPtr();

        // This is the number of total forecast times
        size_t num_forecast_times = forecast_times.size();

        // The chunk length for data values read from each file.
        size_t offset = forecasts.getStations().size() * forecasts.getParameters().size();

        // The offsets of pointers in the master forecasts and
        // the process forecasts until the data values to be read
        //
        size_t offset_proc, offset_master;


        /*
         * Collect data from each worker process
         */
        for (int process_i = 1; process_i < num_procs; ++process_i) {

            // Determine what are the files that this particular process is reading from
            MPI_Recv(&file_start_index, 1, MPI_INT, process_i, MPI_TAG_START, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&total_files, 1, MPI_INT, process_i, MPI_TAG_TOTAL, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            vector<string> files_subset(files.begin() + file_start_index, files.begin() + file_start_index + total_files);

            // Translate these filenames to times and flts that the process is responsible of reading
            FunctionsIO::parseFilenames(proc_times, proc_flts, files_subset, regex_str, flt_unit_in_seconds, delimited);

            // This is the number of times from each process
            // This process has read this many of times. This is used to calculate pointer offset
            size_t num_proc_times = proc_times.size();

            // Get the number of data values from the particular process
            MPI_Recv(&num_process_elements, 1, MPI_UNSIGNED_LONG, process_i, MPI_TAG_ELEMENTS, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            // Allocate memory
            p_value = new double[num_process_elements];

            // Get the data values from the particular process
            MPI_Recv(p_value, num_process_elements, MPI_DOUBLE, process_i, MPI_TAG_FCSTS, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            if (verbose_ >= Verbose::Debug) cout << "From process #" << process_i 
                << " total values: " << Functions::format(p_value, num_process_elements, ",", num_process_elements) << endl;

            // Copy values from the pointer to forecasts
            for (const auto & file : files_subset) {

                /*
                 * Determine the indices for this particular file
                 */
                FunctionsIO::parseFilename(proc_time, proc_flt, file, start_day, rex, flt_unit_in_seconds, delimited);

                try {
                    proc_time_i = proc_times.getIndex(proc_time);
                    proc_flt_i = proc_flts.getIndex(proc_flt);
                    time_i = forecast_times.getIndex(proc_time);
                    flt_i = forecast_flts.getIndex(proc_flt);
                } catch (exception & e) {
                    stringstream msg;
                    msg << "Failed during queryingindices for time (" << proc_time.toString() <<
                        ") and flt (" << proc_flt << ")" << endl << "The original error : "
                        << e.what() << endl << endl;

                    if (verbose_ >= Verbose::Detail) msg << "******** Detailed information ********"
                            << endl << "Process times: " << proc_times
                            << "Process flts: " << proc_flts << "Forecast times: " << forecast_times
                            << "Forecast lead times: " << forecast_flts;

                    throw runtime_error(msg.str());
                }

                /*
                 * Calculate the pointer offsets in column-major
                 */
                offset_proc = (proc_flt_i * num_proc_times + proc_time_i) * offset;
                offset_master = (flt_i * num_forecast_times + time_i) * offset;

                /*
                 * Copy values
                 */
                if (verbose_ >= Verbose::Debug) cout << "From process #" << process_i << " file " << file
                    << " values: " << Functions::format(p_value + offset_proc, offset, ",") << endl;

                memcpy(p_forecasts + offset_master,  // Destination address
                        p_value + offset_proc,       // Source address
                        offset * sizeof(double));    // Number of bytes
            }

            // Housekeeping
            delete [] p_value;

            if (verbose_ >= Verbose::Detail) cout << "Collected data from the worker process #" << process_i << "/" << num_workers << " ..." << endl;
        }

        if (verbose_ >= Verbose::Detail) cout << "Forecasts have been received from all worker processes." << endl;

    } else {

        /*
         * I'm a worker process
         */
        char err_buffer[MPI_MAX_ERROR_STRING];
        int err_len;
        
        // Determine what are the files assigned to the current worker process
        int total_files = Functions::getSubTotal(files.size(), num_procs, world_rank);
        int file_start_index = Functions::getStartIndex(files.size(), num_procs, world_rank);

        if (worker_verbose_ >= Verbose::Detail) cout << "Worker process #" << world_rank << " initiated to read " << total_files << " files" << endl;
        vector<string> files_subset(files.begin() + file_start_index, files.begin() + file_start_index + total_files);

        // Read the assigned files
        ForecastsPointer forecasts_subset;
        AnEnReadGrib anen_read(worker_verbose_);
        anen_read.readForecasts(forecasts_subset, grib_parameters, files_subset,
                regex_str, flt_unit_in_seconds, delimited, stations_index);

        if (worker_verbose_ >= Verbose::Debug) cout << "From process #" << world_rank << " forecasts:" << forecasts_subset << endl;

        // Send the start and count for file names
        int err = MPI_Send(&file_start_index, 1, MPI_INT, 0, MPI_TAG_START, MPI_COMM_WORLD);
        stringstream msg;
        if (err != MPI_SUCCESS) {
            MPI_Error_string(err, err_buffer, &err_len);
            msg << "Failed to send file_start_index (" << file_start_index << ") to master from worker #" << world_rank << ". MPI error: " << string(err_buffer);
            throw runtime_error(msg.str());
        }

        err = MPI_Send(&total_files, 1, MPI_INT, 0, MPI_TAG_TOTAL, MPI_COMM_WORLD);
        if (err != MPI_SUCCESS) {
            MPI_Error_string(err, err_buffer, &err_len);
            msg << "Failed to send total_files (" << total_files << ") to master from worker #" << world_rank << ". MPI error: " << string(err_buffer);
            throw runtime_error(msg.str());
        }

        // Send the number of elements
        unsigned long num_elements = forecasts_subset.num_elements();
        err = MPI_Send(&num_elements, 1, MPI_UNSIGNED_LONG, 0, MPI_TAG_ELEMENTS, MPI_COMM_WORLD);
        if (err != MPI_SUCCESS) {
            MPI_Error_string(err, err_buffer, &err_len);
            msg << "Failed to send num_elements (" << num_elements << ") to master from worker #" << world_rank << ". MPI error: " << string(err_buffer);
            throw runtime_error(msg.str());
        }

        // Send the forecast values
        double *data_ptr = forecasts_subset.getValuesPtr();
        err = MPI_Send(data_ptr, forecasts_subset.num_elements(), MPI_DOUBLE, 0, MPI_TAG_FCSTS, MPI_COMM_WORLD);
        if (err != MPI_SUCCESS) {
            MPI_Error_string(err, err_buffer, &err_len);
            msg << "Failed to send forecasts to master from worker #" << world_rank << ". MPI error: " << string(err_buffer);
            throw runtime_error(msg.str());
        }

        if (worker_verbose_ >= Verbose::Detail) cout << "Worker process #" << world_rank << " finished all reading." << endl;
    }

    return;
}

