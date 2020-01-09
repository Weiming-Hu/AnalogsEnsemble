/* 
 * File:   AnEnIS.cpp
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <cervone@psu.edu>
 * 
 * Created on January 8, 2020, 11:40 AM
 */

#include "AnEnIS.h"
#include "Functions.h"
#include "colorTexts.h"

#include <algorithm>

using namespace std;
using namespace AnEnDefaults;

static const size_t _INIT_LEN = 1;

AnEnIS::AnEnIS() {
}

AnEnIS::AnEnIS(const AnEnIS& orig) : AnEn(orig) {
}

AnEnIS::AnEnIS(bool operational, bool time_overlap_check,
        bool save_sims, Verbose verbose) :
AnEn(operational, time_overlap_check, save_sims, verbose) {
}

AnEnIS::~AnEnIS() {
}

void
AnEnIS::compute(const Forecasts & forecasts, const Observations & observations,
        const Times & test_times, const Times & search_times) {

    if (verbose_ >= Verbose::Progress) {
        cout << GREEN << "Start AnEnIS generation ..." << RESET << endl;
    }

    /*
     * Check input
     * 
     * - Dimensions are correct
     * - Times exist in both forecasts and observations
     */

    // Initialize required variables
    const auto & fcst_times = forecasts.getTimes(),
            obs_times = observations.getTimes();
    const auto & fcst_flts = forecasts.getFLTs();

    /*
     * Convert Time objects to their corresponding indices
     */
    vector<size_t> fcsts_test_index, fcsts_search_index;
    Functions::toIndex(fcsts_test_index, test_times, fcst_times);
    Functions::toIndex(fcsts_search_index, search_times, fcst_times);

    /*
     * Compute the index mapping from forecast times and lead times to 
     * observations times
     */
    Functions::Matrix obs_index_table(
            search_times.size(), fcst_flts.size(), NAN);
    Functions::updateTimeTable(
            search_times, fcst_flts, obs_times, obs_index_table);

    /*
     * If operational mode is used, append test time indices to the end of
     * the search time indices.
     * 
     * The time table needs to be updated with the observation indices to,
     * not only search times, but also test times.
     */
    if (operational_) {
        fcsts_search_index.insert(fcsts_search_index.end(),
                fcsts_test_index.begin(), fcsts_test_index.end());

        // Check for ascending order
        if (!is_sorted(fcsts_search_index.begin(), fcsts_search_index.end())) {
            ostringstream msg;
            msg << BOLDRED << "Test must be after search in operation" << RESET;
            throw runtime_error(msg.str());
        }
        
        // Update the observation index table by adding test time indices
        obs_index_table.resize(obs_index_table.size1() + test_times.size(),
                obs_index_table.size2(), true);
        Functions::updateTimeTable(test_times, fcst_flts, obs_times,
                obs_index_table, search_times.size());
    }

    /*
     * Read weights and circular flags from forecast parameters into vectors
     */
    const auto & parameters = forecasts.getParameters();
    weights_.resize(parameters.size());
    circulars_.resize(parameters.size());
    for (size_t i = 0; i < weights_.size(); ++i) {
        weights_[i] = parameters.left[i].second.getWeight();
        circulars_[i] = parameters.left[i].second.getCircular();
    }

    /*
     * Compute standard deviations
     */
    fixedSds_(forecasts, fcsts_search_index);
    if (operational_) runningSds_(forecasts, fcsts_test_index);

    /*
     * Progress messages output
     * 
     * - Modes of this computation
     * - Times for this computation
     */

    /*
     * Compute analogs
     */

    /*
     * Wrap up
     */
    if (verbose_ >= Verbose::Progress) {
        cout << GREEN << "AnEnIS generation succeeded!" <<
                RESET << endl;
    }

    return;
}

void
AnEnIS::fixedSds_(const Forecasts& forecasts,
        const std::vector<size_t> times_index) {

    size_t num_times = times_index.size();
    size_t num_parameters = forecasts.getParameters().size();
    size_t num_stations = forecasts.getStations().size();
    size_t num_flts = forecasts.getFLTs().size();

    // Allocate array memory for parameters, stations, and forecast lead times
    sds_.resize(boost::extents[_INIT_LEN]
            [num_parameters][num_stations][num_flts]);
    size_t i_time = 0;

#if defined(_OPENMP)
#pragma omp parallel for default(none) schedule(dynamic) collapse(3) \
shared(num_parameters, num_stations, num_flts, num_times, forecasts, \
circulars_, weights_, sds_) firstprivate(i_time) 
#endif
    for (size_t parameter_i = 0; parameter_i < num_parameters; ++parameter_i) {
        for (size_t station_i = 0; station_i < num_stations; ++station_i) {
            for (size_t flt_i = 0; flt_i < num_flts; ++flt_i) {
                if (weights_[parameter_i] != 0) {
                    
                    // Extract values for times
                    vector<double> values(num_times);
                    for (size_t i = 0; i < num_times; ++i) {
                        i_time = times_index[i];
                        values[i] = forecasts.getValue(
                                parameter_i, station_i, i_time, flt_i);
                    }

                    if (circulars_[parameter_i]) {
                        sds_[0][parameter_i][station_i][flt_i] =
                                Functions::sdCircular(values);
                    } else {
                        sds_[0][parameter_i][station_i][flt_i] =
                                Functions::sdLinear(values);
                    }
                } 
            } // End of loop of lead times
        } // End of loop of stations
    } // End of loop of parameters

    return;
}

void
AnEnIS::runningSds_(const Forecasts & forecasts,
        const std::vector<size_t> & times_index) {
    
    // This function relies on existing values in the standard deviation array
    // to work. Make sure the array has the correct shape.
    //
    if (sds_.shape()[0] != _INIT_LEN ||
            sds_.shape()[1] != forecasts.getParameters().size() ||
            sds_.shape()[2] != forecasts.getStations().size() ||
            sds_.shape()[3] != forecasts.getFLTs().size()) {
        ostringstream msg;
        msg << BOLDRED << "Fixed sds has not been calculated yet" << RESET;
        throw runtime_error(msg.str());
    }
    
    // Resize the standard deviation array to store running calculation results
    size_t num_times = times_index.size();
    size_t num_parameters = sds_.shape()[1];
    size_t num_stations = sds_.shape()[2];
    size_t num_flts = sds_.shape()[3];
    sds_.resize(boost::extents[num_times]
            [num_parameters][num_stations][num_flts]);
    
    for (size_t time_i = 0; time_i < times_index.size(); ++time_i) {
        
        size_t time_prev = (time_i == 0 ? time_i : time_i - 1);
        
        for (size_t parameter_i = 0; parameter_i < num_parameters; ++parameter_i) {
            for (size_t station_i = 0; station_i < num_stations; ++station_i) {
                for (size_t flt_i = 0; flt_i < num_flts; ++flt_i) {
                    if (weights_[parameter_i] != 0) {
                        if (circulars_[parameter_i]) {
                            sds_[time_i][parameter_i][station_i][flt_i] = NAN;
                        } else {
                            
                            
                            
                            
                            
                            sds_[time_i][parameter_i][station_i][flt_i] = NAN;
                        }
                    }
                } // End of loop of lead times
            } // End of loop of stations
        } // End of loop of parameters
        
    } // End of loop of times
    
    return;
}