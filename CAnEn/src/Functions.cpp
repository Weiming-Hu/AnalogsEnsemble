/* 
 * File:   Functions.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 * 
 * Created on February 6, 2019, 2:58 PM
 */

#include "Functions.h"
#include "colorTexts.h"

using namespace std;

#if defined(_OPENMP)
#include <omp.h>
#endif

const double MULTIPLY = M_PI / 180;
const double MULTIPLY_REVERSE = 180 / M_PI;
const double FILL_VALUE = NAN;

Functions::Functions(int verbose) : verbose_(verbose) {
}

//Functions::Functions(const Functions& orig) {
//}

Functions::~Functions() {
}

errorType
Functions::computeStandardDeviation(
        const Forecasts_array& forecasts, StandardDeviation& sds,
        vector<size_t> i_times) const {

    if (verbose_ >= 3) cout << "Computing standard deviation ... " << endl;
    
    if (i_times.size() == 0) {
        i_times.resize(forecasts.getTimes().size());
        iota(i_times.begin(), i_times.end(), 0);
    }

    size_t num_parameters = forecasts.getParameters().size();
    size_t num_stations = forecasts.getStations().size();
    size_t num_times = i_times.size();
    size_t num_flts = forecasts.getFLTs().size();

//    auto & array = forecasts;

    vector<bool> circular_flags(num_parameters, false);
    auto & parameters_by_insert = forecasts.getParameters().get<anenPar::by_insert>();
    for (size_t i_parameter = 0; i_parameter < num_parameters; i_parameter++) {
        circular_flags[i_parameter] = parameters_by_insert[i_parameter].getCircular();
    }

    // Resize sds according to forecasts
    StandardDeviation::extent_gen extents;
    sds.resize(extents[num_parameters][num_stations][num_flts]);

#if defined(_OPENMP)
#pragma omp parallel for default(none) schedule(static) collapse(3) \
shared(num_parameters, num_stations, num_flts, num_times, forecasts, \
i_times, circular_flags, sds) 
#endif
    for (size_t i_parameter = 0; i_parameter < num_parameters; i_parameter++) {
        for (size_t i_station = 0; i_station < num_stations; i_station++) {
            for (size_t i_flt = 0; i_flt < num_flts; i_flt++) {

                // Extract values for times
                vector<double> values(num_times);
                for (size_t pos_time = 0; pos_time < num_times; pos_time++) {
                    size_t i_time = i_times[pos_time];
                    values[pos_time] = forecasts.getValue(i_parameter, i_station, i_time, i_flt);
                } // End of times loop

                if (circular_flags[i_parameter]) {
                    // If the parameter is circular
                    sds[i_parameter][i_station][i_flt] = sdCircular(values);
                } else {
                    // If the parameter is not circular
                    sds[i_parameter][i_station][i_flt] = sdLinear(values);
                } // End of if statement of is_circular

            } // End of FLTs loop
        } // End of stations loop
    } // End of parameters loop

    return (SUCCESS);
}

errorType
Functions::computeSearchWindows(boost::numeric::ublas::matrix<size_t> & windows,
        size_t num_flts, size_t window_half_size) const {

    if (verbose_ >= 3) cout << "Computing search windows for FLT ... " << endl;

    windows.resize(num_flts, 2);

    for (size_t i_flt = 0; i_flt < num_flts; i_flt++) {
        int begin = 0;
        size_t end = 0;

        begin = i_flt - window_half_size;
        end = i_flt + window_half_size;
        windows(i_flt, 0) = (begin < 0) ? 0 : begin;
        windows(i_flt, 1) = (end >= num_flts) ? (num_flts - 1) : end;
    }

    return (SUCCESS);
}

errorType
Functions::computeObservationsTimeIndices(
        const anenTime::Times & times_forecasts,
        const anenTime::Times & flts_forecasts,
        const anenTime::Times & times_observations,
        Functions::TimeMapMatrix & mapping_ref,
        int time_match_mode) const {

    if (verbose_ >= 3) cout << "Computing mapping from forecast [Time, FLT] to observation [Time]  ... " << endl;

    Functions::TimeMapMatrix mapping(times_forecasts.size(), flts_forecasts.size(), FILL_VALUE);

    const auto & times_forecasts_by_insert = times_forecasts.get<anenTime::by_insert>();
    const auto & flts_forecasts_by_insert = flts_forecasts.get<anenTime::by_insert>();

    size_t index = 0;

    // OpenMP does not allow return status. So this flag is used to keep track
    // of return status of the loop
    //
    int loop_flag = 0;

    // Define vairables for perfectly nexted parallel loops with collapse
    auto limit_row = mapping.size1();
    auto limit_col = mapping.size2();

#if defined(_OPENMP)
#pragma omp parallel for default(none) schedule(static) collapse(2) reduction(max:loop_flag) \
shared(mapping, times_observations, times_forecasts_by_insert, flts_forecasts_by_insert, cout, \
limit_row, limit_col) firstprivate(index)
#endif
    for (size_t i_row = 0; i_row < limit_row; i_row++) {
        for (size_t i_col = 0; i_col < limit_col; i_col++) {

            try {
                index = times_observations.getTimeIndex(
                        times_forecasts_by_insert[i_row] + flts_forecasts_by_insert[i_col]);
                mapping(i_row, i_col) = index;
            } catch (...) {
                loop_flag = 1;
            }
        }
    }

    if (loop_flag > 0) {

        if (time_match_mode == 0) {
            if (verbose_ >= 1) cerr << BOLDRED
                    << "Error: Could not find some forecast times in observation times."
                    << " NA values might exist." << RESET << endl;
            return (OUT_OF_RANGE);
        } else {
            if (verbose_ >= 2) cerr << RED
                    << "Warning: Could not find some forecast times in observation times."
                    << " NA values might exist." << RESET << endl;
        }
    }

    swap(mapping_ref, mapping);
    return (SUCCESS);
}

errorType
Functions::convertToIndex(
        const anenTime::Times & targets,
        const anenTime::Times & container,
        std::vector<size_t> & indexes) const {
    
    indexes.clear();
    
    const auto & targets_by_insert = targets.get<anenTime::by_insert>();
    
    for (const auto & e : targets_by_insert) {
        indexes.push_back(container.getTimeIndex(e));
    }
    
    return (SUCCESS);
}

double
Functions::sdLinear(const vector<double>& values) const {
    return (sqrt(variance(values)));
}

double
Functions::sdCircular(const vector<double>& values) const {

    vector<double> sins(values.size());
    vector<double> coss(values.size());

    for (unsigned int i = 0; i < values.size(); i++) {

        // This is to convert from degrees to radians
        //
        double rad = values[i] * MULTIPLY;

        sins[i] = sin(rad);
        coss[i] = cos(rad);
    }

    double s = mean(sins);
    double c = mean(coss);

    // Yamartino estimator
    double e = sqrt(1.0 - (pow(s, 2.0) + pow(c, 2.0)));
    double asine = asin(e);
    double ex3 = pow(e, 3);

    // This is the best estimator that Yamartino has found
    //              2 / sqrt(3) - 1 = 0.1547
    //
    const double b = 0.1547;

    double q = asine * (1 + b * ex3);

    // Convert back to degrees
    return (q * MULTIPLY_REVERSE);
}

double
Functions::mean(const std::vector<double>& values, const double max_nan_allowed) const {
    double sum = 0.0;
    size_t nan_count = 0, vec_size = values.size();

    for (const auto & value : values) {
        if (std::isnan(value)) {
            nan_count++;
        } else {
            sum += value;
        }
    }

    if (!std::isnan(max_nan_allowed) && nan_count > max_nan_allowed) return NAN;
    if (nan_count == vec_size) return NAN;

    return (sum / (double) (vec_size - nan_count));
}

double
Functions::variance(const std::vector<double>& values) const {
    double average = mean(values);
    if (std::isnan(average)) return NAN;

    double sum = 0.0;
    size_t valid = 0;

    for (const auto & value : values) {
        if (!std::isnan(value)) {

            sum += pow(value - average, 2);
            valid++;
        }
    }

    return (sum / ((double) valid - 1.0));
}

double
Functions::diffCircular(double i, double j) const {
    double res1 = abs(i - j);
    double res2 = abs(res1 - 360);
    return (min(res1, res2));
}

void
Functions::setVerbose(int verbose) {
    verbose_ = verbose;
}

int
Functions::getVerbose() {
    return (verbose_);
}
