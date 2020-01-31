/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   AnEnSSE.cpp
 * Author: guido
 * 
 * Created on January 29, 2020, 5:08 PM
 */

#include "AnEnSSE.h"
#include <algorithm>


#if defined(_OPENMP)
#include <omp.h>
#endif

using namespace std;
using namespace AnEnDefaults;
 
// These variables define the positions in the similarity array for different
// types of values.
//
static const size_t _SIM_VALUE_INDEX = 0;
static const size_t _SIM_FCST_INDEX = 1;
static const size_t _SIM_OBS_INDEX = 2;
static const size_t _SIM_STATION_INDEX = 3;

// This is the default value for similarity array
static const array<double, 4> _INIT_ARR_VALUE = {NAN, NAN, NAN, NAN};

static bool
simsSort(const array<double, 4> & lhs,
        const array<double, 4> & rhs) {
    if (std::isnan(lhs[_SIM_VALUE_INDEX])) return false;
    if (std::isnan(rhs[_SIM_VALUE_INDEX])) return true;
    return (lhs[_SIM_VALUE_INDEX] < rhs[_SIM_VALUE_INDEX]);
}

static bool
distSort(const pair<double, size_t> & lhs,
        const pair<double, size_t> & rhs) {
    return (lhs.first< rhs.first);
}


AnEnSSE::AnEnSSE() {
}

AnEnSSE::AnEnSSE(const AnEnSSE& orig) : AnEnIS(orig) {
    *this = orig;
}

AnEnSSE::~AnEnSSE() {
}

AnEnSSE &
AnEnSSE::operator=(const AnEnSSE & rhs) {
    if (this != &rhs) {
        AnEnIS::operator=(rhs);
        
        sims_station_ = rhs.sims_station_;
    }
    return *this;
}



void
AnEnSSE::compute(const Forecasts & forecasts,
        const Observations & observations,
        vector<size_t> & fcsts_test_index,
        vector<size_t> & fcsts_search_index) {

    if (verbose_ >= Verbose::Progress) cout << "Start AnEnSSE generation ..." << endl;

    preprocess_(forecasts, observations, fcsts_test_index, fcsts_search_index);
    
    
    /*
     * Read weights and circular flags from forecast parameters into vectors
     */
    const auto & parameters = forecasts.getParameters();

    vector<double> weights;
    vector<bool> circulars;

    parameters.getWeights(weights);
    parameters.getCirculars(circulars);

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
    std::vector< std::array<double, 4> > sims_arr;
    sims_arr.resize(num_search_times_index * 3); // XXX Just to test

    /*
     * Progress messages output
     */
    if (verbose_ >= Verbose::Detail) print(cout);
    if (verbose_ >= Verbose::Progress) cout << "Computing analogs ..." << endl;

#if defined(_OPENMP)
#pragma omp parallel for default(none) schedule(dynamic) collapse(3) \
shared(num_stations, num_flts, num_test_times_index, num_search_times_index, \
fcsts_test_index, fcsts_search_index, forecasts, observations, weights, circulars) \
firstprivate(sims_arr, _INIT_ARR_VALUE)
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
                 * Compute similarity for all search times
                 */
                for (size_t search_time_i = 0; search_time_i < num_search_times_index; ++search_time_i) {

                    size_t current_search_index = fcsts_search_index[search_time_i];

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
                    double obs_time_index = obsIndexTable_(search_time_i, flt_i);
                    if (std::isnan(obs_time_index)) continue;

                    // Check whether the associated observation is NA
                    double obs = observations.getValue(obs_var_index_, station_i, obs_time_index);
                    if (std::isnan(obs)) continue;

                    /***********************************************************
                     *                                                         *
                     *                   Computation Core                      *
                     *                                                         *
                     * Compute the metric for this station, flt, and test time *
                     *                                                         *
                     **********************************************************/

                    // Weights and circulars can be directly retrieved from
                    // forecasts. But for better performance, weights and circulars
                    // are pre-computed and passed.
                    //
                    double metric = computeSimMetric_(
                            forecasts, station_i, station_i, flt_i, current_test_index,
                            current_search_index, weights, circulars);

                    // Save the similarity metric with corresponding indices
                    sims_arr[search_time_i][_SIM_VALUE_INDEX] = metric;
                    sims_arr[search_time_i][_SIM_FCST_INDEX] = current_search_index;
                    sims_arr[search_time_i][_SIM_OBS_INDEX] = obs_time_index;
                }

                /*
                 * Sort based on similarity metrics
                 */
                if (quick_sort_) nth_element(sims_arr.begin(),
                        sims_arr.begin() + num_sims_, sims_arr.end(), simsSort);
                else partial_sort(sims_arr.begin(),
                        sims_arr.begin() + num_sims_, sims_arr.end(), simsSort);

                /*
                 * Output values and indices
                 */
                for (size_t analog_i = 0; analog_i < num_analogs_; ++analog_i) {

                    // Check whether the observation index is valid
                    double obs_time_index = sims_arr[analog_i][_SIM_OBS_INDEX];
                    if (std::isnan(obs_time_index)) continue;

                    double obs_value = observations.getValue(obs_var_index_, station_i, obs_time_index);

                    // Assign the analog value from the observation
                    analogsValue_.setValue(obs_value, station_i, test_time_i, flt_i, analog_i);

                    if (save_analogs_index_) {
                        analogsIndex_.setValue(obs_time_index, station_i, test_time_i, flt_i, analog_i);
                    }
                }

                if (save_sims_index_ || save_sims_) {
                    for (size_t sim_i = 0; sim_i < num_sims_; ++sim_i) {

                        if (save_sims_) {
                            // Assign similarity metric value
                            simsMetric_.setValue(sims_arr[sim_i][_SIM_VALUE_INDEX],
                                    station_i, test_time_i, flt_i, sim_i);
                        }

                        if (save_sims_index_) {
                            // Assign similarity metric index
                            simsIndex_.setValue(sims_arr[sim_i][_SIM_FCST_INDEX],
                                    station_i, test_time_i, flt_i, sim_i);
                        }
                    }
                }


            }
        }
    }

    if (verbose_ >= Verbose::Progress) cout << "AnEnIS generation done!" << endl;

    return;
}

void
AnEnSSE::preprocess_(const Forecasts & forecasts,
        const Observations & observations,
        vector<size_t> & fcsts_test_index,
        vector<size_t> & fcsts_search_index) {
        
    // Do everything that the parent class should be doing
    AnEnIS::preprocess_(forecasts, observations, fcsts_test_index, fcsts_search_index);
    
    Stations const & stations = forecasts.getStations();
    size_t num_stations = stations.size();
    size_t num_flts = forecasts.getFLTs().size();
    size_t num_test_times_index = fcsts_test_index.size();
    size_t num_search_times_index = fcsts_search_index.size();

    
    // And now compute the stations that are going to be used for the SSE part of the algorithm
    //
    const int max_stations = 9;
    search_stations_.resize(num_stations, max_stations); // Must redefine the 9
//    fill_n(search_stations_.data(), search_stations_.size1() * search_stations_.size2(), NAN);
    
    typedef vector<pair<double, size_t> > mypair;
    
    // Compute the distances.  This is inefficient and might be optimzied in the future
    //
    for ( size_t outer = 0 ; outer < num_stations ; ++outer ) {
         mypair distance(num_stations);
        
        for ( size_t inner = 0 ; inner < num_stations ; ++inner) {
//            auto search_x = stations.left[inner]->second.getX();
//            auto search_y = stations.left[inner]->second.getY();
//            
//            auto test_x = stations.left[outer]->second.getX();
//            auto test_y = stations.left[outer]->second.getY();
//            
//            distance[outer] = pair<double, size_t> (sqrt( pow( search_x - test_x, 2 ) + pow( search_y - test_y, 2 )  ), inner);
        }
         
        // Get the closest ones and it does not matter if they are internally sorted
        nth_element(distance.begin(), distance.begin() + max_stations, distance.end(), distSort);
        
        
        // copy the first few stations to the search stations data structure
//        for ( auto index = 0 ; index < distance.begin() + max_stations ; ++index ) {
//            
//            // Check if they are smaller than a threshold
//            search_stations_(outer, index) = distance[index].second;
//        }
        
    }
    
    
    
    
}









//
//void
//AnEnSSE::computeSearchStations(
//        const Stations & stations,
//        size_t max_num_search_stations,
//        double distance, size_t num_nearest_stations,
//        vector<size_t> stations_tag) const {
//
//    size_t num_stations = stations.size();
//    
//    // Check max number of search stations. If the number of nearest neighbor
//    // is fixed, this will also be the maximum number of search station.
//    //
//    if (num_nearest_stations != 0) {
//        max_num_search_stations = num_nearest_stations;
//    }
//    
//    // Make sure the tag vectors are initialized correctly
//    if (stations_tag.size() == 0) {
//        // If search station tags are not specified, it is assumed that
//        // all search stations have the same tag.
//        //
//        stations_tag.resize(num_stations, 0); // XXX magic number!!!
//    }
//    
////    if (verbose_ >= 3) cout << "Computing search space extension ... " << endl;
//
//    if (num_nearest_stations == 0) {
//        // In this case, the number of nearest neighbor stations is not 
//        // defined. The SSE criteria is the distance plus the station tags.
//        //
//
//        if (distance == 0) {
//            throw runtime_error("Missing parameters!");
//        }
//        
//#if defined(_OPENMP)
//#pragma omp parallel for default(none) schedule(static) \
//shared(stations, distance, max_num_search_stations, stations_tag, num_stations)
//#endif
//        for (size_t i_test = 0; i_test < num_stations; ++i_test) {
//
//            // Find search stations based on the distance
//            vector<size_t> id_search_station =
//                    search_stations.getStationsIdByDistance(
//                    test_stations_by_insert[i_test].getX(),
//                    test_stations_by_insert[i_test].getY(),
//                    distance);
//
//            // Assign search stations to matrix
//            for (size_t i_search = 0; i_search < max_num_search_stations &&
//                    i_search < id_search_station.size(); i_search++) {
//
//                // Convert from station ID to station index in search stations
//                size_t i_search_station = search_stations.getStationIndex(
//                        id_search_station[i_search]);
//
//                // Check the search station tags. Only add search stations
//                // when the tag is the same with the current test station.
//                //
//                if (test_station_tags[i_test] == search_station_tags[i_search_station]) {
//                    i_search_stations(i_test, i_search) = i_search_station;
//                }
//            }
//        }
//
//    } else {
//        // Otherwise, if the number of nearest neighbors has been assigned,
//        // search space extension becomes a k-neighbor search with the extra
//        // constraint on distance and test/search station tags.
//        //
//        search_stations.getNearestStationsIndex(
//                i_search_stations, test_stations, distance,
//                num_nearest_stations, test_station_tags, search_station_tags);
//    }
//    
//    return;
//}
