/* 
 * File:   Functions.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 * 
 * Created on February 6, 2019, 2:58 PM
 */

#include "Functions.h"
#include "colorTexts.h"

#include <cmath>
#include <sstream>
#include <stdexcept>

#if defined(_OPENMP)
#include <omp.h>
#endif

using namespace std;

static const double _DEG2RAD = M_PI / 180;
static const double _RAD2DEG = 180 / M_PI;

void
Functions::updateTimeTable(
        const Times & fcst_times, const Times & fcst_flts,
        const Times & obs_times, Matrix & table, size_t start_row_i) {

    // Check the dimensions of input table
    if (table.size1() < start_row_i + fcst_times.size() ||
            table.size2() < fcst_flts.size()) {
        ostringstream msg;
        msg << BOLDRED << "Not enough space in table["<< table.size1() << "," <<
                table.size2() << "] to store " << fcst_times.size() << 
                " rows and " << fcst_flts.size() <<
                " columns starting at row #" << start_row_i << RESET;
        throw overflow_error(msg.str());
    }

    // Define dimension variables so that nested parallel for-loops can be
    // perfectly collapsed (with Intel compilers)
    //
    auto rows = fcst_times.size(), cols = fcst_flts.size();

#if defined(_OPENMP)
#pragma omp parallel for default(none) schedule(static) collapse(2) \
shared(table, obs_times, fcst_times, fcst_flts, rows, cols, start_row_i)
#endif
    for (size_t row_i = 0; row_i < rows; ++row_i) {
        for (size_t col_i = 0; col_i < cols; ++col_i) {

            try {
                table(row_i + start_row_i, col_i) = obs_times.getIndex(
                        fcst_times.left[row_i].second +
                        fcst_flts.left[col_i].second);
            } catch (range_error & e) {
                // If range_error is thrown because the time cannot be found,
                // nothing is done and the index value in the table simply
                // remain intact.
                //
            }
        }
    }

    return;
}

double
Functions::sdLinear(const vector<double> & values) {
    return (sqrt(variance(values)));
}

double
Functions::sdCircular(const vector<double> & values) {

    vector<double> sins(values.size());
    vector<double> coss(values.size());

    for (unsigned int i = 0; i < values.size(); i++) {

        // This is to convert from degrees to radians
        //
        double rad = values[i] * _DEG2RAD;

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
    return (q * _RAD2DEG);
}

double
Functions::mean(const vector<double> & values, double max_nan_allowed) {
    double sum = 0.0;
    size_t nan_count = 0, vec_size = values.size();

    for (const auto & value : values) {
        if (std::isnan(value)) {
            nan_count++;
        } else {
            sum += value;
        }
    }

    if (nan_count > max_nan_allowed) return NAN;
    if (nan_count == vec_size) return NAN;

    return (sum / (double) (vec_size - nan_count));
}

double
Functions::variance(const vector<double> & values) {
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
Functions::diffCircular(double i, double j) {
    double res1 = abs(i - j);
    double res2 = abs(res1 - 360);
    return (min(res1, res2));
}

//
//const double MULTIPLY = M_PI / 180;
//const double MULTIPLY_REVERSE = 180 / M_PI;
//const double FILL_VALUE = NAN;
//
//Functions::Functions(int verbose) : verbose_(verbose) {
//}
//
////Functions::Functions(const Functions& orig) {
////}
//
//Functions::~Functions() {
//}
//
//errorType
//Functions::computeStandardDeviation(
//        const Forecasts_array& forecasts, StandardDeviation& sds,
//        vector<size_t> i_times) const {
//
//    if (verbose_ >= 3) cout << "Computing standard deviation ... " << endl;
//    
//    if (i_times.size() == 0) {
//        i_times.resize(forecasts.getTimes().size());
//        iota(i_times.begin(), i_times.end(), 0);
//    }
//
//    size_t num_parameters = forecasts.getParameters().size();
//    size_t num_stations = forecasts.getStations().size();
//    size_t num_times = i_times.size();
//    size_t num_flts = forecasts.getFLTs().size();
//
////    auto & array = forecasts;
//
//    vector<bool> circular_flags(num_parameters, false);
//    auto & parameters_by_insert = forecasts.getParameters().get<anenPar::by_insert>();
//    for (size_t i_parameter = 0; i_parameter < num_parameters; i_parameter++) {
//        circular_flags[i_parameter] = parameters_by_insert[i_parameter].getCircular();
//    }
//
//    // Resize sds according to forecasts
//    StandardDeviation::extent_gen extents;
//    sds.resize(extents[num_parameters][num_stations][num_flts]);
//
//#if defined(_OPENMP)
//#pragma omp parallel for default(none) schedule(static) collapse(3) \
//shared(num_parameters, num_stations, num_flts, num_times, forecasts, \
//i_times, circular_flags, sds) 
//#endif
//    for (size_t i_parameter = 0; i_parameter < num_parameters; i_parameter++) {
//        for (size_t i_station = 0; i_station < num_stations; i_station++) {
//            for (size_t i_flt = 0; i_flt < num_flts; i_flt++) {
//
//                // Extract values for times
//                vector<double> values(num_times);
//                for (size_t pos_time = 0; pos_time < num_times; pos_time++) {
//                    size_t i_time = i_times[pos_time];
//                    values[pos_time] = forecasts.getValue(i_parameter, i_station, i_time, i_flt);
//                } // End of times loop
//
//                if (circular_flags[i_parameter]) {
//                    // If the parameter is circular
//                    sds[i_parameter][i_station][i_flt] = sdCircular(values);
//                } else {
//                    // If the parameter is not circular
//                    sds[i_parameter][i_station][i_flt] = sdLinear(values);
//                } // End of if statement of is_circular
//
//            } // End of FLTs loop
//        } // End of stations loop
//    } // End of parameters loop
//
//    return (SUCCESS);
//}
//
//errorType
//Functions::computeSearchWindows(boost::numeric::ublas::matrix<size_t> & windows,
//        size_t num_flts, size_t window_half_size) const {
//
//    if (verbose_ >= 3) cout << "Computing search windows for FLT ... " << endl;
//
//    windows.resize(num_flts, 2);
//
//    for (size_t i_flt = 0; i_flt < num_flts; i_flt++) {
//        int begin = 0;
//        size_t end = 0;
//
//        begin = i_flt - window_half_size;
//        end = i_flt + window_half_size;
//        windows(i_flt, 0) = (begin < 0) ? 0 : begin;
//        windows(i_flt, 1) = (end >= num_flts) ? (num_flts - 1) : end;
//    }
//
//    return (SUCCESS);
//}
//
//
//errorType
//Functions::convertToIndex(
//        const anenTime::Times & targets,
//        const anenTime::Times & container,
//        std::vector<size_t> & indexes) const {
//    
//    indexes.clear();
//    
//    const auto & targets_by_insert = targets.get<anenTime::by_insert>();
//    
//    for (const auto & e : targets_by_insert) {
//        indexes.push_back(container.getTimeIndex(e));
//    }
//    
//    return (SUCCESS);
//}
//

//
//void
//Functions::setVerbose(int verbose) {
//    verbose_ = verbose;
//}
//
//int
//Functions::getVerbose() {
//    return (verbose_);
//}
