/* 
 * File:   Functions.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 * 
 * Created on February 6, 2019, 2:58 PM
 */

#include "Functions.h"

#include <cmath>
#include <string>
#include <limits>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <stdexcept>

#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/geometry/geometries/point.hpp"
#include "boost/geometry/geometries/ring.hpp"
#include "boost/geometry/index/rtree.hpp"
#include "boost/lambda/lambda.hpp"
#include "boost/geometry.hpp"

#if defined(_OPENMP)
#include <omp.h>
#endif

using namespace std;
namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

static const double _DEG2RAD = M_PI / 180;
static const double _RAD2DEG = 180 / M_PI;


/*
 * This definition is used to record the mapping from observation times to
 * forecast times and lead times. This is the reverse of the observation time
 * index table which goes from forecast times and lead times to observation times.
 */
using time_arr = array<size_t, 3>;

// These defines the value type on each position in the time array.
static const size_t _VALUE_INDEX = 0;
static const size_t _TIME_INDEX = 1;
static const size_t _FLT_INDEX = 2;


/*
 * This defines an operator for comparing observation time entry which is based
 * on the first position, time stamp value.
 */
struct time_arr_compare {

    bool operator()(const time_arr & lhs, const time_arr & rhs) const {
        return lhs[_VALUE_INDEX] < rhs[_VALUE_INDEX];
    }
};

void
Functions::createObsMap(unordered_map<string, size_t> & map,
        const vector<size_t> & id, const Parameters & parameters) {
    
    // Clear the map
    map.clear();
    
    // Insert pairs
    for (auto i : id) map.insert(make_pair(parameters.getParameter(i).getName(), i));
    
    return;
}

void
Functions::toValues(Array4D& analogs, size_t obs_id,
        const Array4D& analogs_time_index,
        const Observations& observations) {

    /*
     * Sanity check
     */
    if (obs_id >= observations.getParameters().size()) throw runtime_error("Observation ID exceeds the number of available observations");
    if (analogs_time_index.num_elements() == 0) throw runtime_error("Analogs time index is empty");


    /*
     * Allocate memory
     */
    analogs.resize(analogs_time_index);


    /*
     * Query values
     */
    const size_t* dims = analogs.shape();
    size_t num_stations = dims[0];
    size_t num_times = dims[1];
    size_t num_flts = dims[2];
    size_t num_members = dims[3];

#if defined(_OPENMP)
#pragma omp parallel for default(none) schedule(static) collapse(4) \
shared(num_stations, num_times, num_flts, num_members, analogs_time_index, obs_id, analogs, observations)
#endif
    for (size_t station_i = 0; station_i < num_stations; station_i++) {
        for (size_t time_i = 0; time_i < num_times; time_i++) {
            for (size_t flt_i = 0; flt_i < num_flts; flt_i++) {
                for (size_t member_i = 0; member_i < num_members; member_i++) {

                    double time_index = analogs_time_index.getValue(station_i, time_i, flt_i, member_i);
                    double value = NAN;

                    if (std::isnan(time_index)) {
                        // Skip if the time index is NAN
                    } else {
                        value = observations.getValue(obs_id, station_i, time_index);
                    }

                    // Assign the value
                    analogs.setValue(value, station_i, time_i, flt_i, member_i);
                }
            }
        }
    }

    return;
}

void
Functions::toValues(Array4D& analogs, size_t obs_id,
        const Array4D& analogs_time_index, const Array4D& analogs_station_index,
        const Observations& observations) {

    /*
     * Sanity check
     */
    if (obs_id >= observations.getParameters().size()) throw runtime_error("Observation ID exceeds the number of available observations");
    if (analogs_time_index.num_elements() == 0) throw runtime_error("Analogs time index is empty");
    if (analogs_station_index.num_elements() != analogs_time_index.num_elements()) throw runtime_error("#analogs station index != #analogs time index");


    /*
     * Allocate memory
     */
    analogs.resize(analogs_time_index);


    /*
     * Query values
     */
    const size_t* dims = analogs.shape();
    size_t num_stations = dims[0];
    size_t num_times = dims[1];
    size_t num_flts = dims[2];
    size_t num_members = dims[3];

#if defined(_OPENMP)
#pragma omp parallel for default(none) schedule(static) collapse(4) \
shared(num_stations, num_times, num_flts, num_members, analogs_time_index, \
obs_id, analogs, observations, analogs_station_index)
#endif
    for (size_t station_i = 0; station_i < num_stations; station_i++) {
        for (size_t time_i = 0; time_i < num_times; time_i++) {
            for (size_t flt_i = 0; flt_i < num_flts; flt_i++) {
                for (size_t member_i = 0; member_i < num_members; member_i++) {

                    double time_index = analogs_time_index.getValue(station_i, time_i, flt_i, member_i);
                    double station_index = analogs_station_index.getValue(station_i, time_i, flt_i, member_i);
                    double value = NAN;

                    if (std::isnan(time_index) || std::isnan(station_index)) {
                        // Skip if any of the index is NAN
                    } else {
                        value = observations.getValue(obs_id, station_index, time_index);
                    }

                    // Assign the value
                    analogs.setValue(value, station_i, time_i, flt_i, member_i);
                }
            }
        }
    }

    return;
}

void
Functions::setSearchStations(const Stations & stations, Matrix & table, double distance) {

    // Determine the number of neighbors
    size_t num_neighbors = table.size2();
    size_t num_stations = stations.size();

    if (num_neighbors > num_stations)
        throw runtime_error("Number of neighbors should not be larger than the number of stations");

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
            msg << "Unknown verbose flag " << flag << ". Accept 0 ~ 4";
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

double
Functions::wind_speed(double u, double v) {
    return (sqrt(u * u + v * v));
}

double
Functions::wind_dir(double u, double v) {

    // Positive u wind is from the west
    // Positive v wind is from the south
    // reference: http://colaweb.gmu.edu/dev/clim301/lectures/wind/wind-uv
    //
    double dir = fmod(atan2(v, u) * 180 / M_PI, 360);

    return (dir < 0 ? (dir + 360) : dir);
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

#ifdef _DISABLE_NON_HEADER_BOOST
    /*
     * Please note that if _DISABLE_NON_HEADER_BOOST is defined, this function
     * will be unavailable because it depends on the non-header portion of boost.
     * The R package BH only provides header-only boost so this function won't work.
     * I'm aware of the R package RcppBDT but, since this function is never used 
     * in R routines, I can simply disable this function and avoid an extra dependency.
     */
    throw runtime_error("Functions::toSeconds is disabled because it depends on non-header boost libraries");
#else
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
#endif
}

void
Functions::collapseLeadTimes(
        Observations & observations,
        const Forecasts & forecasts) {

    // Calculate the unique time combination from forecast times and lead times
    time_arr time_entry;
    set<time_arr, time_arr_compare> unique_times;

    const auto & fcst_times = forecasts.getTimes();
    const auto & fcst_flts = forecasts.getFLTs();

    /*
     * Insert times from forecasts to observations.
     * 
     * Please note that I'm looping first on forecast lead times and then forecast
     * times because I'm giving priority to earlier forecast lead times compared
     * to later lead time that are further into the future. I will keep the values
     * that are close to the initialization time.
     */
    for (size_t flt_i = 0; flt_i < fcst_flts.size(); ++flt_i) {
        for (size_t time_i = 0; time_i < fcst_times.size(); ++time_i) {

            time_entry[_TIME_INDEX] = time_i;
            time_entry[_FLT_INDEX] = flt_i;
            time_entry[_VALUE_INDEX] = fcst_times.getTime(time_i).timestamp
                    + fcst_flts.getTime(flt_i).timestamp;

            unique_times.insert(time_entry);
        }
    }

    // Insert the sorted unique times into observation times
    Times obs_times;
    size_t time_i = 0;
    const auto & unique_times_end = unique_times.end();
    for (auto it = unique_times.begin(); it != unique_times_end; ++it, ++time_i) {
        obs_times.push_back((*it)[_VALUE_INDEX]);
    }

    // Set dimensions for observations
    observations.setDimensions(forecasts.getParameters(), forecasts.getStations(), obs_times);
    observations.initialize(NAN);

    // Copy values from forecasts
    time_i = 0;
    size_t num_parameters = observations.getParameters().size();
    size_t num_stations = observations.getStations().size();

#if defined(_OPENMP)
#pragma omp parallel default(none) shared(unique_times, unique_times_end, \
num_parameters, num_stations, forecasts, observations) firstprivate(time_i)
#endif
    for (auto it = unique_times.begin(); it != unique_times_end; ++it, ++time_i) {
#if defined(_OPENMP)
#pragma omp for schedule(static) collapse(2)
#endif
        for (size_t parameter_i = 0; parameter_i < num_parameters; ++parameter_i) {
            for (size_t station_i = 0; station_i < num_stations; ++station_i) {
                double value = forecasts.getValue(parameter_i, station_i, (*it)[_TIME_INDEX], (*it)[_FLT_INDEX]);
                observations.setValue(value, parameter_i, station_i, time_i);
            }
        }
    }

    return;
}

void
Functions::unwrapTimeSeries(Forecasts & forecasts, const Times & times, const Times & flts,
        const Observations & observations) {

    /****************************
     * Initialize forecast data *
     * **************************/

    const auto & parameters = observations.getParameters();
    const auto & stations = observations.getStations();

    forecasts.setDimensions(parameters, stations, times, flts);
    forecasts.initialize(NAN);

    /*************
     * Copy data *
     * ***********/

    const auto & time_series = observations.getTimes();
    size_t ts_index;
    auto num_times = times.size();
    auto num_flts = flts.size();

#if defined(_OPENMP)
#pragma omp parallel for default(none) schedule(static) collapse(2) \
shared(times, flts, num_times, num_flts, time_series, parameters, stations, observations, forecasts) firstprivate(ts_index)
#endif
    for (size_t time_i = 0; time_i < num_times; ++time_i) {
        for (size_t flt_i = 0; flt_i < num_flts; ++flt_i) {

            // Calculate the time series index for this combination of forecast time and lead time
            auto ts_time = times.getTime(time_i).timestamp + flts.getTime(flt_i).timestamp;

            try {
                ts_index = time_series.getIndex(ts_time);
            } catch (range_error & e) {
                // Skip writing if the index cannot be found
                continue;
            }

            // Copy value for all stations and parameters
            for (size_t parameter_i = 0; parameter_i < parameters.size(); ++parameter_i) {
                for (size_t station_i = 0; station_i < stations.size(); ++station_i) {
                    double val = observations.getValue(parameter_i, station_i, ts_index);
                    forecasts.setValue(val, parameter_i, station_i, time_i, flt_i);
                }
            }
        }
    }

    return;
}

int
Functions::getStartIndex(int total, int num_procs, int rank) {
    // Rank 0 is the master. Start with rank 1.
    if (total < num_procs - 1) throw runtime_error("You are probably wasting computing resources. You requested too many processes.");
    return (int) ceil((rank - 1) * total / (float) (num_procs - 1));
}

int
Functions::getEndIndex(int total, int num_procs, int rank) {
    // Rank 0 is the master. Start with rank 1.
    if (total < num_procs - 1) throw runtime_error("You are probably wasting computing resources. You requested too many processes.");
    if (rank == num_procs - 1) return total;
    return (int) ceil(rank * total / (float) (num_procs - 1));
}

int
Functions::getSubTotal(int total, int num_procs, int rank) {
    return getEndIndex(total, num_procs, rank) - getStartIndex(total, num_procs, rank);
}

