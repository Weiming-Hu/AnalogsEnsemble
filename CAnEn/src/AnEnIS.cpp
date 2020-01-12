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

static const size_t _SINGLE_LEN = 1;

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
    computeSds_(forecasts, fcsts_search_index, fcsts_test_index);

    /*
     * Progress messages output
     * 
     * - Modes of this computation
     * - Times for this computation
     */
    if (verbose_ >= Verbose::Detail) {
        cout << endl << "****************************" << endl
                << "AnEnIS session details:" << endl
                << "weights: " << Functions::format(weights_) << endl
                << "circulars: " << Functions::format(circulars_) << endl
                << "operational: " << operational_ << endl
                << "check time overlap: " << check_time_overlap_ << endl
                << "save similarity: " << save_sims_ << endl
                << "****************************" << endl << endl;
    }

    /*
     * Compute analogs
     */

    /*
     * Wrap up
     */
    if (verbose_ >= Verbose::Progress) {
        cout << GREEN << "AnEnIS process Done!" <<
                RESET << endl;
    }

    return;
}

void
AnEnIS::computeSds_(const Forecasts & forecasts,
        const vector<size_t> & times_fixed_index,
        const vector<size_t> & times_running_index) {
    
    size_t num_times_fixed = times_fixed_index.size(),
            num_times_running = times_running_index.size(),
            num_parameters = forecasts.getParameters().size(),
            num_stations = forecasts.getStations().size(),
            num_flts = forecasts.getFLTs().size(),
            num_times = 0;

    vector<size_t> times = times_fixed_index;
    
    if (operational_) {
        num_times = num_times_running;
        times.insert(times.end(),
                times_running_index.begin(), times_running_index.end());
    }

    sds_.resize(boost::extents
            [num_times][num_parameters][num_stations][num_flts]);

#if defined(_OPENMP)
#pragma omp parallel for default(none) schedule(dynamic) collapse(4) \
shared(num_times_fixed, num_times, num_parameters, num_stations, num_flts, \
circulars_, weights_, sds_, operational_, forecasts, times)
#endif
    for (size_t time_i = 0; time_i < num_times; ++time_i) {
        for (size_t par_i = 0; par_i < num_parameters; ++par_i) {
            for (size_t sta_i = 0; sta_i < num_stations; ++sta_i) {
                for (size_t flt_i = 0; flt_i < num_flts; ++flt_i) {

                    if (weights_[par_i] != 0) {
                        if (operational_) {
                            computeSd_(forecasts, times, time_i, par_i, sta_i,
                                    flt_i, num_times_fixed + time_i + 1);
                        } else {
                            computeSd_(forecasts, times,
                                    time_i, par_i, sta_i, flt_i);
                        }
                    }

                }
            }
        }
    }

    return;
}

void
AnEnIS::computeSd_(
        const Forecasts & forecasts, const vector<size_t> & times_index,
        size_t time_i, size_t par_i, size_t sta_i, size_t flt_i, size_t count) {
    
    if (count == 0) count = times_index.size();
    vector<double> values(count);

    // Copy values from the array to a vector
    for (size_t i = 0; i < count; ++i) {
        values[i] = forecasts.getValue(par_i, sta_i, times_index[i], flt_i);
    }

    if (circulars_[par_i]) {
        sds_[time_i][par_i][sta_i][flt_i] = Functions::sdCircular(values);
    } else {
        sds_[time_i][par_i][sta_i][flt_i] = Functions::sdLinear(values);
    }
    
    return;
}