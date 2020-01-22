/* 
 * File:   AnEnIS.cpp
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <cervone@psu.edu>
 * 
 * Created on January 8, 2020, 11:40 AM
 */

#include "AnEnIS.h"
#include "colorTexts.h"
#include "Calculator.h"

#include <algorithm>

using namespace std;
using namespace AnEnDefaults;

static const size_t _PREVIEW_COUNT = 5;
static const size_t _SINGLE_LEN = 1;
static const size_t _SIM_VALUE_INDEX = 0;
static const size_t _SIM_FCST_INDEX = 1;
static const size_t _SIM_OBS_INDEX = 2;
static const array<double, 3> _INIT_ARR_VALUE = {NAN, NAN, NAN};

static bool
simsSort(const array<double, 3> & lhs,
        const array<double, 3> & rhs) {
    if (std::isnan(lhs[_SIM_VALUE_INDEX])) return false;
    if (std::isnan(rhs[_SIM_VALUE_INDEX])) return true;
    return (lhs[_SIM_VALUE_INDEX] < rhs[_SIM_VALUE_INDEX]);
}

AnEnIS::AnEnIS() : AnEn(),
num_analogs_(AnEnDefaults::_NUM_ANALOGS),
obs_var_index_(AnEnDefaults::_OBS_VAR_INDEX),
quick_sort_(AnEnDefaults::_QUICK_SORT),
save_sims_index_(AnEnDefaults::_SAVE_SIMS_INDEX),
save_analogs_index_(AnEnDefaults::_SAVE_ANALOGS_INDEX),
num_sims_(AnEnDefaults::_NUM_SIMS),
max_par_nan_(AnEnDefaults::_MAX_PAR_NAN),
max_flt_nan_(AnEnDefaults::_MAX_FLT_NAN),
flt_radius_(AnEnDefaults::_FLT_RADIUS) {
}

AnEnIS::AnEnIS(const AnEnIS& orig) : AnEn(orig) {
    num_analogs_ = orig.num_analogs_;
    obs_var_index_ = orig.obs_var_index_;
    quick_sort_ = orig.quick_sort_;
    save_sims_index_ = orig.save_sims_index_;
    save_analogs_index_ = orig.save_analogs_index_;
    num_sims_ = orig.num_sims_;
    max_par_nan_ = orig.max_par_nan_;
    max_flt_nan_ = orig.max_flt_nan_;
    flt_radius_ = orig.flt_radius_;
}

AnEnIS::AnEnIS(size_t num_members,
        bool operational,
        bool check_search_future,
        bool save_sims,
        Verbose verbose,
        size_t obs_var_index,
        bool quick_sort,
        bool save_sims_index,
        bool save_analogs_index,
        size_t num_sims,
        size_t max_par_nan,
        size_t max_flt_nan,
        size_t flt_radius) :
AnEn(operational, check_search_future, save_sims, verbose),
num_analogs_(num_members), obs_var_index_(obs_var_index),
quick_sort_(quick_sort), save_sims_index_(save_sims_index),
save_analogs_index_(save_analogs_index), num_sims_(num_sims),
max_par_nan_(max_par_nan), max_flt_nan_(max_flt_nan), flt_radius_(flt_radius) {
    if (num_sims_ < num_analogs_) num_sims_ = num_analogs_;
    if (operational_) check_search_future_ = true;
}

AnEnIS::~AnEnIS() {
}

void
AnEnIS::compute(const Forecasts & forecasts,
        const Observations & observations,
        const Times & test_times, const Times & search_times) {

    const auto & fcst_times = forecasts.getTimes();

    /*
     * Convert Time objects to their corresponding indices
     */
    vector<size_t> fcsts_test_index, fcsts_search_index;
    Functions::toIndex(fcsts_test_index, test_times, fcst_times);
    Functions::toIndex(fcsts_search_index, search_times, fcst_times);

    compute(forecasts, observations, fcsts_test_index, fcsts_search_index);
    return;
}

void
AnEnIS::compute(const Forecasts & forecasts,
        const Observations & observations,
        vector<size_t> fcsts_test_index,
        vector<size_t> fcsts_search_index) {

    if (verbose_ >= Verbose::Progress) cout << GREEN
            << "Start AnEnIS generation ..." << RESET << endl;
    
    checkIndexRange_(forecasts, fcsts_test_index, fcsts_search_index);
    checkConsistency_(forecasts, observations);

    /*
     * Compute standard deviations
     */
    computeSds_(forecasts, fcsts_search_index, fcsts_test_index);

    /*
     * If operational mode is used, append test time indices to the end of
     * the search time indices.
     */
    if (operational_) {
        fcsts_search_index.insert(fcsts_search_index.end(),
                fcsts_test_index.begin(), fcsts_test_index.end());
    }

    // Check for ascending order
    if (!is_sorted(fcsts_search_index.begin(), fcsts_search_index.end())) {
        ostringstream msg;
        msg << BOLDRED << "Test must be after search in operation" << RESET;
        throw runtime_error(msg.str());
    }

    /*
     * Compute the index mapping from forecast times and lead times to 
     * observations times
     */
    const auto & fcst_times = forecasts.getTimes(),
            obs_times = observations.getTimes();
    const auto & fcst_flts = forecasts.getFLTs();

    obsIndexTable_ = Functions::Matrix(
            fcsts_search_index.size(), fcst_flts.size(), NAN);
    Functions::updateTimeTable(fcst_times,
            fcsts_search_index, fcst_flts, obs_times, obsIndexTable_);

    /*
     * Read weights and circular flags from forecast parameters into vectors
     */
    const auto & parameters = forecasts.getParameters();

    vector<double> weights;
    vector<bool> circulars;

    parameters.getWeights(weights);
    parameters.getCirculars(circulars);

    /*
     * Pre-allocate memory for analog computation
     */
    if (verbose_ >= Verbose::Progress) cout << GREEN
            << "Allocating memory ..." << RESET << endl;

    size_t num_stations = forecasts.getStations().size(),
            num_flts = forecasts.getFLTs().size(),
            num_test_times_index = fcsts_test_index.size(),
            num_search_times_index = fcsts_search_index.size();

    analogsValue_.resize(boost::extents
            [num_stations][num_test_times_index][num_flts][num_analogs_]);
    fill_n(analogsValue_.data(), analogsValue_.num_elements(), NAN);

    if (save_analogs_index_) {
        analogsIndex_.resize(boost::extents
                [num_stations][num_test_times_index][num_flts][num_analogs_]);
        fill_n(analogsIndex_.data(), analogsIndex_.num_elements(), NAN);
    }

    if (save_sims_) {
        simsMetric_.resize(boost::extents
                [num_stations][num_test_times_index][num_flts][num_sims_]);
        fill_n(simsMetric_.data(), simsMetric_.num_elements(), NAN);
    }

    if (save_sims_index_) {
        simsIndex_.resize(boost::extents
                [num_stations][num_test_times_index][num_flts][num_sims_]);
        fill_n(simsIndex_.data(), simsIndex_.num_elements(), NAN);
    }

    /*
     * Progress messages output
     */
    if (verbose_ >= Verbose::Detail) print(cout);
    if (verbose_ >= Verbose::Progress) cout << GREEN << "Computing analogs ..." << RESET << endl;

#if defined(_OPENMP)
#pragma omp parallel for default(none) schedule(dynamic) collapse(3) \
shared(num_stations, num_flts, num_test_times_index, num_search_times_index, \
fcsts_test_index, fcsts_search_index, forecasts, observations, weights, circulars)
#endif
    for (size_t sta_i = 0; sta_i < num_stations; ++sta_i) {
        for (size_t flt_i = 0; flt_i < num_flts; ++flt_i) {
            for (size_t time_test_i = 0; time_test_i < num_test_times_index; ++time_test_i) {

                size_t current_test_index = fcsts_test_index[time_test_i];
                double current_test_time = forecasts.getTimeStamp(current_test_index);
                double current_flt_offset = forecasts.getFltTimeStamp(flt_i);
                
                // Initialize similarity array values
                simsArr_.resize(0);
                simsArr_.resize(num_search_times_index, _INIT_ARR_VALUE);

                /*
                 * Compute similarity for all search times
                 */
                for (size_t time_search_i = 0; time_search_i < num_search_times_index; ++time_search_i) {

                    size_t current_search_index = fcsts_search_index[time_search_i];
                    
                    /*
                     * Comparing to the test forecast itself is strictly forbidden
                     */
                    if (current_search_index == current_test_index) continue;

                    if (check_search_future_) {
                        /*
                         * Check whether current valid forecast time exceeds test forecast initialization time
                         * for which the corresponding observation is not available
                         */
                        double current_search_time = forecasts.getTimeStamp(current_search_index);
                        if (current_search_time + current_flt_offset >= current_test_time) continue;
                    }

                    // Check whether this search time is found in observations
                    double obs_time_index = obsIndexTable_(time_search_i, flt_i);
                    if (std::isnan(obs_time_index)) continue;

                    // Check whether the associated observation is NA
                    double obs = observations.getValue(obs_var_index_, sta_i, obs_time_index);
                    if (std::isnan(obs)) continue;

                    /***********************************************************
                     *                                                         *
                     *                   Computation Core                      *
                     *                                                         *
                     * Compute the metric for this station, flt, and test time *
                     *                                                         *
                     **********************************************************/
                    double metric = computeSimMetric_(
                            forecasts, sta_i, flt_i, current_test_index,
                            current_search_index, weights, circulars);

                    // Save the similarity metric with corresponding indices
                    simsArr_[time_search_i][_SIM_VALUE_INDEX] = metric;
                    simsArr_[time_search_i][_SIM_FCST_INDEX] = current_search_index;
                    simsArr_[time_search_i][_SIM_OBS_INDEX] = obs_time_index;
                }

                /*
                 * Sort based on similarity metrics
                 */
                if (quick_sort_) nth_element(simsArr_.begin(),
                        simsArr_.begin() + num_sims_, simsArr_.end(), simsSort);
                else partial_sort(simsArr_.begin(),
                        simsArr_.begin() + num_sims_, simsArr_.end(), simsSort);

                /*
                 * Output values and indices
                 */
                for (size_t analog_i = 0; analog_i < num_analogs_; ++analog_i) {

                    double obs_time_index = simsArr_[analog_i][_SIM_OBS_INDEX];
                    if (std::isnan(obs_time_index)) continue;

                    double obs_value = observations.getValue(obs_var_index_, sta_i, obs_time_index);

                    // Assign the analog value from the observation
                    analogsValue_[sta_i][time_test_i][flt_i][analog_i] = obs_value;

                    if (save_analogs_index_) {
                        analogsIndex_[sta_i][time_test_i][flt_i][analog_i] = obs_time_index;
                    }
                }

                if (save_sims_index_ || save_sims_) {
                    for (size_t sim_i = 0; sim_i < num_sims_; ++sim_i) {

                        if (save_sims_) {
                            // Assign similarity metric value
                            simsMetric_[sta_i][time_test_i][flt_i][sim_i] =
                                    simsArr_[sim_i][_SIM_VALUE_INDEX];
                        }

                        if (save_sims_index_) {
                            // Assign similarity metric index
                            simsIndex_[sta_i][time_test_i][flt_i][sim_i] =
                                    simsArr_[sim_i][_SIM_FCST_INDEX];
                        }
                    }
                }
                
                
            }
        }
    }

    if (verbose_ >= Verbose::Progress) cout << GREEN << "AnEnIS generation done!" << RESET << endl;
    return;
}

const Array4D &
AnEnIS::getSimsValue() const {
    return simsMetric_;
}

const Array4D &
AnEnIS::getSimsIndex() const {
    return simsIndex_;
}

const Array4D &
AnEnIS::getAnalogsValue() const {
    return analogsValue_;
}

const Array4D &
AnEnIS::getAnalogsIndex() const {
    return analogsIndex_;
}

void
AnEnIS::print(std::ostream & os) const {

    os << endl << "****************************" << endl
            << "AnEnIS session details:" << endl
            << "number of analogs: " << num_analogs_ << endl
            << "number of similarity: " << num_sims_ << endl
            << "observation variable index: " << obs_var_index_ << endl
            << "quick sort: " << quick_sort_ << endl
            << "operational: " << operational_ << endl
            << "check time overlap: " << check_search_future_ << endl
            << "save analog time indices: " << save_analogs_index_ << endl
            << "save similarity: " << save_sims_ << endl
            << "save similarity time indices: " << save_sims_index_ << endl;

    if (verbose_ >= Verbose::Debug) {
        os << "sds_ dimensions: [" << Functions::format(
                sds_.shape(), sds_.num_dimensions())
                << "]" << endl << "similarityMetric_ dimensions: ["
                << Functions::format(simsMetric_.shape(),
                simsMetric_.num_dimensions())
                << "]" << endl << "similarityIndex_ dimensions: ["
                << Functions::format(simsIndex_.shape(),
                simsIndex_.num_dimensions())
                << "]" << endl << "analogsValue_ dimensions: ["
                << Functions::format(analogsValue_.shape(),
                analogsIndex_.num_dimensions())
                << "]" << endl << "analogsIndex_ dimensions: ["
                << Functions::format(analogsIndex_.shape(),
                analogsValue_.num_dimensions())
                << "]" << endl << "obs_index_table_ dimensions: ["
                << obsIndexTable_.size1() << ","
                << obsIndexTable_.size2() << "]" << endl;
    }

    cout << "****************************" << endl << endl;
    return;
}

std::ostream &
operator<<(std::ostream & os, const AnEnIS & obj) {
    obj.print(os);
    return os;
}

double
AnEnIS::computeSimMetric_(const Forecasts & forecasts,
        size_t sta_i, size_t flt_i, size_t time_test_i, size_t time_search_i,
        const vector<double> & weights, const vector<bool> & circulars) {

    /*
     * Prepare for similarity metric calculation
     */
    double sim = 0, sd = 0;
    size_t count_par_nan = 0;
    size_t num_pars = forecasts.getParameters().size();
    size_t num_flts = forecasts.getFLTs().size();
    size_t flt_i_start = (flt_i <= flt_radius_ ? 0 : flt_i - flt_radius_);
    size_t flt_i_end = (flt_i + flt_radius_ >= num_flts ? num_flts - 1 : flt_i + flt_radius_);
    vector<double> window(flt_i_end - flt_i_start + 1);

    /*
     * Calculate similarity metric
     */
    for (size_t par_i = 0; par_i < num_pars; par_i++) {

        if (weights[par_i] == 0) continue;

        for (size_t pos = 0, window_i = flt_i_start; window_i <= flt_i_end; ++window_i, ++pos) {

            double value_search = forecasts.getValue(par_i, sta_i, time_search_i, window_i);
            double value_test = forecasts.getValue(par_i, sta_i, time_test_i, window_i);

            if (std::isnan(value_search) || std::isnan(value_test)) {
                window[pos] = NAN;
            } else {
                if (circulars[par_i]) {
                    window[pos] = pow(Functions::diffCircular(value_search, value_test), 2);
                } else {
                    window[pos] = pow(value_search - value_test, 2);
                }
            }
        } // End loop of the lead time window

        double average = Functions::mean(window, max_flt_nan_);

        if (std::isnan(average)) {
            ++count_par_nan;
            if (count_par_nan > max_par_nan_) return NAN;
        } else {

            if (operational_) {
                sd = sds_[par_i][sta_i][flt_i][time_test_i];
            } else {
                sd = sds_[par_i][sta_i][flt_i][0];
            }

            sim += weights[par_i] * (sqrt(average) / sd);
        }

    } // End loop of parameters

    return sim;
}

void
AnEnIS::computeSds_(const Forecasts & forecasts,
        const vector<size_t> & times_fixed_index,
        const vector<size_t> & times_accum_index) {

    if (verbose_ >= Verbose::Detail) {
        cout << "Computing standard deviation ..." << RESET << endl;
    }

    if (operational_ && times_accum_index.size() == 0) {
        ostringstream msg;
        msg << BOLDRED << "times_accum_index is required in operation" << RESET;
        throw runtime_error(msg.str());
    }

    size_t num_times = (operational_ ? times_accum_index.size() : _SINGLE_LEN);
    size_t num_parameters = forecasts.getParameters().size();
    size_t num_stations = forecasts.getStations().size();
    size_t num_flts = forecasts.getFLTs().size();
    size_t calculator_capacity = times_fixed_index.size();
    if (operational_) calculator_capacity += times_accum_index.size();
    
    vector<double> weights;
    vector<bool> circulars;

    forecasts.getParameters().getWeights(weights);
    forecasts.getParameters().getCirculars(circulars);

    // Pre-allocate memory for calculation
    sds_.resize(boost::extents
            [num_parameters][num_stations][num_flts][num_times]);
    fill_n(sds_.data(), sds_.num_elements(), NAN);

#if defined(_OPENMP)
#pragma omp parallel for default(none) schedule(dynamic) collapse(3) \
shared(num_parameters, num_stations, num_flts, forecasts, times_fixed_index, \
times_accum_index, weights, circulars, num_times, calculator_capacity)
#endif
    for (size_t par_i = 0; par_i < num_parameters; ++par_i) {
        for (size_t sta_i = 0; sta_i < num_stations; ++sta_i) {
            for (size_t flt_i = 0; flt_i < num_flts; ++flt_i) {

                // Skip the iteration if the weight is 0
                if (weights[par_i] == 0) continue;

                // Initialize a calculator
                Calculator calc(circulars[par_i], calculator_capacity);
                double value;

                // Push values into the calculator if it is not NAN
                for (size_t i = 0; i < times_fixed_index.size(); ++i) {
                    value = forecasts.getValue(par_i, sta_i, times_fixed_index[i], flt_i);
                    
                    // Remove NAN value
                    if (!std::isnan(value)) calc.pushValue(value);
                }

                // Calculate standard deviation
                sds_[par_i][sta_i][flt_i][0] = calc.sd();

                if (operational_) {
                    for (size_t time_i = 1; time_i < num_times; ++time_i) {

                        // Get the forecast value
                        value = forecasts.getValue(par_i, sta_i, times_accum_index[time_i - 1], flt_i);

                        if (std::isnan(value)) {
                            // Copy the value from previous iteration if the value is NAN
                            sds_[par_i][sta_i][flt_i][time_i] = sds_[par_i][sta_i][flt_i][time_i - 1];

                        } else {
                            // Push the valid value into the calculator
                            calc.pushValue(value);

                            // Update the running standard deviation
                            sds_[par_i][sta_i][flt_i][time_i] = calc.sd();
                        }
                    } // End of loop of accumulated time indices
                }
            } // End of loop of FLTs
        } // End of loop of stations
    } // End of loop of parameters

    if (verbose_ >= Verbose::Debug) {
        cout << "Standard deviations: ";
        if (sds_.num_elements() > _PREVIEW_COUNT) {
            cout << Functions::format(sds_.data(), _PREVIEW_COUNT, ",") << ", ...";
        } else {
            cout << Functions::format(sds_.data(), sds_.num_elements());
        }
        cout << endl;
    }

    return;
}

void
AnEnIS::checkIndexRange_(const Forecasts & forecasts,
        const vector<size_t> & fcsts_test_index,
        const vector<size_t> & fcsts_search_index) const {
    
    size_t num_times = forecasts.getTimes().size();
    
    auto max_it = max_element(fcsts_test_index.begin(), fcsts_test_index.end());
    if (*max_it >= num_times) {
        ostringstream msg;
        msg << BOLDRED << "Forecast test time index maximum (" << *max_it
                << ") exceeds the number of times (" << num_times
                << ")" << RESET;
        throw range_error(msg.str());
    }
    
    max_it = max_element(fcsts_search_index.begin(), fcsts_search_index.end());
    if (*max_it >= num_times) {
        ostringstream msg;
        msg << BOLDRED << "Forecast search time index maximum (" << *max_it
                << ") exceeds the number of times (" << num_times
                << ")" << RESET;
        throw range_error(msg.str());
    }
    
    return;
}

void
AnEnIS::checkConsistency_(const Forecasts & forecasts,
        const Observations & observations) const {
    
    if (forecasts.getStations().size() != observations.getStations().size()) {
        ostringstream msg;
        msg << BOLDRED << "#forecast stations (" 
                << forecasts.getStations().size() 
                << ") != #observation stations (" <<
                observations.getStations().size() << ")" << RESET;
        throw runtime_error(msg.str());
    }

    return;
}