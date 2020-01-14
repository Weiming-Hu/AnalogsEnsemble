/* 
 * File:   AnEnIS.cpp
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <cervone@psu.edu>
 * 
 * Created on January 8, 2020, 11:40 AM
 */

#include "AnEnIS.h"
#include "colorTexts.h"

#include <algorithm>

using namespace std;
using namespace AnEnDefaults;

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
    return(lhs[_SIM_VALUE_INDEX] < rhs[_SIM_VALUE_INDEX]);
}

AnEnIS::AnEnIS() : num_analogs_(1), obs_var_index_(0), quick_sort_(true),
save_sims_index_(false), save_analogs_index_(false), num_sims_(0),
max_par_nan_(0), max_flt_nan_(0), flt_radius_(1) {
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
        bool time_overlap_check,
        bool save_sims,
        Verbose verbose,
        size_t obs_var_index,
        bool quick_sort,
        bool save_sims_index,
        bool save_analogs_index,
        size_t num_sims,
        double max_par_nan,
        double max_flt_nan,
        size_t flt_radius) :
AnEn(operational, time_overlap_check, save_sims, verbose),
num_analogs_(num_members), obs_var_index_(obs_var_index),
quick_sort_(quick_sort), save_sims_index_(save_sims_index),
save_analogs_index_(save_analogs_index), num_sims_(num_sims),
max_par_nan_(max_par_nan), max_flt_nan_(max_flt_nan), flt_radius_(flt_radius) {
}

AnEnIS::~AnEnIS() {
}

void
AnEnIS::compute(const Forecasts & forecasts,
        const Observations & observations,
        const Times & test_times, const Times & search_times) {

    if (verbose_ >= Verbose::Progress) cout << GREEN
            << "Start AnEnIS generation ..." << RESET << endl;

    const auto & fcst_times = forecasts.getTimes(),
            obs_times = observations.getTimes();

    /*
     * Convert Time objects to their corresponding indices
     */
    vector<size_t> fcsts_test_index, fcsts_search_index;
    Functions::toIndex(fcsts_test_index, test_times, fcst_times);
    Functions::toIndex(fcsts_search_index, search_times, fcst_times);
    
    /*
     * If operational mode is used, append test time indices to the end of
     * the search time indices.
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
    }
    
    compute(forecasts, observations, fcsts_test_index, fcsts_search_index);
    return;
}

void
AnEnIS::compute(const Forecasts & forecasts,
        const Observations & observations,
        const vector<size_t> & fcsts_test_index,
        const vector<size_t> & fcsts_search_index) {

    const auto & fcst_times = forecasts.getTimes(),
            obs_times = observations.getTimes();
    const auto & fcst_flts = forecasts.getFLTs();

    /*
     * Compute the index mapping from forecast times and lead times to 
     * observations times
     */
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
     * Compute standard deviations
     */
//    computeSds_(forecasts, fcsts_search_index, search_times.size());

    /*
     * Pre-allocate memory for analog computation
     */
    if (verbose_ >= Verbose::Detail) cout << GREEN
            << "Allocating memory ..." << RESET << endl;
    
    size_t num_stations = forecasts.getStations().size(),
            num_flts = forecasts.getFLTs().size(),
            num_test_times_index = fcsts_test_index.size(),
            num_search_times_index = fcsts_search_index.size();
    
    analogsValue_.resize(boost::extents
            [num_stations][num_test_times_index][num_flts][num_analogs_]);
    if (save_analogs_index_) analogsIndex_.resize(boost::extents
            [num_stations][num_test_times_index][num_flts][num_analogs_]);
    
    if (num_sims_ < num_analogs_) num_sims_ = num_analogs_;
    if (save_sims_) simsMetric_.resize(boost::extents
            [num_stations][num_test_times_index][num_flts][num_sims_]);
    if (save_sims_index_) simsIndex_.resize(boost::extents
            [num_stations][num_test_times_index][num_flts][num_sims_]);

    /*
     * Progress messages output
     */
    if (verbose_ >= Verbose::Detail) print(cout);

    /*
     * Analog computation
     */
    if (verbose_ >= Verbose::Detail) cout << GREEN
            << "Computing analogs ..." << RESET << endl;

    for (size_t sta_i = 0; sta_i < num_stations; ++sta_i) {
        for (size_t flt_i = 0; flt_i < num_flts; ++flt_i) {
            for (size_t time_test_i = 0; time_test_i < num_test_times_index; ++time_test_i) {
                
                /*
                 * Compute similarity for all search times
                 */
                simsArr_.resize(num_search_times_index, _INIT_ARR_VALUE);
                for (size_t time_search_i = 0; time_search_i < num_search_times_index; ++time_search_i) {
                    
                    if (check_time_overlap_) {
                        /*
                         * Check whether to search this time when overlapping time is prohibited
                         */
                        if (fcst_times.left[fcsts_search_index[time_search_i]].second.timestamp +
                                fcst_flts.left[flt_i].second.timestamp >=
                                fcst_times.left[fcsts_test_index[time_test_i]].second.timestamp) continue;
                    }
                    
                    if (operational_) {
                        /*
                         * Check whether to search this time in operational mode
                         */
                        if (fcsts_search_index[time_search_i] >=
                                fcsts_test_index[time_test_i]) continue;
                    }
                    
                    double obs_time_index = obsIndexTable_(time_search_i, flt_i);
                    if (std::isnan(obs_time_index)) continue;
                    
                    double obs = observations.getValue(obs_var_index_, sta_i, obs_time_index);
                    if (std::isnan(obs)) continue;

                    simsArr_[time_search_i][_SIM_VALUE_INDEX] = computeSim_(
                            forecasts, sta_i, flt_i, time_test_i,
                            time_search_i, weights, circulars);
                    simsArr_[time_search_i][_SIM_FCST_INDEX] =
                            fcsts_search_index[time_search_i];
                    simsArr_[time_search_i][_SIM_OBS_INDEX] = obs_time_index;
                }
                
                /*
                 * Sort similarity metrics
                 */
                if (quick_sort_) nth_element(simsArr_.begin(),
                        simsArr_.begin() + num_sims_, simsArr_.end(), simsSort);
                else partial_sort(simsArr_.begin(),
                        simsArr_.begin() + num_sims_, simsArr_.end(), simsSort);
                
                /*
                 * Output values and indices
                 */
                for (size_t analog_i = 0; analog_i < num_analogs_; ++analog_i) {
                    analogsValue_[sta_i][time_test_i][flt_i][analog_i] = observations.getValue(
                            obs_var_index_, sta_i, simsArr_[analog_i][_SIM_OBS_INDEX]);

                    if (save_analogs_index_) analogsIndex_[sta_i][time_test_i][flt_i][analog_i] = 
                            simsArr_[analog_i][_SIM_OBS_INDEX];
                    
                    // TODO: mpi_send();
                }

                if (save_sims_index_ || save_sims_) {
                    for (size_t sim_i = 0; sim_i < num_sims_; ++sim_i) {
                        if (save_sims_) simsMetric_[sta_i][time_test_i][flt_i][sim_i] =
                                simsArr_[sim_i][_SIM_VALUE_INDEX];
                        if (save_sims_index_) simsIndex_[sta_i][time_test_i][flt_i][sim_i] =
                                simsArr_[sim_i][_SIM_FCST_INDEX];
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
    return (simsMetric_);    
}

const Array4D &
AnEnIS::getSimsIndex() const {
    return (simsIndex_);        
}

const Array4D &
AnEnIS::getAnalogsValue() const {
    return (analogsValue_);    
}

const Array4D &
AnEnIS::getAnalogsIndex() const {
    return (analogsIndex_);    
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
            << "check time overlap: " << check_time_overlap_ << endl
            << "save analog time indices: " << save_analogs_index_ << endl
            << "save similarity: " << save_sims_ << endl
            << "save similarity time indices: " << save_sims_index_ << endl;

    if (verbose_ >= Verbose::Debug) {
        os << "sds_ dimensions: ["
                << Functions::format(sds_.shape(), sds_.num_dimensions())
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
AnEnIS::computeSim_(const Forecasts & forecasts,
        size_t sta_i, double flt_i, size_t time_test_i, size_t time_search_i,
        const vector<double> & weights, const vector<bool> & circulars) {
    
    /*
     * Prepare for similarity metric calculation
     */
    double sim = 0, sd = 0;
    size_t count_par_nan = 0,
            num_pars = forecasts.getParameters().size(),
            num_flts = forecasts.getFLTs().size(),
            flt_i_start = (flt_i - flt_radius_ < 0 ? 0 : flt_i - flt_radius_),
            flt_i_end = (flt_i + flt_radius_ == num_flts ?
                num_flts - 1 : flt_i + flt_radius_);

    /*
     * Calculate similarity metric
     */
    for (size_t par_i = 0; par_i < num_pars; par_i++) {

        if (weights[par_i] != 0) {
            vector<double> window(flt_i_end - flt_i_start + 1);
            size_t pos = 0;

            for (size_t window_i = flt_i_start; window_i <= flt_i_end;
                    ++window_i, ++pos) {

                double value_search = forecasts.getValue(
                        par_i, sta_i, time_search_i, window_i);
                double value_test = forecasts.getValue(
                        par_i, sta_i, time_test_i, window_i);

                if (std::isnan(value_search) || std::isnan(value_test)) {
                    window[pos] = NAN;
                } else {
                    if (circulars[par_i]) {
                        window[pos] = pow(Functions::diffCircular(
                                value_search, value_test), 2);
                    } else {
                        window[pos] = pow(value_search - value_test, 2);
                    }    
                }
            } // End loop of the lead time window

            double average = Functions::mean(window, max_flt_nan_);

            if (std::isnan(average)) {
                ++count_par_nan;
                if (count_par_nan > max_par_nan_) {
                    sim = NAN;
                    break;
                }
            } else {
                if (operational_) sd = sds_[time_test_i][par_i][sta_i][flt_i];
                else sd = sds_[0][par_i][sta_i][flt_i];
                sim += weights[par_i] * (sqrt(average) / sd);
            }
            
        } // End of check of the the parameter weight
    } // End loop of parameters
    
    
    
    return 0.0;
}

void
AnEnIS::computeSds_(const Forecasts & forecasts,
        const vector<size_t> & times_index, size_t running_pos_start) {
    
    if (verbose_ >= Verbose::Detail) {
        cout << "Computing standard deviation ..." << RESET << endl;
    }
    
    size_t num_times = _SINGLE_LEN,
            num_times_fixed = times_index.size(),
            num_parameters = forecasts.getParameters().size(),
            num_stations = forecasts.getStations().size(),
            num_flts = forecasts.getFLTs().size();

    if (operational_) {
        num_times_fixed = running_pos_start;
        num_times = times_index.size() - running_pos_start;
    }

    sds_.resize(boost::extents
            [num_times][num_parameters][num_stations][num_flts]);

#if defined(_OPENMP)
#pragma omp parallel for default(none) schedule(dynamic) collapse(4) \
shared(num_times_fixed, num_times, num_parameters, num_stations, num_flts)
//circulars_, weights_, sds_, operational_, forecasts, times_index)
#endif
    for (size_t time_i = 0; time_i < num_times; ++time_i) {
        for (size_t par_i = 0; par_i < num_parameters; ++par_i) {
            for (size_t sta_i = 0; sta_i < num_stations; ++sta_i) {
                for (size_t flt_i = 0; flt_i < num_flts; ++flt_i) {

//                    if (weights_[par_i] != 0) {
//                            computeSd_(forecasts, times_index, time_i, par_i,
//                                sta_i, flt_i, num_times_fixed + time_i);
//                    }

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

//    if (circulars_[par_i]) {
        sds_[time_i][par_i][sta_i][flt_i] = Functions::sdCircular(values);
//    } else {
        sds_[time_i][par_i][sta_i][flt_i] = Functions::sdLinear(values);
//    }
    
    if (sds_[time_i][par_i][sta_i][flt_i] == 0) {
        ostringstream msg;
        msg << BOLDRED << "Forecast parameter #" << par_i
                << "has 0 standard deviation" << RESET << endl;
        throw runtime_error(msg.str());
    }
    
    return;
}