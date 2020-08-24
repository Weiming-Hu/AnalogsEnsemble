/* 
 * File:   AnEnIS.cpp
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <cervone@psu.edu>
 * 
 * Created on January 8, 2020, 11:40 AM
 */

#include "AnEnIS.h"
#include "Calculator.h"

#include <algorithm>
#include <stdexcept>

#if defined(_OPENMP)
#include <omp.h>
#endif

#if defined(_ENABLE_AI)
#include <ATen/ATen.h>
#endif

using namespace std;

// When operational mode is OFF, standard deviation is computed across all
// search days, thus having only a length of one for the time dimension.
//
static const size_t _SINGLE_LEN = 1;

const size_t AnEnIS::_SIM_VALUE_INDEX = 0;
const size_t AnEnIS::_SIM_FCST_TIME_INDEX = 1;
const size_t AnEnIS::_SIM_OBS_TIME_INDEX = 2;
const array<double, 3> AnEnIS::_INIT_ARR_VALUE = {NAN, NAN, NAN};

AnEnIS::AnEnIS() : AnEn() {
    Config config;
    setMembers_(config);
}

AnEnIS::AnEnIS(const AnEnIS& orig) : AnEn(orig) {
    *this = orig;
}

AnEnIS::AnEnIS(const Config & config) : AnEn(config) {
    setMembers_(config);

    if (num_sims_ < num_analogs_) num_sims_ = num_analogs_;

    if (operation_ && !prevent_search_future_) {
        if (verbose_ >= Verbose::Warning) {
            cerr << "Warning: Future search is disallowed in operational mode" << endl;
        }

        prevent_search_future_ = true;
    }
}

AnEnIS::~AnEnIS() {
}

void
AnEnIS::compute(const Forecasts & forecasts, const Observations & observations,
        const Times & test_times, const Times & search_times) {

    const auto & fcst_times = forecasts.getTimes();

    /*
     * Convert Time objects to their corresponding indices
     */
    vector<size_t> fcsts_test_index, fcsts_search_index;

    try {
        Functions::toIndex(fcsts_test_index, test_times, fcst_times);
    } catch (exception & ex) {
        string msg = string("toIndex(test_times, ...) -> ") + ex.what();
        throw runtime_error(msg);
    }

    try {
        Functions::toIndex(fcsts_search_index, search_times, fcst_times);
    } catch (exception & ex) {
        string msg = string("toIndex(search_times, ...) -> ") + ex.what();
        throw runtime_error(msg);
    }

    compute(forecasts, observations, fcsts_test_index, fcsts_search_index);

    return;
}

void
AnEnIS::compute(const Forecasts & forecasts,
        const Observations & observations,
        vector<size_t> & fcsts_test_index,
        vector<size_t> & fcsts_search_index) {

    if (verbose_ >= Verbose::Progress) cout << "Start AnEnIS generation ..." << endl;

    preprocess_(forecasts, observations, fcsts_test_index, fcsts_search_index);
    profiler_.log_time_session("Preprocessing (AnEnIS)");

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
     * [similarity value, forecast time index, observation time index]
     * 
     * This is used to store all similarity metrics from all search times for
     * one test time together with indices.
     */
    SimsVec<3> sims_arr;
    sims_arr.resize(num_search_times_index);

    /*
     * Progress messages output
     */
    if (verbose_ >= Verbose::Detail) {
        cout << "************** AnEn Configuration Summary **************" << endl;
        print(cout);
        cout << "*********** End of AnEn Configuration Summary **********" << endl
            << "************** AnEn Computation Summary **************" << endl
            << "Number of stations: " << num_stations << endl
            << "Number of test times: " << num_test_times_index << endl
            << "Number of lead times: " << num_flts << endl
            << "Number of search times: " << fcsts_search_index.size() << endl
#if defined(_OPENMP)
            << "Number of threads to be created: " << omp_get_max_threads() << endl
#endif
            << "*********** End of AnEn Computation Summary **********" << endl;
    }

    if (verbose_ >= Verbose::Progress) cout << "Computing analogs ..." << endl;

    // Prepare variables for progress bar
    size_t total_count = num_stations * num_flts * num_test_times_index;
    size_t counter = 0, current_percent = 0;
    size_t pbar_threshold = total_count * 0.01;

#if defined(_OPENMP)
#pragma omp parallel for default(none) schedule(dynamic) collapse(3) \
shared(num_stations, num_flts, num_test_times_index, num_search_times_index, \
fcsts_test_index, fcsts_search_index, forecasts, observations, circulars, \
total_count, counter, current_percent, pbar_threshold, std::cout) firstprivate(sims_arr)
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
                    double obs_time_index = obs_time_index_table_(search_time_i, flt_i);
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

                    // Circulars can be directly retrieved from
                    // forecasts. But for better performance, circulars
                    // are pre-computed and passed.
                    //
                    double metric;

#if defined(_ENABLE_AI)
                    if (use_AI_) {
                        metric = computeSimMetricAI_(
                                forecasts, station_i, station_i, flt_i,
                                current_test_index, current_search_index);

                    } else {
#endif
                        metric = computeSimMetric_(
                                forecasts, station_i, station_i, flt_i, current_test_index,
                                current_search_index, circulars);
#if defined(_ENABLE_AI)
                    }
#endif

                    // Save the similarity metric with corresponding indices
                    sims_arr[search_time_i][_SIM_VALUE_INDEX] = metric;
                    sims_arr[search_time_i][_SIM_FCST_TIME_INDEX] = current_search_index;
                    sims_arr[search_time_i][_SIM_OBS_TIME_INDEX] = obs_time_index;
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


                /*
                 * Print progress information
                 */
                if (verbose_ >= Verbose::Detail) {
#if defined(_OPENMP)
#pragma omp atomic
#endif
                    counter++;

#if defined(_OPENMP)
                    if (omp_get_thread_num() == 0) {
#endif
                        if (counter > pbar_threshold) {
                            current_percent += counter / (float) total_count * 100;
                            cout << '\r' << "Progress: " << current_percent << "%";
                            cout.flush();
                            counter = 0;
                        }
#if defined(_OPENMP)
                    }
#endif
                }

            } // End loop of test times
        } // End loop of lead times
    } // End loop of stations

    if (verbose_ >= Verbose::Detail) cout << '\r' << "Progress: 100%" << endl;
    if (verbose_ >= Verbose::Progress) cout << "AnEnIS generation done!" << endl;
    profiler_.log_time_session("Generating analogs (AnEnIS)");

    return;
}

void
AnEnIS::print(ostream & os) const {

    AnEn::print(os);

    os << Config::_NUM_ANALOGS << ": " << num_analogs_ << endl
            << Config::_NUM_SIMS << ": " << num_sims_ << endl
            << Config::_OBS_ID << ": " << obs_var_index_ << endl
            << Config::_NUM_PAR_NA << ": " << max_par_nan_ << endl
            << Config::_NUM_FLT_NA << ": " << max_flt_nan_ << endl
            << Config::_FLT_RADIUS << ": " << flt_radius_ << endl
            << Config::_SAVE_ANALOGS << ": " << save_analogs_ << endl
            << Config::_SAVE_ANALOGS_TIME_IND << ": " << save_analogs_time_index_ << endl
            << Config::_SAVE_SIMS << ": " << save_sims_ << endl
            << Config::_SAVE_SIMS_TIME_IND << ": " << save_sims_time_index_ << endl
            << Config::_OPERATION << ": " << operation_ << endl
            << Config::_QUICK << ": " << quick_sort_ << endl
            << Config::_PREVENT_SEARCH_FUTURE << ": " << prevent_search_future_ << endl
#if defined(_ENABLE_AI)
            << "Use AI: " << use_AI_ << endl
#endif
            << Config::_WEIGHTS << ": " << Functions::format(weights_) << endl;

    if (verbose_ >= Verbose::Debug) {
        os << "standard deviation array dimensions: ["
                << Functions::format(sds_.shape(), 4) << "]" << endl;
        os << "observations time index table dimensions: ["
                << obs_time_index_table_.size1() << ","
                << obs_time_index_table_.size2() << "]" << endl;

        if (save_sims_) os << "similarity metric array dimensions: ["
                << Functions::format(sims_metric_.shape(), 4) << "]" << endl;

        if (save_sims_time_index_) os << "similarity time index array dimensions: ["
                << Functions::format(sims_time_index_.shape(), 4) << "]" << endl;

        if (save_analogs_) os << "analogs arary dimensions: ["
                << Functions::format(analogs_value_.shape(), 4) << "]" << endl;

        if (save_analogs_time_index_) os << "analogs time index array dimensions: ["
                << Functions::format(analogs_time_index_.shape(), 4) << "]" << endl;
    }

    return;
}

ostream &
operator<<(ostream & os, const AnEnIS & obj) {
    obj.print(os);
    return os;
}

AnEnIS &
AnEnIS::operator=(const AnEnIS& rhs) {

    if (this != &rhs) {
        num_analogs_ = rhs.num_analogs_;
        num_sims_ = rhs.num_sims_;
        obs_var_index_ = rhs.obs_var_index_;
        max_par_nan_ = rhs.max_par_nan_;
        max_flt_nan_ = rhs.max_flt_nan_;
        flt_radius_ = rhs.flt_radius_;
        save_analogs_ = rhs.save_analogs_;
        save_analogs_time_index_ = rhs.save_analogs_time_index_;
        save_sims_ = rhs.save_sims_;
        save_sims_time_index_ = rhs.save_sims_time_index_;
        operation_ = rhs.operation_;
        quick_sort_ = rhs.quick_sort_;
        prevent_search_future_ = rhs.prevent_search_future_;
        sds_ = rhs.sds_;
        sds_time_index_map_ = rhs.sds_time_index_map_;
        sims_metric_ = rhs.sims_metric_;
        sims_time_index_ = rhs.sims_time_index_;
        analogs_value_ = rhs.analogs_value_;
        analogs_time_index_ = rhs.analogs_time_index_;
    }

    return *this;
}

size_t AnEnIS::num_analogs() const {
    return num_analogs_;
}

size_t AnEnIS::num_sims() const {
    return num_sims_;
}

size_t AnEnIS::obs_var_index() const {
    return obs_var_index_;
}

size_t AnEnIS::max_par_nan() const {
    return max_par_nan_;
}

size_t AnEnIS::max_flt_nan() const {
    return max_flt_nan_;
}

size_t AnEnIS::flt_radius() const {
    return flt_radius_;
}

bool AnEnIS::save_analogs() const {
    return save_analogs_;
}

bool AnEnIS::save_analogs_time_index() const {
    return save_analogs_time_index_;
}

bool AnEnIS::save_sims() const {
    return save_sims_;
}

bool AnEnIS::save_sims_time_index() const {
    return save_sims_time_index_;
}

bool AnEnIS::operation() const {
    return operation_;
}

bool AnEnIS::quick_sort() const {
    return quick_sort_;
}

bool AnEnIS::prevent_search_future() const {
    return prevent_search_future_;
}

const vector<double>& AnEnIS::weights() const {
    return weights_;
}

const Array4DPointer &
AnEnIS::sds() const {
    return sds_;
}

const Array4DPointer &
AnEnIS::sims_metric() const {
    if (!save_sims_) throw runtime_error(
            "Similarity array is not saved. Please change your configuration (save_sims)");
    return sims_metric_;
}

const Array4DPointer &
AnEnIS::sims_time_index() const {
    if (!save_sims_time_index_) throw runtime_error(
            "Similarity times index array is not saved. Please change your configuration (save_sims_time_index)");

    return sims_time_index_;
}

const Array4DPointer &
AnEnIS::analogs_value() const {
    if (!save_analogs_) throw runtime_error(
            "Analog array is not saved. Please change your configuration (save_analogs)");

    return analogs_value_;
}

const Array4DPointer &
AnEnIS::analogs_time_index() const {
    if (!save_analogs_time_index_) throw runtime_error(
            "Analog times index array is not saved. Please change your configuration (save_analogs_time_index)");

    return analogs_time_index_;
}

const Functions::Matrix &
AnEnIS::obs_time_index_table() const {
    return obs_time_index_table_;
}

void
AnEnIS::preprocess_(const Forecasts & forecasts,
        const Observations & observations,
        vector<size_t> & fcsts_test_index,
        vector<size_t> & fcsts_search_index) {

    checkIndexRange_(forecasts, fcsts_test_index, fcsts_search_index);
    checkConsistency_(forecasts, observations);
    checkSave_();
    
    /*
     * Adjust weights if they are not initialized
     */
    size_t num_parameters = forecasts.getParameters().size();
    if (weights_.empty()) weights_.resize(num_parameters, 1);
    else if (weights_.size() != num_parameters) throw runtime_error("Incorrect number of weights");

    /*
     * Compute standard deviations
     */
    computeSds_(forecasts, fcsts_search_index, fcsts_test_index);

    /*
     * If operational mode is used, append test time indices to the end of
     * the search time indices.
     */
    if (operation_) {
        fcsts_search_index.insert(fcsts_search_index.end(),
                fcsts_test_index.begin(), fcsts_test_index.end());
    }

    // Check for ascending order
    if (!is_sorted(fcsts_search_index.begin(), fcsts_search_index.end())) {
        stringstream msg;
        msg << "Forecast search times should be sorted in ascending order";
        if (operation_) msg << ". You are using the operational mode."
            << " Please make sure search and test periods do not overlap.";
        throw runtime_error(msg.str());
    }

    /*
     * Compute the index mapping from forecast times and lead times to 
     * observations times
     */
    const auto & fcst_times = forecasts.getTimes();
    const auto & obs_times = observations.getTimes();
    const auto & fcst_flts = forecasts.getFLTs();

    obs_time_index_table_ = Functions::Matrix(
            fcsts_search_index.size(), fcst_flts.size(), NAN);
    Functions::updateTimeTable(fcst_times,
            fcsts_search_index, fcst_flts, obs_times, obs_time_index_table_);

    /*
     * Pre-allocate memory for analog computation
     */
    allocateMemory_(forecasts, fcsts_test_index, fcsts_search_index);

    return;
}

void
AnEnIS::allocateMemory_(const Forecasts & forecasts,
        const vector<size_t> & fcsts_test_index, const vector<size_t> & fcsts_search_index) {

    if (verbose_ >= Verbose::Progress) cout << "Allocating memory ..." << endl;

    size_t num_stations = forecasts.getStations().size();
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

    return;
}

bool
AnEnIS::_simsSort_(const array<double, 3> & lhs,
        const array<double, 3> & rhs) {
    if (std::isnan(lhs[_SIM_VALUE_INDEX])) return false;
    if (std::isnan(rhs[_SIM_VALUE_INDEX])) return true;
    return (lhs[_SIM_VALUE_INDEX] < rhs[_SIM_VALUE_INDEX]);
}

void
AnEnIS::setMembers_(const Config & config) {

    num_analogs_ = config.num_analogs;
    num_sims_ = config.num_sims;
    obs_var_index_ = config.obs_var_index;
    max_par_nan_ = config.max_par_nan;
    max_flt_nan_ = config.max_flt_nan;
    flt_radius_ = config.flt_radius;
    save_analogs_ = config.save_analogs;
    save_analogs_time_index_ = config.save_analogs_time_index;
    save_sims_ = config.save_sims;
    save_sims_time_index_ = config.save_sims_time_index;
    operation_ = config.operation;
    quick_sort_ = config.quick_sort;
    prevent_search_future_ = config.prevent_search_future;
    weights_ = config.weights;

    use_AI_ = false;
    return;
}

void
AnEnIS::setSdsTimeMap_(const vector<size_t> & times_accum_index) {

    size_t count = times_accum_index.size();

    if (count == 0) {
        throw runtime_error("Empty running indices during operational sd calculation");
    }

    for (size_t i = 0; i < count; ++i) {

        /* 
         * The key is the forecast test time index and the the value is the
         * corresponding index of the time dimension in the standard deviation array.
         */
        sds_time_index_map_[times_accum_index[i]] = i;
    }

    return;
}

double
AnEnIS::computeSimMetric_(const Forecasts & forecasts,
        size_t sta_search_i, size_t sta_test_i,
        size_t flt_i, size_t time_test_i, size_t time_search_i,
        const vector<bool> & circulars) {

    /*
     * Prepare for similarity metric calculation
     */
    double sim = 0, sd = 0;
    size_t count_par_nan = 0;
    size_t num_parameters = forecasts.getParameters().size();
    size_t num_flts = forecasts.getFLTs().size();
    size_t flt_i_start = (flt_i <= flt_radius_ ? 0 : flt_i - flt_radius_);
    size_t flt_i_end = (flt_i + flt_radius_ >= num_flts ? num_flts - 1 : flt_i + flt_radius_);
    vector<double> window(flt_i_end - flt_i_start + 1);

    /*
     * Calculate similarity metric
     */
    for (size_t parameter_i = 0; parameter_i < num_parameters; parameter_i++) {

        // Skip the iteration if the weight is 0
        if (weights_[parameter_i] == 0) continue;

        // Get standard deviation for this parameter
        if (operation_) {
            sd = sds_.getValue(parameter_i, sta_search_i, flt_i, sds_time_index_map_[time_test_i]);
        } else {
            sd = sds_.getValue(parameter_i, sta_search_i, flt_i, 0);
        }

        // Skip the iteration if there is no variation in this parameter
        if (sd == 0) continue;

        for (size_t pos = 0, window_i = flt_i_start; window_i <= flt_i_end; ++window_i, ++pos) {

            double value_search = forecasts.getValue(parameter_i, sta_search_i, time_search_i, window_i);
            double value_test = forecasts.getValue(parameter_i, sta_test_i, time_test_i, window_i);

            if (std::isnan(value_search) || std::isnan(value_test)) {
                window[pos] = NAN;
            } else {
                if (circulars[parameter_i]) {
                    window[pos] = pow(Functions::diffCircular(value_search, value_test), 2);
                } else {
                    window[pos] = pow(value_search - value_test, 2);
                }
            }
        } // End loop of the lead time window

        double average = Functions::sum(window, max_flt_nan_);

        if (std::isnan(average)) {
            ++count_par_nan;
            if (count_par_nan > max_par_nan_) return NAN;
        } else {

            sim += weights_[parameter_i] * (sqrt(average) / sd);
        }

    } // End loop of parameters

    return sim;
}

void
AnEnIS::allocateSds_(const Forecasts & forecasts,
        const vector<size_t> & times_fixed_index,
        const vector<size_t> & times_accum_index) {

    size_t num_times = (operation_ ? times_accum_index.size() : _SINGLE_LEN);
    size_t num_parameters = forecasts.getParameters().size();
    size_t num_stations = forecasts.getStations().size();
    size_t num_flts = forecasts.getFLTs().size();

    // Pre-allocate memory for calculation
    sds_.resize(num_parameters, num_stations, num_flts, num_times);
    sds_.initialize(NAN);

    return;
}

void
AnEnIS::computeSds_(const Forecasts & forecasts,
        const vector<size_t> & times_fixed_index,
        const vector<size_t> & times_accum_index) {

    if (verbose_ >= Verbose::Detail) {
        cout << "Computing standard deviation ..." << endl;
    }

    allocateSds_(forecasts, times_fixed_index, times_accum_index);

    size_t num_parameters = sds_.shape()[0];
    size_t num_stations = sds_.shape()[1];
    size_t num_flts = sds_.shape()[2];
    size_t num_times = sds_.shape()[3];

    size_t calculator_capacity = times_fixed_index.size();

    if (operation_) {
        calculator_capacity += times_accum_index.size();
        setSdsTimeMap_(times_accum_index);
    }

    vector<bool> circulars;
    forecasts.getParameters().getCirculars(circulars);

#if defined(_OPENMP)
#pragma omp parallel for default(none) schedule(dynamic) collapse(3) \
shared(num_parameters, num_stations, num_flts, forecasts, times_fixed_index, \
times_accum_index, circulars, num_times, calculator_capacity)
#endif
    for (size_t par_i = 0; par_i < num_parameters; ++par_i) {
        for (size_t sta_i = 0; sta_i < num_stations; ++sta_i) {
            for (size_t flt_i = 0; flt_i < num_flts; ++flt_i) {

                // Skip the iteration if the weight is 0
                if (weights_[par_i] == 0) continue;

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
                sds_.setValue(calc.sd(), par_i, sta_i, flt_i, 0);

                if (operation_) {
                    for (size_t time_i = 1; time_i < num_times; ++time_i) {

                        // Get the forecast value
                        value = forecasts.getValue(par_i, sta_i, times_accum_index[time_i - 1], flt_i);

                        if (std::isnan(value)) {
                            // Copy the value from previous iteration if the value is NAN
                            sds_.setValue(
                                    sds_.getValue(par_i, sta_i, flt_i, time_i - 1),
                                    par_i, sta_i, flt_i, time_i);

                        } else {
                            // Push the valid value into the calculator
                            calc.pushValue(value);

                            // Update the running standard deviation
                            sds_.setValue(calc.sd(), par_i, sta_i, flt_i, time_i);
                        }
                    } // End of loop of accumulated time indices
                }
            } // End of loop of FLTs
        } // End of loop of stations
    } // End of loop of parameters

    return;
}

void
AnEnIS::checkIndexRange_(const Forecasts & forecasts,
        const vector<size_t> & fcsts_test_index,
        const vector<size_t> & fcsts_search_index) const {
    
    if (fcsts_test_index.size() == 0) throw runtime_error("No test indices provided. Do forecasts actually cover the test period?");
    if (fcsts_search_index.size() == 0) throw runtime_error("No search indices provided. Do forecasts actually cover the search period?");

    size_t num_times = forecasts.getTimes().size();

    auto max_it = max_element(fcsts_test_index.begin(), fcsts_test_index.end());
    if (*max_it >= num_times) {
        ostringstream msg;
        msg << "Forecast test time index maximum (" << *max_it
                << ") exceeds the number of times (" << num_times << ")";
        throw range_error(msg.str());
    }

    max_it = max_element(fcsts_search_index.begin(), fcsts_search_index.end());
    if (*max_it >= num_times) {

        ostringstream msg;
        msg << "Forecast search time index maximum (" << *max_it
                << ") exceeds the number of times (" << num_times << ")";
        throw range_error(msg.str());
    }

    return;
}

void
AnEnIS::checkConsistency_(const Forecasts & forecasts,
        const Observations & observations) const {

    if (forecasts.getStations().size() != observations.getStations().size()) {
        ostringstream msg;
        msg << "#forecast stations (" << forecasts.getStations().size()
                << ") != #observation stations ("
                << observations.getStations().size() << ")";
        throw runtime_error(msg.str());
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
AnEnIS::checkSave_() const {

    if (save_analogs_ || save_analogs_time_index_ || save_sims_ || save_sims_time_index_) {
        // This is correct because at least one of them should be true
    } else {
        throw runtime_error("Neither analogs nor similarity are saved. Please check your configuration.");
    }

    return;
}

void
AnEnIS::checkNumberOfMembers_(size_t num_search_times_index) {

    if (num_sims_ > num_search_times_index) {
        num_sims_ = num_search_times_index;

        if (verbose_ >= Verbose::Warning) {
            cerr << "Warning: The number of similarity has been changed to "
                    << num_sims_ << " due to the too short search period." << endl;
        }
    }

    if (num_analogs_ > num_search_times_index) {
        num_analogs_ = num_search_times_index;

        if (verbose_ >= Verbose::Warning) {
            cerr << "Warning: The number of analogs has been changed to "
                    << num_analogs_ << " due to the too short search period." << endl;
        }
    }

    return;
}

#if defined(_ENABLE_AI)
void 
AnEnIS::load_similarity_model(const std::string & similarity_model_path) {
    
    if (verbose_ >= Verbose::Progress) cout << "Reading the similarity model ..." << endl;

    try {
        similarity_model_ = torch::jit::load(similarity_model_path);
        similarity_model_.eval();

    } catch (const c10::Error& e) {
        string msg = string("Failed to read the similarity model ") + similarity_model_path;
        msg += string("\n\n Original error message: ") + e.what();
        throw runtime_error(msg);
    }

    use_AI_ = true;

    return;
}

double
AnEnIS::computeSimMetricAI_(const Forecasts & forecasts,
        size_t sta_search_i, size_t sta_test_i,
        size_t flt_i, size_t time_test_i, size_t time_search_i) {

    long int num_features = forecasts.getParameters().size();
    auto x1 = at::full({1, num_features}, NAN, at::kFloat);
    auto x2 = at::full({1, num_features}, NAN, at::kFloat);

    for (long int feature_i = 0; feature_i < num_features; ++feature_i) {
        x1[0][feature_i] = forecasts.getValue(feature_i, sta_test_i, time_test_i, flt_i);
        x2[0][feature_i] = forecasts.getValue(feature_i, sta_search_i, time_search_i, flt_i);
    }

    std::vector<torch::jit::IValue> inputs = {x1, x2};
    at::Tensor output = similarity_model_.forward(inputs).toTensor();

    return output[0][0].item<double>();
}
#endif

