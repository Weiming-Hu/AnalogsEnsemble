/* 
 * File:   Functions.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 * 
 * Created on February 6, 2019, 2:58 PM
 */

#include "Functions.h"

#include <cmath>
#include <sstream>
#include <stdexcept>

#if defined(_OPENMP)
#include <omp.h>
#endif

using namespace std;

AnEnDefaults::Verbose
Functions::itov(int flag) {

    using namespace AnEnDefaults;

    switch (flag) {
        case 0:
            return Verbose::Error;
        case 1:
            return Verbose::Warning;
        case 2:
            return Verbose::Progress;
        case 3:
            return Verbose::Detail;
        case 4:
            return Verbose::Debug;
        default:
            ostringstream msg;
            msg << "Unknown verbose flag " << flag;
            throw runtime_error(msg.str());
    }
}

int
Functions::vtoi(AnEnDefaults::Verbose verbose) {

    using namespace AnEnDefaults;

    switch (verbose) {
        case Verbose::Error:
            return 0;
        case Verbose::Warning:
            return 1;
        case Verbose::Progress:
            return 2;
        case Verbose::Detail:
            return 3;
        case Verbose::Debug:
            return 4;
        default:
            throw runtime_error("Unknown verbose type");
    }
}

// This is the best estimator that Yamartino has found
//              2 / sqrt(3) - 1 = 0.1547
//
static const double _YAMARTINO = 0.1547;
const double MULTIPLY = M_PI / 180;
const double MULTIPLY_REVERSE = 180 / M_PI;

void
Functions::updateTimeTable(
        const Times & fcst_times, const vector<size_t> & fcst_times_index,
        const Times & fcst_flts, const Times & obs_times, Matrix & table) {

    // Check the dimensions of input table
    if (table.size1() != fcst_times_index.size() ||
            table.size2() != fcst_flts.size()) {
        ostringstream msg;
        msg << "Table dimensions [" << table.size1() << "," << table.size2()
                << "] should be [" << fcst_times_index.size() << ","
                << fcst_flts.size() << "]";
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
                // If the time cannot be found, range_error will be generated.
                // But nothing is done for this case so that the index value
                // in the table simply remains intact.
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
Functions::sum(const vector<double> & values, size_t max_nan_allowed) {
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
    return sum;
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
