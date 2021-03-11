/* 
 * File:   AnEnSSEMSMS.cpp
 * Author: Weiming Hu <cervone@psu.edu>
 * 
 * Created on Mar. 10, 2021, 17:13
 */

#include "AnEnSSEMS.h"

#include <stdexcept>
#include <algorithm>

#if defined(_OPENMP)
#include <omp.h>
#endif

using namespace std;

AnEnSSEMS::AnEnSSEMS() : AnEnSSE() {
}

AnEnSSEMS::AnEnSSEMS(const AnEnSSEMS& orig) : AnEnSSE(orig) {
}

AnEnSSEMS::AnEnSSEMS(const Config & config) : AnEnSSE(config) {
}

AnEnSSEMS::~AnEnSSEMS() {
}

void
AnEnSSEMS::compute(const Forecasts & forecasts,
        const Observations & observations,
        vector<size_t> & fcsts_test_index,
        vector<size_t> & fcsts_search_index) {

    // Match forecast and observation stations based on distances
    matchStations_(forecasts.getStations(), observations.getStations());

    // Call the class method
    compute(forecasts, observations, fcsts_test_index, fcsts_search_index, match_obs_stations_with_);

    return;
}

void 
AnEnSSEMS::compute(const Forecasts & forecasts,
        const Observations & observations,
        std::vector<std::size_t> & fcsts_test_index,
        std::vector<std::size_t> & fcsts_search_index,
        const std::vector<std::size_t> & match_obs_stations_with) {
    return;
}

void
AnEnSSEMS::matchStations_(const Stations & obs_stations,
        const Stations & fcst_stations) {

    // Initialize the vector
    size_t num_obs_stations = obs_stations.size();
    match_obs_stations_with_.resize(num_obs_stations, 0);

    // For each of the observation station, find the cloest forecast station
    for (size_t obs_station_i = 0; obs_station_i < num_obs_stations; ++obs_station_i) {
        match_obs_stations_with_[obs_station_i] = Functions::findClosest(obs_stations.getStation(obs_station_i), fcst_stations);
    }

    return;
}

void
AnEnSSEMS::checkConsistency_(const Forecasts & forecasts,
        const Observations & observations) const {

    if (match_obs_stations_with_.size() == 0) {
        throw runtime_error("Observation stations has not yet been matched to forecasts!");
    }

    if (obs_var_index_ >= observations.getParameters().size()) {
        ostringstream msg;
        msg << "Observation variable index is " << obs_var_index_
                << " (counting from 0) but there are only "
                << observations.getParameters().size() << " parameters";
        throw runtime_error(msg.str());
    }

    return;
}

