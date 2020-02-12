/* 
 * File:   Functions.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 * 
 * Created on February 6, 2019, 2:58 PM
 */

#include "Functions.h"

#include <string>
#include <cmath>
#include <limits>
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include <iterator>

#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/geometry/geometries/point.hpp"
#include "boost/geometry/geometries/ring.hpp"
#include "boost/geometry/index/rtree.hpp"
#include "boost/lambda/lambda.hpp"
#include "boost/geometry.hpp"

#if defined(_OPENMP)
#include <omp.h>
#endif


#include <ctime>

using namespace std;
namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

const double _DEG2RAD = M_PI / 180;
const double _RAD2DEG = 180 / M_PI;

void
Functions::setSearchStations(const Stations & stations, Matrix & table, double distance) {

    // Determine the number of neighbors
    size_t num_neighbors = table.size2();
    size_t num_stations = stations.size();

    // Convert distance to squred distance
    distance *= distance;

    // Check the rows of the table
    if (table.size1() != num_stations) {
        ostringstream msg;
        msg << "The table has " << table.size1() << " rows but there are"
                << num_stations << " test stations";
        throw overflow_error(msg.str());
    }

    // Initialize a vector to store the distances and the indices
    vector< pair<double, size_t> > distances_sq(num_stations);

    for (size_t test_i = 0; test_i < num_stations; ++test_i) {

        const Station & test = stations.getStation(test_i);

        // Calculate pair-wise station distances
        for (size_t search_i = 0; search_i < num_stations; ++search_i) {
            const Station & search = stations.getStation(search_i);

            distances_sq[search_i].first =
                    pow(test.getX() - search.getX(), 2) +
                    pow(test.getY() - search.getY(), 2);
            distances_sq[search_i].second = search_i;
        }

        // Sort based on distances
        nth_element(distances_sq.begin(), distances_sq.begin() + num_neighbors, distances_sq.end(),
                [](const pair<double, size_t> & lhs, const pair<double, size_t> & rhs) {
                    return lhs.first < rhs.first;
                });

        // Copy neighbor stations index to the output table
        size_t current_pos = 0;

        for (size_t neighbor_i = 0; neighbor_i < num_neighbors; ++neighbor_i) {
            if (distances_sq[neighbor_i].first > distance) continue;
            table(test_i, current_pos) = distances_sq[neighbor_i].second;
            ++current_pos;
        }
    }

    return;
}

Verbose
Functions::itov(int flag) {
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
Functions::vtoi(Verbose verbose) {
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

string
Functions::vtos(Verbose verbose) {
    switch (verbose) {
        case Verbose::Error:
            return "Error only";
        case Verbose::Warning:
            return "Error + Warning";
        case Verbose::Progress:
            return "Progress";
        case Verbose::Detail:
            return "Detail";
        case Verbose::Debug:
            return "Debug";
        default:
            throw runtime_error("Unknown verbose type");
    }
}

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

size_t
Functions::levenshtein(const string & str1, const string & str2,
        size_t w, size_t s, size_t a, size_t d) {

    /*
     * This function implements the Damerau-Levenshtein algorithm to
     * calculate a distance between strings.
     *
     * Basically, it says how many letters need to be swapped, substituted,
     * deleted from, or added to string1, at least, to get string2.
     *
     * The idea is to build a distance matrix for the substrings of both
     * strings.  To avoid a large space complexity, only the last three rows
     * are kept in memory (if swaps had the same or higher cost as one deletion
     * plus one insertion, only two rows would be needed).
     *
     * At any stage, "i + 1" denotes the length of the current substring of
     * string1 that the distance is calculated for.
     *
     * row2 holds the current row, row1 the previous row (i.e. for the substring
     * of string1 of length "i"), and row0 the row before that.
     *
     * In other words, at the start of the big loop, row2[j + 1] contains the
     * Damerau-Levenshtein distance between the substring of string1 of length
     * "i" and the substring of string2 of length "j + 1".
     *
     * All the big loop does is determine the partial minimum-cost paths.
     *
     * It does so by calculating the costs of the path ending in characters
     * i (in string1) and j (in string2), respectively, given that the last
     * operation is a substitution, a swap, a deletion, or an insertion.
     *
     * This implementation allows the costs to be weighted:
     *
     * - w (as in "sWap")
     * - s (as in "Substitution")
     * - a (for insertion, AKA "Add")
     * - d (as in "Deletion")
     *
     * Note that this algorithm calculates a distance _iff_ d == a.
     */

    auto len1 = str1.size(), len2 = str2.size();
    auto row0 = new size_t[len2 + 1],
            row1 = new size_t[len2 + 1],
            row2 = new size_t[len2 + 1];
    size_t i = 0, j = 0;

    for (; j <= len2; j++)
        row1[j] = j * a;
    for (; i < len1; i++) {

        row2[0] = (i + 1) * d;
        for (j = 0; j < len2; j++) {

            // substitution
            row2[j + 1] = row1[j] + s * (str1[i] != str2[j]);

            // swap
            if (i > 0 && j > 0 && str1[i - 1] == str2[j] &&
                    str1[i] == str2[j - 1] &&
                    row2[j + 1] > row0[j - 1] + w)
                row2[j + 1] = row0[j - 1] + w;

            // deletion
            if (row2[j + 1] > row1[j + 1] + d)
                row2[j + 1] = row1[j + 1] + d;

            // insertion
            if (row2[j + 1] > row2[j] + a)
                row2[j + 1] = row2[j] + a;
        }

        // swap arr
        auto dummy = row0;
        row0 = row1;
        row1 = row2;
        row2 = dummy;
    }

    i = row1[len2];
    delete [] row0;
    delete [] row1;
    delete [] row2;

    return i;
}

long
Functions::toSeconds(const string& datetime_str,
        const string& origin_str, bool iso_string) {
    
    using namespace boost::posix_time;
    
    ptime input_time, origin_time;
    
    // Origin time is supposed to be always standard format
    origin_time = time_from_string(origin_str);
    
    // Input Date time string can be either the standard format or the ISO format
    if (iso_string) {
        input_time = from_iso_string(datetime_str);
    } else {
        input_time = time_from_string(datetime_str);
    }
    
     // Calculate duration in seconds
    time_duration duration = input_time - origin_time;
    if (duration.is_negative()) throw runtime_error("Input time is earlier than the origin time");
    
    return duration.total_seconds();
}