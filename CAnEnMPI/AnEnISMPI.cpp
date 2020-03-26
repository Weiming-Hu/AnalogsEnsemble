/* 
 * File:   AnEnISMPI.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on March, 2020, 11:10 AM
 */

#include "AnEnISMPI.h"
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

    // Broadcast forecasts
    ForecastsPointer proc_forecasts;
    broadcastForecasts_(forecasts, proc_forecasts, world_rank);

    if (world_rank == 0) {

        // Broadcast
        // - forecasts
        // - observations
        // - test indices (entire copy)
        // - search indices (entire copy)
        //


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
        // - forecasts (parameters, stations, times, flts, data)
        // - observations (parameters, stations, times, data)
        // - test indices (entire copy)
        // - search indices (entire copy)
        //
        ForecastsPointer forecasts_proc;
        double *recv_forecast_values = forecasts_proc.getValuesPtr();
        broadcastByStations_(nullptr, recv_forecast_values, world_rank);

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
AnEnISMPI::broadcastForecasts_(const Forecasts & send, Forecasts & recv, int rank) const {

    unsigned long num_stations;

    if (rank == 0) {
        // I'm the master
        
        // Broadcast parameter circular field
        vector<double> master_send, placeholder;
        send.getParameters().getCirculars(master_send);
        broadcastVector_(master_send, placeholder);

        // Broadcast the total number of stations
        num_stations = send.getStations().size();
        MPI_Bcast(&num_stations, 1, MPI_UNSIGNED_LONG, rank, MPI_COMM_WORLD);

        // Broadcast times
        send.getTimes().getTimestamps(master_send);
        broadcastVector_(master_send, placeholder);

        // Broadcast flts
        send.getFLTs().getTimestamps(master_send);
        broadcastVector_(master_send, placeholder);

        // Broadcast forecast values
        broadcastByStations_(send.getValuesPtr(), nullptr, rank);

    } else {

        // I'm the worker

        // Receive parameter circular field
        vector<double> proc_recv, placeholder;
        broadcastVector_(placeholder, proc_recv);
        Parameters parameters;
        for (const auto & e : proc_recv) parameters.push_back(Parameter(Config::_NAME, e));

        // Receive the total number of stations
        MPI_Bcast(&num_stations, 1, MPI_UNSIGNED_LONG, rank, MPI_COMM_WORLD);
        Stations stations;
        for (unsigned long i = 0; i < num_stations; ++i) stations.push_back(Station(i, i));

        // Receive times
        broadcastVector_(placeholder, proc_recv);
        Times times;
        for (const auto & e : proc_recv) times.push_back(e);
        
        // Receive flts
        broadcastVector_(placeholder, proc_recv);
        Times flts;
        for (const auto & e : proc_recv) flts.push_back(e);

        // Receive forecasts
        recv.setDimensions(parameters, stations, times, flts);
        double *proc_forecast_values = recv.getValuesPtr();
        broadcastByStations_(nullptr, proc_forecast_values, rank);
    }

    return;
}

int
AnEnISMPI::getStartIndex_(int total, int rank) const {
    return 0;
}

int
AnEnISMPI::getEndIndex_(int total, int rank) const {
    return 0;
}

int
AnEnISMPI::getSubTotal(int total, int rank) const {
    return 0;
}

void
AnEnISMPI::broadcastVector_(const vector<double> & send, vector<double> & recv) const {
    return;
}

void
AnEnISMPI::broadcastByStations_(const double *send, double *recv, int rank) const {
    return;
}

void
AnEnISMPI::collectByStations_(const double *send, double *recv, int rank) const {
    return;
}
