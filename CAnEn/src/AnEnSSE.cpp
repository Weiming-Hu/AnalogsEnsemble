/* 
 * File:   AnEnSSE.cpp
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <cervone@psu.edu>
 * 
 * Created on January 29, 2020, 5:08 PM
 */

#include "AnEnSSE.h"

#include <stdexcept>
#include <algorithm>

#if defined(_OPENMP)
#include <omp.h>
#endif

using namespace std;

const size_t AnEnSSE::_SIM_STATION_INDEX = 3;
const array<double, 4> AnEnSSE::_INIT_ARR_VALUE = {NAN, NAN, NAN, NAN};

AnEnSSE::AnEnSSE() : AnEnIS() {
    Config config;
    setMembers_(config);
}

AnEnSSE::AnEnSSE(const AnEnSSE& orig) : AnEnIS(orig) {
    *this = orig;
}

AnEnSSE::AnEnSSE(const Config & config) : AnEnIS(config) {
    setMembers_(config);
    if (num_nearest_ == 0) throw runtime_error("Please set the number of nearest neighbors");
}

AnEnSSE::~AnEnSSE() {
}

void
AnEnSSE::compute(const Forecasts & forecasts,
        const Observations & observations,
        vector<size_t> & fcsts_test_index,
        vector<size_t> & fcsts_search_index) {

    if (verbose_ >= Verbose::Progress) cout << "Start AnEnSSE generation ..." << endl;

    preprocess_(forecasts, observations, fcsts_test_index, fcsts_search_index);
    profiler_.log_time_session("Preprocessing (AnEnSSE)");

    /*
     * Read circular flags from forecast parameters into vectors
     */
    vector<bool> circulars;
    forecasts.getParameters().getCirculars(circulars);

    size_t num_stations = forecasts.getStations().size();
    size_t num_flts = forecasts.getFLTs().size();
    size_t num_test_times_index = fcsts_test_index.size();
    size_t num_search_times_index = fcsts_search_index.size();

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
shared(num_stations, num_flts, num_test_times_index, num_search_times_index, \
fcsts_test_index, fcsts_search_index, forecasts, observations, circulars) \
firstprivate(sims_arr)
#endif
    for (size_t station_i = 0; station_i < num_stations; ++station_i) {
        for (size_t flt_i = 0; flt_i < num_flts; ++flt_i) {
            for (size_t test_time_i = 0; test_time_i < num_test_times_index; ++test_time_i) {

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
                        double current_search_station_index = search_stations_index_(station_i, search_station_i);
                        if (std::isnan(current_search_station_index)) continue;

                        size_t current_obs_station_index;
                        if (extend_obs_) current_obs_station_index = current_search_station_index;
                        else current_obs_station_index = station_i;

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
                        double obs = observations.getValue(obs_var_index_,
                                current_obs_station_index, obs_time_index);
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
                                forecasts, station_i, current_search_station_index,
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
                if (save_analogs_) saveAnalogs_(sims_arr, observations, station_i, test_time_i, flt_i);
                if (save_analogs_time_index_) saveAnalogsTimeIndex_(sims_arr, station_i, test_time_i, flt_i);
                if (save_sims_) saveSims_(sims_arr, station_i, test_time_i, flt_i);
                if (save_sims_time_index_) saveSimsTimeIndex_(sims_arr, station_i, test_time_i, flt_i);
                if (save_sims_station_index_) saveSimsStationIndex_(sims_arr, station_i, test_time_i, flt_i);

            } // End of loop for test times
        } // End of loop for flts
    } // End of loop for stations

    if (verbose_ >= Verbose::Progress) cout << "AnEnSSE generation done!" << endl;
    profiler_.log_time_session("Genrating analogs (AnEnSSE)");

    return;
}

void
AnEnSSE::print(ostream & os) const {

    // Use the parent class method
    AnEnIS::print(os);

    // Print extra information
    os << Config::_NUM_NEAREST << ": " << num_nearest_ << endl
            << Config::_DISTANCE << ": " << distance_ << endl
            << Config::_EXTEND_OBS << ": " << extend_obs_ << endl
            << Config::_SAVE_SIMS_STATION_IND << ": " << save_sims_station_index_ << endl;

    if (verbose_ >= Verbose::Debug) {
        os << "search stations index table dimensions: ["
                << search_stations_index_.size1() << ","
                << search_stations_index_.size2() << "]" << endl;

        if (save_sims_station_index_) os << "similarity station index array dimensions: ["
                << Functions::format(sims_station_index_.shape(), 4) << "]" << endl;
    }

    return;
}

ostream &
operator<<(ostream & os, const AnEnSSE & obj) {
    obj.print(os);
    return os;
}

AnEnSSE &
        AnEnSSE::operator=(const AnEnSSE & rhs) {

    if (this != &rhs) {
        AnEnIS::operator=(rhs);

        sims_station_index_ = rhs.sims_station_index_;
        num_nearest_ = rhs.num_nearest_;
        distance_ = rhs.distance_;
        extend_obs_ = rhs.extend_obs_;
        search_stations_index_ = rhs.search_stations_index_;
    }

    return *this;
}

size_t
AnEnSSE::num_nearest() const {
    return num_nearest_;
}

double
AnEnSSE::distance() const {
    return distance_;
}

bool
AnEnSSE::extend_obs() const {
    return extend_obs_;
}

bool
AnEnSSE::save_sims_station_index() const {
    return save_sims_station_index_;
}

const Array4DPointer &
AnEnSSE::sims_station_index() const {
    if (!save_sims_station_index_) throw runtime_error(
            "Similarity stations index array is not saved. Please change your configuration");

    return sims_station_index_;
}

const Functions::Matrix &
AnEnSSE::search_stations_index() const {
    return search_stations_index_;
}

void
AnEnSSE::preprocess_(const Forecasts & forecasts,
        const Observations & observations,
        vector<size_t> & fcsts_test_index,
        vector<size_t> & fcsts_search_index) {

    // Do everything that the parent class should be doing
    AnEnIS::preprocess_(forecasts, observations, fcsts_test_index, fcsts_search_index);

    // Find search stations for each test stations
    if (verbose_ >= Verbose::Progress) cout << "Computing search stations ..." << endl;
    const Stations & stations = forecasts.getStations();
    search_stations_index_.resize(stations.size(), num_nearest_);

    // Initialize the values to NAN
    auto & storage = search_stations_index_.data();
    fill_n(storage.begin(), storage.size(), NAN);

    // Set search stations for each test stations
    Functions::setSearchStations(stations, search_stations_index_, distance_, exclude_closest_location_);

    return;
}

void
AnEnSSE::allocateMemory_(const Forecasts & forecasts,
        const vector<size_t> & fcsts_test_index, const vector<size_t> & fcsts_search_index) {

    // Do everything that the parent class should be doing
    AnEnIS::allocateMemory_(forecasts, fcsts_test_index, fcsts_search_index);

    // Allocate memory for AnEnSSE
    if (save_sims_station_index_) {
        sims_station_index_.resize(
                forecasts.getStations().size(), fcsts_test_index.size(),
                forecasts.getFLTs().size(), num_sims_);
        sims_station_index_.initialize(NAN);
    }

    return;
}

bool
AnEnSSE::_simsSort_(const array<double, 4> & lhs,
        const array<double, 4> & rhs) {
    if (std::isnan(lhs[_SIM_VALUE_INDEX])) return false;
    if (std::isnan(rhs[_SIM_VALUE_INDEX])) return true;
    return (lhs[_SIM_VALUE_INDEX] < rhs[_SIM_VALUE_INDEX]);
}

void
AnEnSSE::setMembers_(const Config & config) {

    num_nearest_ = config.num_nearest;
    distance_ = config.distance;
    extend_obs_ = config.extend_obs;
    save_sims_station_index_ = config.save_sims_station_index;
    exclude_closest_location_ = config.exclude_closest_location;

    return;
}

void
AnEnSSE::checkSave_() const {

    if (save_analogs_ || save_analogs_time_index_ || save_sims_ ||
            save_sims_time_index_ || save_sims_station_index_) {
        // This is correct because at least one of them should be true
    } else {
        throw runtime_error("Neither analogs nor similarity are saved. Please check your configuration.");
    }

    return;
}

void
AnEnSSE::checkNumberOfMembers_(size_t num_search_times_index) {

    if (num_sims_ > num_search_times_index * num_nearest_) {
        num_sims_ = num_search_times_index * num_nearest_;

        if (verbose_ >= Verbose::Warning) {
            cerr << "Warning: The number of similarity has been changed to "
                    << num_sims_ << " due to the too short search period and too few search stations." << endl;
        }
    }

    if (num_analogs_ > num_search_times_index * num_nearest_) {
        num_analogs_ = num_search_times_index * num_nearest_;

        if (verbose_ >= Verbose::Warning) {
            cerr << "Warning: The number of analogs has been changed to "
                    << num_analogs_ << " due to the too short search period and too few search stations." << endl;
        }
    }

    return;
}
