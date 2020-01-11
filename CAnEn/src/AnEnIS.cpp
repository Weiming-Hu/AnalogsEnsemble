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
    if (operational_) runningSds_(forecasts, fcsts_search_index, fcsts_test_index);
    else fixedSds_(forecasts, fcsts_search_index)

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

    size_t num_times = times_index.size(),
            num_parameters = forecasts.getParameters().size(),
            num_stations = forecasts.getStations().size(),
            num_flts = forecasts.getFLTs().size();

    // Allocate array memory for parameters, stations, and forecast lead times
    sds_.resize(boost::extents[_INIT_LEN]
            [num_parameters][num_stations][num_flts]);
    size_t i_time = 0;

#if defined(_OPENMP)
#pragma omp parallel for default(none) schedule(dynamic) collapse(3) \
shared(num_parameters, num_stations, num_flts, num_times, forecasts, \
times_index, circulars_, weights_, sds_) firstprivate(i_time) 
#endif
    for (size_t par_i = 0; par_i < num_parameters; ++par_i) {
        for (size_t sta_i = 0; sta_i < num_stations; ++sta_i) {
            for (size_t flt_i = 0; flt_i < num_flts; ++flt_i) {
                if (weights_[par_i] != 0) {
                    
                    // Extract values for times
                    vector<double> values(num_times);
                    for (size_t i = 0; i < num_times; ++i) {
                        i_time = times_index[i];
                        values[i] = forecasts.getValue(
                                par_i, sta_i, i_time, flt_i);
                    }

                    if (circulars_[par_i]) {
                        sds_[0][par_i][sta_i][flt_i] =
                                Functions::sdCircular(values);
                    } else {
                        sds_[0][par_i][sta_i][flt_i] =
                                Functions::sdLinear(values);
                    }
                } 
            } // End of loop of lead times
        } // End of loop of stations
    } // End of loop of parameters

    return;
}

void
AnEnIS::operationalSds_(const Forecasts & forecasts,
        const vector<size_t> & times_fixed_index,
        const vector<size_t> & times_running_index) {

    vector<size_t> times;
    times.insert(times.end(), times_fixed_index.begin(), times_fixed_index.end());
    times.insert(times.end(), times_running_index.begin(), times_running_index.end());

    size_t num_times = times.size(),
            num_times_fixed = times_fixed_index.size(),
            num_times_running = times_running_index.size(),
            num_parameters = forecasts.getParameters().size(),
            num_stations = forecasts.getStations().size(),
            num_flts = forecasts.getFLTs().size();
    
    
    // Allocate array memory for parameters, stations, and forecast lead times
    sds_.resize(boost::extents[num_times_running]
            [num_parameters][num_stations][num_flts]);

//#if defined(_OPENMP)
//#pragma omp parallel for default(none) schedule(dynamic) collapse(3) \
//shared(num_parameters, num_stations, num_flts, num_times, forecasts, \
//times_index, circulars_, weights_, sds_) firstprivate(i_time) 
//#endif
    for (size_t time_run_i = 0; time_run_i < num_times_running; ++time_run_i) {
        for (size_t par_i = 0; parameter_i < num_parameters; ++parameter_i) {
            for (size_t sta_i = 0; sta_i < num_stations; ++sta_i) {
                for (size_t flt_i = 0; flt_i < num_flts; ++flt_i) {
                    if (weights_[parameter_i] != 0) {

                        // Extract values for times
                        vector<double> values(num_times);
                        for (size_t i = 0; i < num_times; ++i) {
                            i_time = times_index[i];
                            values[i] = forecasts.getValue(
                                    par_i, sta_i, i_time, flt_i);
                        }

                        if (circulars_[par_i]) {
                            sds_[0][par_i][sta_i][flt_i] =
                                    Functions::sdCircular(values);
                        } else {
                            sds_[0][par_i][sta_i][flt_i] =
                                    Functions::sdLinear(values);
                        }
                    }
                } // End of loop of lead times
            } // End of loop of stations
        } // End of loop of parameters
    } // End of loop of running times

    return;
}

//void
//AnEnIS::runningSds_(const Forecasts & forecasts,
//        const vector<size_t> & times_fixed_index,
//        const vector<size_t> & times_running_index) {
//
//    size_t num_times_fixed = times_fixed_index.size(),
//            num_times_running = times_running_index.size(),
//            num_parameters = forecasts.getParameters().size(),
//            num_stations = forecasts.getStations().size(),
//            num_flts = forecasts.getFLTs().size();
//
//    // Allocate array memory for parameters, stations, and forecast lead times
//    sds_.resize(boost::extents[num_times_running]
//            [num_parameters][num_stations][num_flts]);
//    
//    // The following algorithm to calculate running standard deviation is
//    // adopted from Welford's algorithm, which is presented in Donald Knuth’s
//    // Art of Computer Programming, Vol 2, page 232, 3rd edition
//    //
//    size_t num_times = num_times_fixed + num_times_running,
//            cur_i = 0, prev_i = 0, valid_n = 0;
//    double M_old = 0.0, M_new = 0.0, fcst = 0.0;
//    boost<double, 3> M(boost::extents[num_parameters][num_stations][num_flts]);
//    
//    fill_n(sds_.data(), sds_.num_elements(), 0);
//    
//    {
//        // Initialize values in M to first sample from fixed-length time
//        for (size_t par_i = 0; par_i < num_parameters; ++par_i) {
//            for (size_t sta_i = 0; sta_i < num_stations; ++sta_i) {
//                for (size_t flt_i = 0; flt_i < num_flts; ++flt_i) {
//                    M[par_i][sta_i][flt_i] = forecasts.getValue(
//                            par_i, sta_i, times_fixed_index[0], flt_i);
//                }
//            }
//        }
//
//        // Start the calculation for running standard deviation
//        for (size_t par_i = 0; par_i < num_parameters; ++par_i) {
//            for (size_t sta_i = 0; sta_i < num_stations; ++sta_i) {
//                for (size_t flt_i = 0; flt_i < num_flts; ++flt_i) {
//                    
//                    if (weights_[par_i] != 0) {
//                        valid_n = 1;
//                        
//                        for (size_t time_i = 1; time_i < num_times; ++time_i) {
//                            M_old = M[par_i][sta_i][flt_i];
//
//                            if (time_i < num_times_fixed) {
//                                // Spin-up phase from which the standard 
//                                // deviation is not saved
//                                //
//                                fcst = forecasts.getValue(par_i, sta_i,
//                                        times_fixed_index[time_i], flt_i);
//                            } else {
//                                // Output phase from which the standard
//                                // deviation is saved
//                                //
//                                cur_i = time_i - num_times_fixed;
//                                prev_i = (cur_i == 0 ? 0 : cur_i - 1);
//                                fcst = forecasts.getValue(par_i, sta_i,
//                                        times_running_index[cur_i], flt_i);
//                            }
//
//                            if (!std::isnan(fcst)) {
//                                ++valid_n;
//                                
//                                if (circulars_[par_i]) {
//                                    // TODO
//                                } else {
//                                    M_new = M_old + (fcst - M_old) / valid_n;
//                                    sds_[cur_i][par_i][sta_i][flt_i] =
//                                            sds_[prev_i][par_i][sta_i][flt_i] +
//                                            (fcst - M_old) * (fcst - M_new);
//                                    M[par_i][sta_i][flt_i] = M_new;
//                                }
//                            }
//                        }
//                    }
//
//                } // End of loop of lead times
//            } // End of loop of stations
//        } // End of loop of parameters
//    }
//    
//    return;
//}