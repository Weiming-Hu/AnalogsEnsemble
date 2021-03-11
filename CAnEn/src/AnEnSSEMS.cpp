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
    auto match_with = Functions::findClosest(observations.getStations(), forecasts.getStations(), verbose_);
    profiler_.log_time_session("Matching stations (AnEnSSEMS)");

    // Call the class method
    compute(forecasts, observations, fcsts_test_index, fcsts_search_index, match_with);

    return;
}

void 
AnEnSSEMS::compute(const Forecasts & forecasts,
        const Observations & observations,
        std::vector<std::size_t> & fcsts_test_index,
        std::vector<std::size_t> & fcsts_search_index,
        const std::vector<std::size_t> & match_obs_stations_with) {

    match_obs_stations_with_ = match_obs_stations_with;
    if (verbose_ >= Verbose::Progress) cout << "Start AnEnSSE generation ..." << endl;

    preprocess_(forecasts, observations, fcsts_test_index, fcsts_search_index);
    profiler_.log_time_session("Preprocessing (AnEnSSEMS)");
    
    /*
     * Read circular flags from forecast parameters into vectors
     */
    vector<bool> circulars;
    forecasts.getParameters().getCirculars(circulars);

    size_t num_flts = forecasts.getFLTs().size();
    size_t num_test_times_index = fcsts_test_index.size();
    size_t num_search_times_index = fcsts_search_index.size();
    size_t num_obs_stations = match_obs_stations_with_.size();

    /**
     * A vector of arrays consisting of 
     * [similarity value, forecast time index, observation time index, station index]
     * 
     * This is used to store all similarity metrics from all search times for
     * one test time together with indices.
     */
    SimsVec<4> sims_arr;
    sims_arr.resize(num_search_times_index * num_nearest_);

    /*
     * Progress messages output
     */
    if (verbose_ >= Verbose::Detail) print(cout);
    if (verbose_ >= Verbose::Progress) cout << "Computing analogs ..." << endl;

#if defined(_OPENMP)
#pragma omp parallel for default(none) schedule(dynamic) collapse(3) \
shared(num_obs_stations, num_flts, num_test_times_index, num_search_times_index, \
fcsts_test_index, fcsts_search_index, forecasts, observations, circulars) \
firstprivate(sims_arr)
#endif
    for (size_t obs_station_i = 0; obs_station_i < num_obs_stations; ++obs_station_i) {
        for (size_t flt_i = 0; flt_i < num_flts; ++flt_i) {
            for (size_t test_time_i = 0; test_time_i < num_test_times_index; ++test_time_i) {

                size_t fcst_station_i = match_obs_stations_with_[obs_station_i];
                size_t current_test_index = fcsts_test_index[test_time_i];
                size_t current_test_time = forecasts.getTimeStamp(current_test_index);
                size_t current_flt_offset = forecasts.getFltTimeStamp(flt_i);

                /*
                 * Initialize similarity array values
                 */
                fill_n(sims_arr.begin(), sims_arr.size(), _INIT_ARR_VALUE);

                /*
                 * This variable keeps track of where in the similarity array should
                 * we write the metric and indices.
                 */
                size_t search_entry_i = 0;

                /*
                 * Compute similarity for all search times and all search stations
                 */
                for (size_t search_time_i = 0; search_time_i < num_search_times_index; ++search_time_i) {
                    size_t current_search_index = fcsts_search_index[search_time_i];

                    for (size_t search_station_i = 0; search_station_i < num_nearest_; ++search_station_i) {
                        double current_search_station_index = search_stations_index_(fcst_station_i, search_station_i);
                        if (std::isnan(current_search_station_index)) continue;

                        /*
                         * Comparing to the test forecast itself is strictly forbidden
                         */
                        if (current_search_index == current_test_index) continue;

                        /*
                         * Check whether current valid forecast time exceeds test forecast initialization time
                         * for which the corresponding observation is not available
                         */
                        if (prevent_search_future_) {
                            size_t current_search_time = forecasts.getTimeStamp(current_search_index);
                            if (current_search_time + current_flt_offset >= current_test_time) continue;
                        }

                        // Check whether this search time is found in observations
                        double obs_time_index = obs_time_index_table_(search_time_i, flt_i);
                        if (std::isnan(obs_time_index)) continue;

                        // Check whether the associated observation is NA
                        double obs = observations.getValue(obs_var_index_, obs_station_i, obs_time_index);
                        if (std::isnan(obs)) continue;

                        /***********************************************************
                         *                                                         *
                         *                   Computation Core                      *
                         *                                                         *
                         * Compute the metric for this station, flt, and test time *
                         *                                                         *
                         **********************************************************/

                        // Circulars can be directly retrieved from
                        // forecasts. But for better performance, circulars
                        // are pre-computed and passed.
                        //
                        double metric = computeSimMetric_(
                                forecasts, fcst_station_i, current_search_station_index,
                                flt_i, current_test_index, current_search_index, circulars);

                        // Save the similarity metric with corresponding indices
                        sims_arr[search_entry_i][_SIM_VALUE_INDEX] = metric;
                        sims_arr[search_entry_i][_SIM_FCST_TIME_INDEX] = current_search_index;
                        sims_arr[search_entry_i][_SIM_OBS_TIME_INDEX] = obs_time_index;

                        // Note that no matter whether observations are extended or not, this
                        // index to be saved should be the one from the extended search
                        // station because this index corresponds to the similarity metric.
                        //
                        sims_arr[search_entry_i][_SIM_STATION_INDEX] = current_search_station_index;

                        // Increment the index of the location to write similarity
                        ++search_entry_i;
                    }
                }

                /*
                 * Sort based on similarity metrics
                 */
                if (quick_sort_) nth_element(sims_arr.begin(),
                        sims_arr.begin() + num_analogs_, sims_arr.end(), _simsSort_);
                else partial_sort(sims_arr.begin(),
                        sims_arr.begin() + num_analogs_, sims_arr.end(), _simsSort_);

                /*
                 * Output values and indices
                 */
                if (save_analogs_) saveAnalogs_(sims_arr, observations, obs_station_i, test_time_i, flt_i);
                if (save_analogs_time_index_) saveAnalogsTimeIndex_(sims_arr, obs_station_i, test_time_i, flt_i);
                if (save_sims_) saveSims_(sims_arr, obs_station_i, test_time_i, flt_i);
                if (save_sims_time_index_) saveSimsTimeIndex_(sims_arr, obs_station_i, test_time_i, flt_i);
                if (save_sims_station_index_) saveSimsStationIndex_(sims_arr, obs_station_i, test_time_i, flt_i);

            } // End of loop for test times
        } // End of loop for flts
    } // End of loop for stations

    if (verbose_ >= Verbose::Progress) cout << "AnEnSSEMS generation done!" << endl;
    profiler_.log_time_session("Genrating analogs (AnEnSSEMS)");

    return;
}

void
AnEnSSEMS::checkConsistency_(const Forecasts & forecasts,
        const Observations & observations) const {

    if (match_obs_stations_with_.size() == 0) {
        throw runtime_error("Observation stations has not yet been matched to forecasts!");
    }

    if (match_obs_stations_with_.size() != observations.getStations().size()) {
        throw runtime_error("Each observation station should be linked to a forecast station! The numbers of stations should match!");
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

void
AnEnSSEMS::allocateMemory_(const Forecasts & forecasts,
        const vector<size_t> & fcsts_test_index, const vector<size_t> & fcsts_search_index) {

    if (verbose_ >= Verbose::Progress) cout << "Allocating memory ..." << endl;
    
    size_t num_stations = match_obs_stations_with_.size();
    size_t num_flts = forecasts.getFLTs().size();
    size_t num_test_times_index = fcsts_test_index.size();
    size_t num_search_times_index = fcsts_search_index.size();

    // Check for the maximum number of values we can save
    checkNumberOfMembers_(num_search_times_index);

    if (save_analogs_) {
        analogs_value_.resize(num_stations, num_test_times_index, num_flts, num_analogs_);
        analogs_value_.initialize(NAN);
    }

    if (save_analogs_time_index_) {
        analogs_time_index_.resize(num_stations, num_test_times_index, num_flts, num_analogs_);
        analogs_time_index_.initialize(NAN);
    }

    if (save_sims_) {
        sims_metric_.resize(num_stations, num_test_times_index, num_flts, num_sims_);
        sims_metric_.initialize(NAN);
    }

    if (save_sims_time_index_) {
        sims_time_index_.resize(num_stations, num_test_times_index, num_flts, num_sims_);
        sims_time_index_.initialize(NAN);
    }

    if (save_sims_station_index_) {
        sims_station_index_.resize(num_stations, num_test_times_index, num_flts, num_sims_);
        sims_station_index_.initialize(NAN);
    }
 
    return;
}
