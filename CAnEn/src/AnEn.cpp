/* 
 * File:   AnEn.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 * 
 * Created on July 23, 2018, 10:56 PM
 */

#include "AnEn.h"
#include "exception"
#include "colorTexts.h"

#define _USE_MATH_DEFINES
#include <cmath>
#include <ostream>

#if defined(_OPENMP)
#include <omp.h>
#endif

using namespace std;
using errorType = AnEn::errorType;
using simMethod = AnEn::simMethod;

const double MULTIPLY = M_PI / 180;
const double MULTIPLY_REVERSE = 180 / M_PI;

const size_t AnEn::_FILL_SIZE_T = std::numeric_limits<size_t>::max();

AnEn::AnEn() {
}

AnEn::AnEn(int verbose) :
verbose_(verbose) {
}

AnEn::~AnEn() {
}

errorType
AnEn::computeStandardDeviation(
        const Forecasts_array& forecasts, StandardDeviation& sds) {

    if (verbose_ >= 3) cout << "Computing standard deviation ... " << endl;

    size_t num_parameters = forecasts.getParametersSize();
    size_t num_stations = forecasts.getStationsSize();
    size_t num_times = forecasts.getTimesSize();
    size_t num_flts = forecasts.getFLTsSize();

    auto array = forecasts.data();

    vector<bool> circular_flags(num_parameters, false);
    auto & parameters_by_insert = forecasts.getParameters().get<anenPar::by_insert>();
    for (size_t i_parameter = 0; i_parameter < num_parameters; i_parameter++) {
        circular_flags[i_parameter] = parameters_by_insert[i_parameter].getCircular();
    }

    // Resize sds according to forecasts
    StandardDeviation::extent_gen extents;
    sds.resize(extents[num_parameters][num_stations][num_flts]);

#if defined(_OPENMP)
#pragma omp parallel for default(none) schedule(static) collapse(3)\
shared(num_parameters, num_stations, num_flts, num_times, array,\
circular_flags, sds) 
#endif
    for (size_t i_parameter = 0; i_parameter < num_parameters; i_parameter++) {
        for (size_t i_station = 0; i_station < num_stations; i_station++) {
            for (size_t i_flt = 0; i_flt < num_flts; i_flt++) {

                // Extract values for times
                vector<double> values(num_times);
                for (size_t i_time = 0; i_time < num_times; i_time++) {
                    values[i_time] = array [i_parameter][i_station][i_time][i_flt];
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
AnEn::computeSearchWindows(boost::numeric::ublas::matrix<size_t> & windows,
        size_t num_flts, size_t window_half_size) const {

    if (verbose_ >= 3) cout << "Computing search windows for FLT ... " << endl;

    int begin = 0;
    size_t end = 0;
    windows.resize(num_flts, 2);

    for (size_t i_flt = 0; i_flt < num_flts; i_flt++) {
        begin = i_flt - window_half_size;
        end = i_flt + window_half_size;
        windows(i_flt, 0) = (begin < 0) ? 0 : begin;
        windows(i_flt, 1) = (end >= num_flts) ? (num_flts - 1) : end;
    }

    return (SUCCESS);
}

errorType
AnEn::computeObservationsTimeIndices(
        const anenTime::Times & times_forecasts,
        const anenTime::Times & flts_forecasts,
        const anenTime::Times & times_observations,
        boost::numeric::ublas::matrix<size_t> & mapping) const {

    if (verbose_ >= 3) cout << "Computing mapping from forecast [Time, FLT] to observation [Time]  ... " << endl;

    mapping.resize(times_forecasts.size(), flts_forecasts.size());
    mapping.clear();

    const auto & times_forecasts_by_insert = times_forecasts.get<anenTime::by_insert>();
    const auto & flts_forecasts_by_insert = flts_forecasts.get<anenTime::by_insert>();

    size_t index = 0;

    // OpenMP does not allow return status. So this flag is used to keep track
    // of return status of the loop
    //
    int loop_flag = 1;

#if defined(_OPENMP)
#pragma omp parallel for default(none) schedule(static) collapse(2) reduction(min:loop_flag)\
shared(mapping, times_observations, times_forecasts_by_insert, flts_forecasts_by_insert, cout)\
firstprivate(index)
#endif
    for (size_t i_row = 0; i_row < mapping.size1(); i_row++) {
        for (size_t i_col = 0; i_col < mapping.size2(); i_col++) {

            if (times_observations.getTimeIndex(
                    times_forecasts_by_insert[i_row] +
                    flts_forecasts_by_insert[i_col], index)) {
                mapping(i_row, i_col) = index;
            } else {
                if (verbose_ >= 1) cout << BOLDRED << "Error: Could not find time for "
                        << "Time " << times_forecasts_by_insert[i_row]
                        << "FLT " << flts_forecasts_by_insert[i_col]
                        << "!" << RESET << endl;
                loop_flag = 0;
            }

        }
    }

    if (loop_flag == 0) return (OUT_OF_RANGE);
    else return (SUCCESS);
}

errorType
AnEn::computeSearchStations(
        const anenSta::Stations & test_stations,
        const anenSta::Stations & search_stations,
        boost::numeric::ublas::matrix<size_t> & i_search_stations_ref,
        size_t max_num_search_stations,
        double distance, size_t num_nearest_stations, bool return_index) {

    boost::numeric::ublas::matrix<size_t> i_search_stations(
            test_stations.size(), max_num_search_stations,
            AnEn::_FILL_SIZE_T);

    auto & test_stations_by_insert = test_stations.get<anenSta::by_insert>();

    if (verbose_ >= 3) cout << "Computing search space extension ... " << endl;

    if (num_nearest_stations == 0) {

#if defined(_OPENMP)
#pragma omp parallel for default(none) schedule(static)\
shared(test_stations_by_insert, search_stations, i_search_stations,\
distance, max_num_search_stations)
#endif
        if (distance == 0) {
            if (verbose_ >= 1) cout << BOLDRED << "Error: Please specify"
                    << "distance or/and number of nearest stations to find."
                    << RESET << endl;
            return (MISSING_VALUE);
        }

        for (size_t i_test = 0; i_test < test_stations_by_insert.size();
                i_test++) {

            // Find search stations based on the distance
            vector<size_t> i_search_station =
                    search_stations.getStationsIdByDistance(
                    test_stations_by_insert[i_test].getX(),
                    test_stations_by_insert[i_test].getY(),
                    distance);

            // Assign search stations to matrix
            for (size_t i_search = 0; i_search < max_num_search_stations &&
                    i_search < i_search_station.size(); i_search++) {
                i_search_stations(i_test, i_search) = i_search_station[i_search];
            }
        }

    } else {

#if defined(_OPENMP)
#pragma omp parallel for default(none) schedule(static)\
shared(test_stations_by_insert, search_stations, i_search_stations,\
num_nearest_stations, distance, max_num_search_stations)
#endif
        for (size_t i_test = 0; i_test < test_stations_by_insert.size();
                i_test++) {

            // Find nearest search stations also considering the threshold
            // distance.
            //
            vector<size_t> i_search_station =
                    search_stations.getNearestStationsId(
                    test_stations_by_insert[i_test].getX(),
                    test_stations_by_insert[i_test].getY(),
                    num_nearest_stations, distance);

            // Assign search stations to matrix
            for (size_t i_search = 0; i_search < max_num_search_stations &&
                    i_search < i_search_station.size(); i_search++) {
                i_search_stations(i_test, i_search) = i_search_station[i_search];
            }
        }
    }

    if (return_index) {
        auto & search_stations_by_ID = search_stations.get<anenSta::by_ID>();
        auto & search_stations_by_insert =
                search_stations.get<anenSta::by_insert>();

#if defined(_OPENMP)
#pragma omp parallel for default(none) schedule(static)\
shared(search_stations_by_ID, search_stations_by_insert, i_search_stations,\
_FILL_SIZE_T)
#endif
        for (size_t i_row = 0; i_row < i_search_stations.size1(); i_row++) {
            for (size_t i_col = 0; i_col < i_search_stations.size2(); i_col++) {

                if (i_search_stations(i_row, i_col) != _FILL_SIZE_T) {
                    auto it_ID = search_stations_by_ID.find(
                            i_search_stations(i_row, i_col));

                    if (it_ID != search_stations_by_ID.end()) {
                        i_search_stations(i_row, i_col) = std::distance(
                                search_stations_by_insert.begin(),
                                search_stations.project<anenSta::by_insert>(it_ID));
                    } else {
                        throw out_of_range("Can't find the station ID "
                                + to_string(i_search_stations(i_row, i_col)));
                    }
                }

            } // End loop of columns
        } // End of loop of rows
    }

    swap(i_search_stations, i_search_stations_ref);
    return (SUCCESS);
}

errorType
AnEn::computeSimilarity(
        const Forecasts_array& search_forecasts,
        const StandardDeviation& sds,
        SimilarityMatrices& sims,
        const Observations_array& search_observations,
        const boost::numeric::ublas::matrix<size_t> & mapping,
        size_t i_observation_parameter,
        boost::numeric::ublas::matrix<size_t> && i_search_stations) const {

    // Check input
    handleError(check_input_(search_forecasts, sds, sims,
            search_observations, mapping, i_observation_parameter));

    if (method_ == ONE_TO_MANY) {
        if (i_search_stations.size1() == 0 || i_search_stations.size2() == 0) {
            if (verbose_ >= 1) cout << BOLDRED
                    << "Error: Please Provide the search station matrix."
                    << RESET << endl;
            return (MISSING_VALUE);
        };
    }

    const Forecasts_array & test_forecasts = sims.getTargets();
    size_t num_parameters = test_forecasts.getParametersSize();
    size_t num_flts = test_forecasts.getFLTsSize();
    size_t num_test_stations = test_forecasts.getStationsSize();
    size_t num_test_times = test_forecasts.getTimesSize();
    size_t num_search_stations = search_forecasts.getStationsSize();
    size_t num_search_times = search_forecasts.getTimesSize();

    using COL_TAG_SIM = SimilarityMatrices::COL_TAG;

    if (verbose_ >= 4) {
        cout << "Session information for computeSimilarity: " << endl
                << "# of parameters: " << num_parameters << endl
                << "# of FLTs: " << num_flts << endl
                << "# of test stations: " << num_test_stations << endl
                << "# of test times: " << num_test_times << endl
                << "# of search stations: " << num_search_stations << endl
                << "# of search times: " << num_search_times << endl;
    }

    // Get data
    auto data_search_observations = search_observations.data();
    auto data_search_forecasts = search_forecasts.data();
    auto data_test_forecasts = test_forecasts.data();

    // Pre compute the window size for each FLT
    size_t window_half_size = 1;
    boost::numeric::ublas::matrix<size_t> flts_window;
    handleError(computeSearchWindows(flts_window, num_flts, window_half_size));

    // Get circular parameters and parameter weights
    vector<bool> circular_flags(num_parameters, false);
    vector<double> weights(num_parameters, NAN);
    auto & parameters_by_insert = test_forecasts.getParameters().get<anenPar::by_insert>();
    for (size_t i_parameter = 0; i_parameter < num_parameters; i_parameter++) {
        circular_flags[i_parameter] = parameters_by_insert[i_parameter].getCircular();
        weights[i_parameter] = parameters_by_insert[i_parameter].getWeight();
    }

    if (verbose_ >= 3) cout << "Computing similarity matrices ... " << endl;

    if (method_ == ONE_TO_ALL) {

        // Resize similarity matrices according to the search space
        sims.setMaxEntries(num_search_stations * num_search_times);
        sims.resize();
        fill(sims.data(), sims.data() + sims.num_elements(), NAN);

#if defined(_OPENMP)
#pragma omp parallel for default(none) schedule(static) collapse(5)\
shared(num_test_stations, num_flts, num_test_times, num_search_stations,\
num_search_times, circular_flags, num_parameters, data_search_observations,\
flts_window, mapping, weights, data_search_forecasts, data_test_forecasts,\
sims, sds)
#endif
        for (size_t i_test_station = 0; i_test_station < num_test_stations; i_test_station++) {
            for (size_t i_test_time = 0; i_test_time < num_test_times; i_test_time++) {
                for (size_t i_flt = 0; i_flt < num_flts; i_flt++) {
                    for (size_t i_search_station = 0; i_search_station < num_search_stations; i_search_station++) {
                        for (size_t i_search_time = 0; i_search_time < num_search_times; i_search_time++) {

                            if (!isnan(data_search_observations[i_observation_parameter][i_search_station]
                                    [mapping(i_search_time, i_flt)])) {

                                size_t i_sim_row = i_search_station * num_search_times + i_search_time;
                                if (isnan(sims[i_test_station][i_test_time][i_flt][i_sim_row][COL_TAG_SIM::VALUE]))
                                    sims[i_test_station][i_test_time][i_flt][i_sim_row][COL_TAG_SIM::VALUE] = 0;

                                for (size_t i_parameter = 0; i_parameter < num_parameters; i_parameter++) {

                                    if (weights[i_parameter] != 0) {
                                        vector<double> window(flts_window(i_flt, 1) - flts_window(i_flt, 0) + 1);
                                        short pos = 0;

                                        for (size_t i_window_flt = flts_window(i_flt, 0);
                                                i_window_flt <= flts_window(i_flt, 1); i_window_flt++, pos++) {

                                            double value_search = data_search_forecasts
                                                    [i_parameter][i_search_station][i_search_time][i_window_flt];
                                            double value_test = data_test_forecasts
                                                    [i_parameter][i_test_station][i_test_time][i_window_flt];

                                            if (isnan(value_search) || isnan(value_test)) window[pos] = NAN;

                                            if (circular_flags[i_parameter]) {
                                                window[pos] = pow(diffCircular(value_search, value_test), 2);
                                            } else {
                                                window[pos] = pow(value_search - value_test, 2);
                                            }
                                        } // End loop of search window FLTs

                                        double average = mean(window);

                                        if (sds[i_parameter][i_search_station][i_flt] > 0 && !isnan(average)) {
                                            sims[i_test_station][i_test_time][i_flt][i_sim_row][COL_TAG_SIM::VALUE]
                                                    += weights[i_parameter] * (sqrt(average) / sds[i_parameter][i_search_station][i_flt]);
                                        }
                                    }
                                } // End loop of parameters

                                sims[i_test_station][i_test_time][i_flt][i_sim_row][COL_TAG_SIM::STATION] = i_search_station;
                                sims[i_test_station][i_test_time][i_flt][i_sim_row][COL_TAG_SIM::TIME] = i_search_time;
                            }
                        } // End loop of search times
                    } // End loop of search stations
                } // End loop of FLTs
            } // End loop of test times
        } // End loop of test stations

    } else if (method_ == ONE_TO_MANY) {

        // Resize similarity matrices according to the search space
        sims.setMaxEntries(i_search_stations.size2() * num_search_times);
        sims.resize();
        fill(sims.data(), sims.data() + sims.num_elements(), NAN);

        num_search_stations = i_search_stations.size2();

#if defined(_OPENMP)
#pragma omp parallel for default(none) schedule(static) collapse(4)\
shared(num_test_stations, num_flts, num_test_times, num_search_stations,\
num_search_times, circular_flags, num_parameters, data_search_observations,\
flts_window, mapping, weights, data_search_forecasts, data_test_forecasts,\
sims, sds)
#endif
        for (size_t i_test_station = 0; i_test_station < num_test_stations; i_test_station++) {
            for (size_t i_test_time = 0; i_test_time < num_test_times; i_test_time++) {
                for (size_t i_flt = 0; i_flt < num_flts; i_flt++) {
                    for (size_t i_search_station_index = 0; i_search_station_index < num_search_stations; i_search_station_index++) {

                        size_t i_search_station = i_search_stations(i_test_station, i_search_station_index);
                        if (i_search_station != AnEn::_FILL_SIZE_T) {

                            for (size_t i_search_time = 0; i_search_time < num_search_times; i_search_time++) {

                                if (!isnan(data_search_observations[i_observation_parameter][i_search_station]
                                        [mapping(i_search_time, i_flt)])) {

                                    size_t i_sim_row = i_search_station * num_search_times + i_search_time;
                                    if (isnan(sims[i_test_station][i_test_time][i_flt][i_sim_row][COL_TAG_SIM::VALUE]))
                                        sims[i_test_station][i_test_time][i_flt][i_sim_row][COL_TAG_SIM::VALUE] = 0;

                                    for (size_t i_parameter = 0; i_parameter < num_parameters; i_parameter++) {

                                        if (weights[i_parameter] != 0) {
                                            vector<double> window(flts_window(i_flt, 1) - flts_window(i_flt, 0) + 1);
                                            short pos = 0;

                                            for (size_t i_window_flt = flts_window(i_flt, 0);
                                                    i_window_flt <= flts_window(i_flt, 1); i_window_flt++, pos++) {

                                                double value_search = data_search_forecasts
                                                        [i_parameter][i_search_station][i_search_time][i_window_flt];
                                                double value_test = data_test_forecasts
                                                        [i_parameter][i_test_station][i_test_time][i_window_flt];

                                                if (isnan(value_search) || isnan(value_test)) window[pos] = NAN;

                                                if (circular_flags[i_parameter]) {
                                                    window[pos] = pow(diffCircular(value_search, value_test), 2);
                                                } else {
                                                    window[pos] = pow(value_search - value_test, 2);
                                                }
                                            } // End loop of search window FLTs

                                            double average = mean(window);

                                            if (sds[i_parameter][i_search_station][i_flt] > 0 && !isnan(average)) {
                                                sims[i_test_station][i_test_time][i_flt][i_sim_row][COL_TAG_SIM::VALUE]
                                                        += weights[i_parameter] * (sqrt(average) / sds[i_parameter][i_search_station][i_flt]);
                                            }
                                        }
                                    } // End loop of parameters

                                    sims[i_test_station][i_test_time][i_flt][i_sim_row][COL_TAG_SIM::STATION] = i_search_station;
                                    sims[i_test_station][i_test_time][i_flt][i_sim_row][COL_TAG_SIM::TIME] = i_search_time;
                                }
                            } // End loop of search times
                        }
                    } // End loop of search stations
                } // End loop of FLTs
            } // End loop of test times
        } // End loop of test stations

    } else if (method_ == ONE_TO_ONE) {

        // Resize similarity matrices according to the search space
        sims.setMaxEntries(num_search_times);
        sims.resize();
        fill(sims.data(), sims.data() + sims.num_elements(), NAN);

#if defined(_OPENMP)
#pragma omp parallel for default(none) schedule(static) collapse(4)\
shared(num_test_stations, num_flts, num_test_times, num_search_stations,\
num_search_times, circular_flags, num_parameters, data_search_observations,\
flts_window, mapping, weights, data_search_forecasts, data_test_forecasts,\
sims, sds)
#endif
        for (size_t i_station = 0; i_station < num_test_stations; i_station++) {
            for (size_t i_test_time = 0; i_test_time < num_test_times; i_test_time++) {
                for (size_t i_flt = 0; i_flt < num_flts; i_flt++) {
                    for (size_t i_search_time = 0; i_search_time < num_search_times; i_search_time++) {

                        if (!isnan(data_search_observations[i_observation_parameter][i_station]
                                [mapping(i_search_time, i_flt)])) {

                            if (isnan(sims[i_station][i_test_time][i_flt][i_search_time][COL_TAG_SIM::VALUE]))
                                sims[i_station][i_test_time][i_flt][i_search_time][COL_TAG_SIM::VALUE] = 0;
                            for (size_t i_parameter = 0; i_parameter < num_parameters; i_parameter++) {

                                if (weights[i_parameter] != 0) {
                                    vector<double> window(flts_window(i_flt, 1) - flts_window(i_flt, 0) + 1);
                                    short pos = 0;

                                    for (size_t i_window_flt = flts_window(i_flt, 0);
                                            i_window_flt <= flts_window(i_flt, 1); i_window_flt++, pos++) {

                                        double value_search = data_search_forecasts
                                                [i_parameter][i_station][i_search_time][i_window_flt];
                                        double value_test = data_test_forecasts
                                                [i_parameter][i_station][i_test_time][i_window_flt];

                                        if (isnan(value_search) || isnan(value_test)) window[pos] = NAN;

                                        if (circular_flags[i_parameter]) {
                                            window[pos] = pow(diffCircular(value_search, value_test), 2);
                                        } else {
                                            window[pos] = pow(value_search - value_test, 2);
                                        }
                                    } // End loop of search window FLTs

                                    double average = mean(window);

                                    if (sds[i_parameter][i_station][i_flt] > 0 && !isnan(average)) {
                                        sims[i_station][i_test_time][i_flt][i_search_time][COL_TAG_SIM::VALUE]
                                                += weights[i_parameter] * (sqrt(average) / sds[i_parameter][i_station][i_flt]);
                                    }
                                }

                            } // End loop of parameters

                            sims[i_station][i_test_time][i_flt][i_search_time][COL_TAG_SIM::STATION] = i_station;
                            sims[i_station][i_test_time][i_flt][i_search_time][COL_TAG_SIM::TIME] = i_search_time;
                        }
                    } // End loop of search times
                } // End loop of FLTs
            } // End loop of test times
        } // End loop of test stations
    } else {
        return (UNKNOWN_METHOD);
    }

    return (SUCCESS);
}

errorType
AnEn::selectAnalogs(
        Analogs & analogs,
        SimilarityMatrices & sims,
        const Observations_array& search_observations,
        boost::numeric::ublas::matrix<size_t> mapping,
        size_t i_parameter, size_t num_members, bool quick) const {

    if (verbose_ >= 3) cout << "Selecting analogs ..." << endl;

    if (num_members >= sims.getMaxEntries()) {
        if (verbose_ >= 1) cout << BOLDRED << "Error: Number of members is "
                << "bigger than the number of entries in SimilarityMatrices!"
                << RESET << endl;
        return (OUT_OF_RANGE);
    }

    if (i_parameter >= search_observations.getParametersSize()) {
        if (verbose_ >= 1) cout << BOLDRED << "Error: i_parameter exceeds the limits. "
                << "There are only " << search_observations.getParametersSize()
            << " parameters available." << RESET << endl;
        return (OUT_OF_RANGE);
    }

    size_t num_test_stations = sims.getTargets().getStationsSize();
    size_t num_test_times = sims.getTargets().getTimesSize();
    size_t num_flts = sims.getTargets().getFLTsSize();
    auto & observation_times_by_insert =
            search_observations.getTimes().get<anenTime::by_insert>();

    Analogs::extent_gen extents;
    analogs.resize(extents[0][0][0][0][0]);
    analogs.resize(extents[num_test_stations][num_test_times][num_flts][num_members][3]);

    using COL_TAG_ANALOG = Analogs::COL_TAG;
    using COL_TAG_SIM = SimilarityMatrices::COL_TAG;

    sims.sortRows(quick, num_members);

    const auto & data_observations = search_observations.data();

#if defined(_OPENMP)
#pragma omp parallel for default(none) schedule(static) collapse(4)\
shared(data_observations, sims, num_members, mapping, analogs, num_test_stations,\
i_parameter, num_test_times, num_flts, observation_times_by_insert)
#endif
    for (size_t i_test_station = 0; i_test_station < num_test_stations; i_test_station++) {
        for (size_t i_test_time = 0; i_test_time < num_test_times; i_test_time++) {
            for (size_t i_flt = 0; i_flt < num_flts; i_flt++) {
                for (size_t i_member = 0; i_member < num_members; i_member++) {

                    if (!isnan(sims[i_test_station][i_test_time][i_flt][i_member][COL_TAG_SIM::VALUE])) {
                        size_t i_search_station = (size_t) sims[i_test_station][i_test_time][i_flt][i_member][COL_TAG_SIM::STATION];
                        size_t i_search_forecast_time = (size_t) sims[i_test_station][i_test_time][i_flt][i_member][COL_TAG_SIM::TIME];
                        size_t i_observation_time = mapping(i_search_forecast_time, i_flt);

                        analogs[i_test_station][i_test_time][i_flt][i_member][COL_TAG_ANALOG::STATION] = i_search_station;
                        analogs[i_test_station][i_test_time][i_flt][i_member][COL_TAG_ANALOG::TIME] =
                                observation_times_by_insert[i_observation_time];

                        analogs[i_test_station][i_test_time][i_flt][i_member][COL_TAG_ANALOG::VALUE] =
                                data_observations[i_parameter][i_search_station][i_observation_time];
                    }

                }
            }
        }
    }

    return (SUCCESS);
}

void
AnEn::handleError(const errorType & indicator) const {
    if (indicator != SUCCESS) {
        throw runtime_error("Error code " + to_string(indicator));
    }
}

int
AnEn::getVerbose() const {

    return verbose_;
}

simMethod
AnEn::getMethod() const {
    return method_;
}

void
AnEn::setMethod(simMethod method) {
    if (method == ONE_TO_ALL || method == ONE_TO_MANY || method == ONE_TO_ONE)
        this->method_ = method;
}

void
AnEn::setVerbose(int verbose) {
    verbose_ = verbose;
}

double
AnEn::sdLinear(const vector<double>& values) const {
    return (sqrt(variance(values)));
}

double
AnEn::sdCircular(const vector<double>& values) const {

    double rad;
    vector<double> sins(values.size());
    vector<double> coss(values.size());

    for (unsigned int i = 0; i < values.size(); i++) {

        // This is to convert from degrees to radians
        //
        rad = values[i] * MULTIPLY;

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
AnEn::mean(const std::vector<double>& values) const {
    double sum = 0.0;
    size_t valid = 0;

    for (const auto & value : values) {
        if (!isnan(value)) {
            sum += value;
            valid++;
        }
    }

    if (valid == 0) return NAN;

    return (sum / (double) valid);
}

double
AnEn::variance(const std::vector<double>& values) const {
    double average = mean(values);
    if (isnan(average)) return NAN;

    double sum = 0.0;
    size_t valid = 0;

    for (const auto & value : values) {
        if (!isnan(value)) {

            sum += pow(value - average, 2);
            valid++;
        }
    }

    return (sum / ((double) valid - 1.0));
}

double
AnEn::diffCircular(double i, double j) const {
    double res1 = abs(i - j);
    double res2 = abs(res1 - 360);
    return (min(res1, res2));
}

errorType
AnEn::check_input_(
        const Forecasts_array& search_forecasts,
        const StandardDeviation& sds,
        SimilarityMatrices& sims,
        const Observations_array& search_observations,
        boost::numeric::ublas::matrix<size_t> mapping,
        size_t i_observation_parameter) const {

    const Forecasts_array & test_forecasts = sims.getTargets();

    size_t num_parameters = test_forecasts.getParametersSize();
    size_t num_test_stations = test_forecasts.getStationsSize();
    size_t num_flts = test_forecasts.getFLTsSize();
    size_t num_search_stations = search_forecasts.getStationsSize();

    if (i_observation_parameter >= search_observations.getParametersSize()) {
        if (verbose_ >= 1) cout << BOLDRED
                << "Error: Please specify a valid observation variable!"
                << RESET << endl;
        return (OUT_OF_RANGE);
    }

    if (mapping.size1() != search_forecasts.getTimesSize()) {
        if (verbose_ >= 1) cout << BOLDRED
                << "Error: The rows of mapping should equal the number of forecast times!"
                << RESET << endl;
        return (WRONG_SHAPE);
    }

    if (mapping.size2() != search_forecasts.getFLTsSize()) {
        if (verbose_ >= 1) cout << BOLDRED
                << "Error: The columns of mapping should equal the number of FLTs!"
                << RESET << endl;
        return (WRONG_SHAPE);
    }

    if (!(sds.shape()[0] == search_forecasts.getParametersSize() &&
            sds.shape()[1] == search_forecasts.getStationsSize() &&
            sds.shape()[2] == search_forecasts.getFLTsSize())) {
        if (verbose_ >= 1) cout << BOLDRED
                << "Error: Standard deviation array has a different shape to search forecasts!"
                << RESET << endl;
        return (WRONG_SHAPE);
    }

    if (num_parameters != search_forecasts.getParametersSize()) {
        if (verbose_ >= 1) cout << BOLDRED
                << "Error: Search and test forecasts should have same numbers of parameters!"
                << RESET << endl;
        return (WRONG_SHAPE);
    }

    if (num_flts != search_forecasts.getFLTsSize()) {
        if (verbose_ >= 1) cout << BOLDRED
                << "Error: Search and test forecasts should have same numbers of FLTs!"
                << RESET << endl;
        return (WRONG_SHAPE);
    }

    if (num_search_stations != search_observations.getStationsSize()) {
        if (verbose_ >= 1) cout << BOLDRED
                << "Error: Search forecasts and observations should have same numbers of stations!"
                << RESET << endl;
        return (WRONG_SHAPE);
    }

    if (method_ == ONE_TO_ONE) {
        if (num_test_stations != search_forecasts.getStationsSize()) {
            if (verbose_ >= 1) cout << BOLDRED
                    << "Error: Search and test forecasts should have same numbers of stations!"
                    << RESET << endl;
            return (WRONG_SHAPE);
        }
    }

    return (SUCCESS);
}
