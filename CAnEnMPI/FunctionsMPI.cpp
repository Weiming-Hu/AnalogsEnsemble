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
FunctionsMPI::scatterObservations(const Observations & send, Observations & recv, int num_procs, int rank) {

    scatterBasicData(send, recv, num_procs, rank);

    // Scatter array
    if (rank == 0) {

        ObservationsPointer obs_sub;

        const Parameters & all_parameters = send.getParameters();
        const Times & all_times = send.getTimes();

        size_t num_total_stations = send.getStations().size();

        for (int worker_rank = 1; worker_rank < num_procs; ++worker_rank) {

            // Determine which stations to send to this worker process
            int worker_station_start = Functions::getStartIndex(num_total_stations, num_procs, worker_rank);
            int worker_stations_count = Functions::getSubTotal(num_total_stations, num_procs, worker_rank);

            Stations stations_subset;
            const Stations & stations_all = send.getStations();
            for (int station_i = worker_station_start; station_i < worker_stations_count; ++station_i) {
                stations_subset.push_back(stations_all.getStation(station_i));
            }

            // Subset
            send.subset(all_parameters, stations_subset, all_times, obs_sub);

            // Send data
            MPI_Send(obs_sub.getValuesPtr(), obs_sub.num_elements(), MPI_DOUBLE, worker_rank, MPI_ANY_TAG, MPI_COMM_WORLD);
        }
    } else {
        int count = boost::numeric_cast<int>(recv.num_elements());
        if (count == 0) throw runtime_error("(FunctionsMPI::scatterObservations) A worker process is receiving array data without allocating memory");

        double *data_ptr = recv.getValuesPtr();
        MPI_Recv(data_ptr, count, MPI_DOUBLE, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    return;
}

void
FunctionsMPI::scatterForecasts(const Forecasts & send, Forecasts & recv, int num_procs, int rank) {

    scatterBasicData(send, recv, num_procs, rank);

    vector<size_t> master_flts, worker_flts;

    if (rank == 0) {
        // I'm the master

        // Prepare data to broadcast (flts)
        send.getFLTs().getTimestamps(master_flts);
    }

    // Broadcast information (flts)
    broadcastVector(master_flts, worker_flts, rank);

    if (rank != 0) {
        // I'm the worker

        // Receive flts
        Times flts;
        for (const auto & e : worker_flts) flts.push_back(e);

        // Resize worker forecasts
        recv.setDimensions(recv.getParameters(), recv.getStations(), recv.getTimes(), flts);
    }

    // Scatter array
    scatterArray(send, recv, num_procs, rank);

    return;
}

void
FunctionsMPI::scatterBasicData(const BasicData & send, BasicData & recv, int num_procs, int rank) {
    unsigned long num_stations;
    vector<bool> master_circulars, worker_circulars;
    vector<size_t> master_times, worker_times;

    if (rank == 0) {
        // I'm the master

        // Prepare data to broadcast (stations, parameters, times)
        num_stations = send.getStations().size();
        send.getParameters().getCirculars(master_circulars);
        send.getTimes().getTimestamps(master_times);
    }

    // Broadcast information (stations, parameters, times)
    MPI_Bcast(&num_stations, 1, MPI_UNSIGNED_LONG, rank, MPI_COMM_WORLD);
    broadcastVector(master_circulars, worker_circulars, rank);
    broadcastVector(master_times, worker_times, rank);

    if (rank != 0) {
        // I'm the worker

        // Create parameters
        Parameters parameters;
        for (const auto & e : worker_circulars) parameters.push_back(Parameter(Config::_NAME, e));

        // Create stations
        Stations stations;
        int sub_total = Functions::getSubTotal(num_stations, num_procs, rank);
        for (int i = 0; i < sub_total; ++i) stations.push_back(Station(i, i));

        // Create times
        Times times;
        for (const auto & e : worker_times) times.push_back(e);

        // Resize worker forecasts
        recv.setMembers(parameters, stations, times);
    }

    return;
}

void
FunctionsMPI::scatterArray(const Array4D & send, Array4D & recv, int num_procs, int rank) {

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
            send.subset(all_parameters, subset_stations, all_times, all_flts, array_subset);

            // Send data
            MPI_Send(array_subset.getValuesPtr(), array_subset.num_elements(), MPI_DOUBLE, worker_rank, MPI_ANY_TAG, MPI_COMM_WORLD);
        }
    } else {

        int count = boost::numeric_cast<int>(recv.num_elements());
        if (count == 0) throw runtime_error("(FunctionsMPI::scatterArray) A worker process is receiving array data without allocating memory");

        double *data_ptr = recv.getValuesPtr();
        MPI_Recv(data_ptr, count, MPI_DOUBLE, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    return;
}

void
FunctionsMPI::broadcastVector(const vector<size_t> & send, vector<size_t> & recv, int rank) {
    
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
    return;
}

void
FunctionsMPI::broadcastVector(const vector<bool> & send, vector<bool> & recv, int rank) {

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
    return;
}

void
FunctionsMPI::gatherArray(Array4D & arr, int num_procs, int rank) {

    if (rank == 0) {

        size_t dim1 = arr.shape()[1];
        size_t dim2 = arr.shape()[2];
        size_t dim3 = arr.shape()[3];
        int num_total_stations = arr.shape()[0];

        if (dim1 == 0 || dim1 == 0 || dim3 == 0 || num_total_stations == 0) throw runtime_error("(FunctionsMPI::gatherArray) Master process is trying to receive array without allocating memory");

        // I'm the master process. I receive data from workers.
        for (int worker_rank = 1; worker_rank < num_procs; ++worker_rank) {

            // Determine which stations to send to this worker process
            int worker_station_start = Functions::getStartIndex(num_total_stations, num_procs, worker_rank);
            int worker_stations_count = Functions::getSubTotal(num_total_stations, num_procs, worker_rank);
            int subset_values_count = dim1 * dim2 * dim3 * worker_stations_count;

            double *data_ptr = new double[subset_values_count];
            int ptr_i = 0;

            for (size_t dim3_i = 0; dim3_i < dim3; ++dim3)
                for (size_t dim2_i = 0; dim2_i < dim2; ++dim2)
                    for (size_t dim1_i = 0; dim1_i < dim1; ++dim1)
                        for (int station_i = worker_station_start; station_i < worker_stations_count; ++station_i, ++ptr_i)
                            arr.setValue(data_ptr[ptr_i], station_i, dim1_i, dim2_i, dim3_i);

            delete [] data_ptr;
        }

    } else {

        // I'm the worker process. I send data to the master.
        double * data_ptr = arr.getValuesPtr();
        int count = boost::numeric_cast<int>(arr.num_elements());
        MPI_Send(data_ptr, count, MPI_DOUBLE, 0, MPI_ANY_TAG, MPI_COMM_WORLD);
    }

    return;
}
