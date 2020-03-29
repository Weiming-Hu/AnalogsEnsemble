/* 
 * File:   FunctionsMPI.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on March, 2020, 11:10 AM
 */

#include "ObservationsPointer.h"
#include "Array4DPointer.h"
#include "FunctionsMPI.h"
#include "Functions.h"

#include <cmath>
#include <numeric>
#include <string>
#include <mpi.h>
#include <boost/numeric/conversion/cast.hpp>

using namespace std;

void
FunctionsMPI::scatterObservations(const Observations & send, Observations & recv, int num_procs, int rank, Verbose verbose) {

    scatterBasicData(send, recv, num_procs, rank, verbose);


    // Scatter array
    if (rank == 0) {

        ObservationsPointer obs_sub;

        const Parameters & all_parameters = send.getParameters();
        const Times & all_times = send.getTimes();

        size_t num_total_stations = send.getStations().size();

        for (int worker_rank = 1; worker_rank < num_procs; ++worker_rank) {

            // Determine which stations to send to this worker process
            int worker_station_start = Functions::getStartIndex(num_total_stations, num_procs, worker_rank);
            int worker_station_end = Functions::getEndIndex(num_total_stations, num_procs, worker_rank);
            int worker_stations_count = worker_station_end - worker_station_start;

            Stations stations_subset;
            const Stations & stations_all = send.getStations();
            for (int station_i = worker_station_start; station_i < worker_station_end; ++station_i) {
                stations_subset.push_back(stations_all.getStation(station_i));
            }

            // Subset
            if (verbose >= Verbose::Debug) cout << "Master subseting observations along the station dimension for worker #" << worker_rank << "..." << endl;
            send.subset(all_parameters, stations_subset, all_times, obs_sub);

            // Send data
            if (verbose >= Verbose::Detail) cout << "Master sending a subset of observations [stations start: "
                << worker_station_start << " count: " << worker_stations_count <<"] to worker #" << worker_rank << "..." << endl;

            double * data_ptr = obs_sub.getValuesPtr();
            MPI_Send(data_ptr, obs_sub.num_elements(), MPI_DOUBLE, worker_rank, 0, MPI_COMM_WORLD);
        }
    } else {

        recv.setDimensions(recv.getParameters(), recv.getStations(), recv.getTimes());

        size_t num_parameters = recv.getParameters().size();
        size_t num_stations = recv.getStations().size();
        size_t num_times = recv.getTimes().size();

        if (verbose >= Verbose::Debug) cout << "Worker #" << rank << " is allocating memory for observations ["
                << num_parameters << " parameters, " << num_stations << " stations, " << num_times << " times]" << "..." << endl;

        int count = boost::numeric_cast<int>(recv.num_elements());
        if (count == 0) throw runtime_error("(FunctionsMPI::scatterObservations) A worker process is receiving array data without allocating memory");

        double *data_ptr = recv.getValuesPtr();
        MPI_Recv(data_ptr, count, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    return;
}

void
FunctionsMPI::scatterForecasts(const Forecasts & send, Forecasts & recv, int num_procs, int rank, Verbose verbose) {

    scatterBasicData(send, recv, num_procs, rank, verbose);

    vector<size_t> master_flts, worker_flts;

    if (rank == 0) {
        // I'm the master

        // Prepare data to broadcast (flts)
        send.getFLTs().getTimestamps(master_flts);
    }

    // Broadcast information (flts)
    broadcastVector(master_flts, worker_flts, rank, verbose);

    if (rank != 0) {
        // I'm the worker

        // Receive flts
        Times flts;
        for (const auto & e : worker_flts) flts.push_back(e);

        // Resize worker forecasts
        recv.setDimensions(recv.getParameters(), recv.getStations(), recv.getTimes(), flts);

        size_t num_parameters = recv.getParameters().size();
        size_t num_stations = recv.getStations().size();
        size_t num_times = recv.getTimes().size();
        size_t num_flts = recv.getFLTs().size();

        if (verbose >= Verbose::Debug) cout << "Worker #" << rank << " is allocating memory for forecasts ["
                << num_parameters << " parameters, " << num_stations << " stations, "
                << num_times << " times, " << num_flts << " lead times]" << "..." << endl;
    }

    // Scatter array
    scatterArray(send, recv, num_procs, rank, verbose);

    return;
}

void
FunctionsMPI::scatterBasicData(const BasicData & send, BasicData & recv, int num_procs, int rank, Verbose verbose) {
    unsigned long num_stations;
    vector<bool> master_circulars, worker_circulars;
    vector<size_t> master_times, worker_times;

    if (rank == 0) {
        // I'm the master

        // Prepare data to broadcast (stations, parameters, times)
        num_stations = send.getStations().size();
        send.getParameters().getCirculars(master_circulars);
        send.getTimes().getTimestamps(master_times);

        if (verbose >= Verbose::Debug) cout << "Master broadcasting BasicData (parameters, stations, and times) ..." << endl;
    }

    // Broadcast information (stations, parameters, times)
    MPI_Bcast(&num_stations, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
    broadcastVector(master_circulars, worker_circulars, rank, verbose);
    broadcastVector(master_times, worker_times, rank, verbose);

    if (rank != 0) {
        // I'm the worker

        // Create parameters
        Parameters parameters;
        for (size_t i = 0; i < worker_circulars.size(); ++i) {
            Parameter parameter(string("Placeholder_") + to_string(i), worker_circulars[i]);
            parameters.push_back(parameter);
        }

        // Create stations
        Stations stations;
        int sub_total = Functions::getSubTotal(num_stations, num_procs, rank);
        for (int i = 0; i < sub_total; ++i) stations.push_back(Station(i, i));

        // Create times
        Times times;
        for (const auto & e : worker_times) times.push_back(e);

        // Resize worker forecasts
        recv.setMembers(parameters, stations, times);
        if (verbose >= Verbose::Debug) cout << "Worker #" << rank << " set BasicData members with "
                << recv.getParameters().size() << " parameters, " << recv.getStations().size() << " stations, and "
                << recv.getTimes().size() << " times" << endl;
    }

    return;
}

void
FunctionsMPI::scatterArray(const Array4D & send, Array4D & recv, int num_procs, int rank, Verbose verbose) {

    if (rank == 0) {
        Array4DPointer array_subset;

        size_t num_total_stations = send.shape()[1];

        vector<size_t> all_parameters(send.shape()[0]);
        vector<size_t> subset_stations;
        vector<size_t> all_times(send.shape()[2]);
        vector<size_t> all_flts(send.shape()[3]);

        // The master rank will broadcast all dimensions except for the station dimension
        iota(all_parameters.begin(), all_parameters.end(), 0);
        iota(all_times.begin(), all_times.end(), 0);
        iota(all_flts.begin(), all_flts.end(), 0);
        
        for (int worker_rank = 1; worker_rank < num_procs; ++worker_rank) {

            // Determine which stations to send to this worker process
            int worker_station_start = Functions::getStartIndex(num_total_stations, num_procs, worker_rank);
            int worker_stations_count = Functions::getSubTotal(num_total_stations, num_procs, worker_rank);

            subset_stations.resize(worker_stations_count);
            iota(subset_stations.begin(), subset_stations.end(), worker_station_start);

            // Subset array for this woker process
            if (verbose >= Verbose::Debug) cout << "Master subseting array along the station dimension for worker #" << worker_rank << "..." << endl;
            send.subset(all_parameters, subset_stations, all_times, all_flts, array_subset);

            // Send data
            if (verbose >= Verbose::Detail) cout << "Master sending a subset of array [station start: "
                << worker_station_start << " count: " << worker_stations_count << "] to worker #" << worker_rank << "..." << endl;

            double *data_ptr = array_subset.getValuesPtr();
            MPI_Send(data_ptr, array_subset.num_elements(), MPI_DOUBLE, worker_rank, 0, MPI_COMM_WORLD);
        }
    } else {

        int count = boost::numeric_cast<int>(recv.num_elements());
        if (count == 0) throw runtime_error("(FunctionsMPI::scatterArray) A worker process is receiving array data without allocating memory");

        double *data_ptr = recv.getValuesPtr();
        if (verbose >= Verbose::Debug) cout << "Worker #" << rank << " waiting for array data from master ..." << endl;
        MPI_Recv(data_ptr, count, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        if (verbose >= Verbose::Debug) cout << "Worker #" << rank << " received array data from master: " << Functions::format(data_ptr, count) << endl;
    }

    return;
}

void
FunctionsMPI::broadcastVector(const vector<size_t> & send, vector<size_t> & recv, int rank, Verbose verbose) {
    if (rank == 0 && verbose >= Verbose::Debug) cout << "Master broadcasting a size_t vector ..." << endl;
    
    // Broadcast the length of a vector
    unsigned long size;
    if (rank == 0) size = send.size();
    MPI_Bcast(&size, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);

    // Broadcast the values in the vector
    unsigned long *values = new unsigned long[size];
    if (rank == 0) for (unsigned long i = 0; i < size; ++i) values[i] = send[i];

    MPI_Bcast(values, size, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);

    // Copy values back to the vector in worker processes
    if (rank != 0) {
        recv.resize(size);
        for (unsigned long i = 0; i < size; ++i) recv[i] = values[i];
    }

    delete [] values;
    if (rank != 0 && verbose >= Verbose::Debug) cout << "Worker #" << rank << " received a size_t vector of length " << recv.size() << endl;
    return;
}

void
FunctionsMPI::broadcastVector(const vector<bool> & send, vector<bool> & recv, int rank, Verbose verbose) {
    if (rank == 0 && verbose >= Verbose::Debug) cout << "Master broadcasting a bool vector ..." << endl;

    // Broadcast the length of a vector
    unsigned long size;
    if (rank == 0) size = send.size();
    MPI_Bcast(&size, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);

    // Broadcast the values in the vector
    unsigned short *values = new unsigned short[size];
    if (rank == 0) for (unsigned long i = 0; i < size; ++i) values[i] = send[i];

    MPI_Bcast(values, size, MPI_UNSIGNED_SHORT, 0, MPI_COMM_WORLD);

    // Copy values back to the vector in worker processes
    if (rank != 0) {
        recv.resize(size);
        for (unsigned long i = 0; i < size; ++i) recv[i] = values[i];
    }

    delete [] values;
    if (rank != 0 && verbose >= Verbose::Debug) cout << "Worker #" << rank << " received a bool vector of length " << recv.size() << endl;
    return;
}

void
FunctionsMPI::gatherArray(Array4D & arr, int station_dim_index, int num_procs, int rank, Verbose verbose) {

    if (rank == 0) {

        vector<size_t> subset_dim(arr.shape(), arr.shape() + 4);
        vector<size_t> subset_dim_end(subset_dim);
        vector<size_t> subset_dim_start(4, 0);

        int num_total_stations = arr.shape()[station_dim_index];

        if (arr.num_elements() == 0) throw runtime_error("(FunctionsMPI::gatherArray) Master process is trying to receive array without allocating memory");

        // I'm the master process. I receive data from workers.
        for (int worker_rank = 1; worker_rank < num_procs; ++worker_rank) {
            if (verbose >= Verbose::Detail) cout << "Master receiving a subset of array from worker #" << worker_rank << " ..." << endl;

            // Determine which stations to send to this worker process
            int worker_station_start = Functions::getStartIndex(num_total_stations, num_procs, worker_rank);
            int worker_stations_count = Functions::getSubTotal(num_total_stations, num_procs, worker_rank);

            // Determine the start of station index
            subset_dim[station_dim_index] = worker_stations_count;
            subset_dim_start[station_dim_index] = worker_station_start;
            subset_dim_end[station_dim_index] = worker_station_start + worker_stations_count;

            // Calculate how many values to be expected
            int subset_values_count = subset_dim[0] * subset_dim[1] * subset_dim[2] * subset_dim[3];

            // Receive data from worker process
            double *data_ptr = new double[subset_values_count];
            MPI_Recv(data_ptr, subset_values_count, MPI_DOUBLE, worker_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if (verbose >= Verbose::Debug) cout << "Master received (" << Functions::format(subset_dim)
                << ") array values from worker #" << worker_rank << " starting from station " << worker_station_start << ": "
                    << Functions::format(data_ptr, subset_values_count) << endl;
            int ptr_i = 0;

            for (size_t dim3_i = subset_dim_start[3]; dim3_i < subset_dim_end[3]; ++dim3_i)
                for (size_t dim2_i = subset_dim_start[2]; dim2_i < subset_dim_end[2]; ++dim2_i)
                    for (size_t dim1_i = subset_dim_start[1]; dim1_i < subset_dim_end[1]; ++dim1_i)
                        for (size_t dim0_i = subset_dim_start[0]; dim0_i < subset_dim_end[0]; ++dim0_i, ++ptr_i)
                            arr.setValue(data_ptr[ptr_i], dim0_i, dim1_i, dim2_i, dim3_i);

            delete [] data_ptr;
        }

    } else {

        // I'm the worker process. I send data to the master.
        double * data_ptr = arr.getValuesPtr();
        int count = boost::numeric_cast<int>(arr.num_elements());
        if (verbose >= Verbose::Debug) cout << "Worker #" << rank << " sending " << count << " array values to master ..." << endl;
        MPI_Send(data_ptr, count, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }

    return;
}
