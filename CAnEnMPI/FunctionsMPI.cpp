/* 
 * File:   FunctionsMPI.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on March, 2020, 11:10 AM
 */

#include "FunctionsMPI.h"

using namespace std;

void
FunctionsMPI::collectAnEn(int rank) {
    return;
}

void
FunctionsMPI::scatterObservations(const Observations & send, Observations & recv, int rank) {
    return;
}

void
FunctionsMPI::scatterBasicData(const BasicData & send, BasicData & recv, int rank) {
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
        int sub_total = getSubTotal(num_stations, rank);
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
FunctionsMPI::scatterForecasts(const Forecasts & send, Forecasts & recv, int rank) {

    scatterBasicData(send, recv, rank);

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
    scatterArray(send, recv, rank);

    return;
}

int
FunctionsMPI::getStartIndex(int total, int rank) {
    return 0;
}

int
FunctionsMPI::getEndIndex(int total, int rank) {
    return 0;
}

int
FunctionsMPI::getSubTotal(int total, int rank) {
    return 0;
}

void
FunctionsMPI::broadcastVector(const vector<size_t> & send, vector<size_t> & recv, int rank) {
    return;
}

void
FunctionsMPI::broadcastVector(const vector<bool> & send, vector<bool> & recv, int rank) {
    return;
}
