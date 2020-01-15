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

// This is the best estimator that Yamartino has found
//              2 / sqrt(3) - 1 = 0.1547
//
static const double _YAMARTINO = 0.1547;

void
Functions::updateTimeTable(
        const Times & fcst_times, const vector<size_t> & fcst_times_index,
        const Times & fcst_flts, const Times & obs_times, Matrix & table) {

    // Check the dimensions of input table
    if (table.size1() != fcst_times_index.size() ||
            table.size2() != fcst_flts.size()) {
        ostringstream msg;
        msg << BOLDRED << "Table dimensions [" << table.size1() << ","
                << table.size2() << "] should be ["
                << fcst_times_index.size() << "," << fcst_flts.size()
                << "]" << RESET;
        throw overflow_error(msg.str());
    }

    // Define dimension variables so that nested parallel for-loops can be
    // perfectly collapsed (with Intel compilers)
    //
    auto rows = table.size1(), cols = table.size2();

#if defined(_OPENMP)
#pragma omp parallel for default(none) schedule(static) collapse(2) \
shared(table, obs_times, fcst_times, fcst_flts, rows, cols, fcst_times_index)
#endif
    for (size_t row_i = 0; row_i < rows; ++row_i) {
        for (size_t col_i = 0; col_i < cols; ++col_i) {

            try {
                table(row_i, col_i) = obs_times.getIndex(
                        fcst_times.left[fcst_times_index[row_i]].second +
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

void
Functions::meanCircularDecomp(const vector<double> & degs,
        double & s, double & c) {

    vector<double> sins(degs.size());
    vector<double> coss(degs.size());

    for (size_t i = 0; i < degs.size(); i++) {

        // This is to convert from degrees to radians
        //
        double rad = degs[i] * _DEG2RAD;

        sins[i] = sin(rad);
        coss[i] = cos(rad);
    }

    s = mean(sins);
    c = mean(coss);
    return;
}

double
Functions::sdYamartino(const double & s, const double & c) {
    // Yamartino estimator
    double e = sqrt(1.0 - (pow(s, 2.0) + pow(c, 2.0)));
    double asine = asin(e);
    double ex3 = pow(e, 3);
    double sd = asine * (1 + _YAMARTINO * ex3);
    return (sd * _RAD2DEG);
}

double
Functions::sdCircular(const vector<double> & degs) {
    double s, c;
    meanCircularDecomp(degs, s, c);
    return (sdYamartino(s, c));
}

double
Functions::mean(const vector<double> & values, size_t max_nan_allowed) {
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
    return (variance(values, average));
}

double
Functions::variance(const vector<double> & values, const double & average) {
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
