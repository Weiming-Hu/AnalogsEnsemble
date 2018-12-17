/* 
 * File:   AnEn.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 * 
 * Created on July 23, 2018, 10:56 PM
 */

#include "AnEn.h"
#include "exception"
#include "colorTexts.h"
#include <boost/numeric/ublas/io.hpp>

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

const double AnEn::_FILL_VALUE = NAN;

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
#pragma omp parallel for default(none) schedule(static) collapse(3) \
shared(num_parameters, num_stations, num_flts, num_times, array, \
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
AnEn::computeObservationsTimeIndices(
        const anenTime::Times & times_forecasts,
        const anenTime::Times & flts_forecasts,
        const anenTime::Times & times_observations,
        AnEn::TimeMapMatrix & mapping_ref,
        int time_match_mode) const {

    if (verbose_ >= 3) cout << "Computing mapping from forecast [Time, FLT] to observation [Time]  ... " << endl;

    AnEn::TimeMapMatrix mapping(times_forecasts.size(), flts_forecasts.size(), AnEn::_FILL_VALUE);

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
            if (verbose_ >= 1) cout << BOLDRED
                << "Error: Could not find some search forecast times in observation times."
                    << RESET << endl;
            return (OUT_OF_RANGE);
        } else {
            if (verbose_ >= 2) cout << RED
                << "Warning: Could not find some search forecast times in observation times."
                    << RESET << endl;
        }
    }

    swap(mapping_ref, mapping);
    return (SUCCESS);
}

errorType
AnEn::computeSearchStations(
        const anenSta::Stations & test_stations,
        const anenSta::Stations & search_stations,
        AnEn::SearchStationMatrix & i_search_stations_ref,
        size_t max_num_search_stations,
        double distance, size_t num_nearest_stations, bool return_index) const {

    // Create a new i_search_stations with the fill value
    AnEn::SearchStationMatrix i_search_stations(
            test_stations.size(), max_num_search_stations,
            AnEn::_FILL_VALUE);

    // Define variables for perfectly nested parallel loops for collapse
    const auto & test_stations_by_insert = test_stations.get<anenSta::by_insert>();
    auto limit_test = test_stations_by_insert.size();
    
    // Check whether stations have xs and ys
    bool have_xy = test_stations.haveXY() && search_stations.haveXY();

    try {
        if (method_ == ONE_TO_ONE) {

            // Resize the table because each test station will be assigned with 
            // only one search station.
            //
            i_search_stations.resize(test_stations.size(), 1);
            
            if (have_xy) {
                if (verbose_ >= 3) cout << "Computing one-on-one match between search and test stations based on xs and ys ... " << endl;

#if defined(_OPENMP)
#pragma omp parallel for default(none) schedule(static) \
shared(test_stations_by_insert, search_stations, i_search_stations, limit_test)
#endif
                for (size_t i_test = 0; i_test < limit_test; i_test++) {

                    // Find the nearest search stations
                    size_t i_search_station = search_stations.getNearestStationsId(
                            test_stations_by_insert[i_test].getX(),
                            test_stations_by_insert[i_test].getY(), 1)[0];

                    i_search_stations(i_test, 0) = i_search_station;
                }
            } else {
                if (verbose_ >= 3) cout << "Computing one-on-one match between search and test stations based on indices ... " << endl;
                
                // If location info is not provided for search and test stations, I'm going to assume they are
                // perfectly aligned with each other.
                //
                if (search_stations.size() != test_stations.size()) {
                    cout << BOLDRED << "Error: Search stations and test stations must be the same when no location info is provided."
                            << RESET << endl;
                    return (WRONG_METHOD);
                }
                
                // Fill the table with incremental values
                size_t index = 0;
                for (size_t i_test = 0; i_test < limit_test; i_test++) {
                    i_search_stations(i_test, 0) = index;
                    ++index;
                }
            }

        } else if (method_ == ONE_TO_MANY) {
            
            if (!have_xy) {
                cout << BOLDRED << "Error: Xs and ys are required for search space extension." 
                        << RESET << endl;
                return (WRONG_METHOD);
            }
            
            if (verbose_ >= 3) cout << "Computing search space extension ... " << endl;

            if (num_nearest_stations == 0) {

                if (distance == 0) {
                    if (verbose_ >= 1) cout << BOLDRED << "Error: Please specify"
                            << " distance or/and number of nearest stations to find."
                            << RESET << endl;
                    return (MISSING_VALUE);
                }

#if defined(_OPENMP)
#pragma omp parallel for default(none) schedule(static) \
shared(test_stations_by_insert, search_stations, i_search_stations, \
distance, max_num_search_stations, limit_test)
#endif
                for (size_t i_test = 0; i_test < limit_test; i_test++) {

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
#pragma omp parallel for default(none) schedule(static) \
shared(test_stations_by_insert, search_stations, i_search_stations, \
num_nearest_stations, distance, max_num_search_stations, limit_test)
#endif
                for (size_t i_test = 0; i_test < limit_test; i_test++) {

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

        } else {
            if (verbose_ >= 1) cout << BOLDRED << "Error: Unknown method ("
                    << method_ << ")!" << RESET << endl;
            return (UNKNOWN_METHOD);
        }

        if (return_index && have_xy) {
            auto & search_stations_by_ID = search_stations.get<anenSta::by_ID>();
            auto & search_stations_by_insert =
                    search_stations.get<anenSta::by_insert>();

            // Define variables for perfectly nested parallel loops with collapse
            auto limit_search_row = i_search_stations.size1();
            auto limit_search_col = i_search_stations.size2();

#if defined(_OPENMP)
#pragma omp parallel for default(none) schedule(static) \
shared(search_stations_by_ID, search_stations_by_insert, i_search_stations, \
search_stations, limit_search_row, limit_search_col)
#endif
            for (size_t i_row = 0; i_row < limit_search_row; i_row++) {
                for (size_t i_col = 0; i_col < limit_search_col; i_col++) {

                    // Check if the value is _FILL_VALUE
                    if (!std::isnan(i_search_stations(i_row, i_col))) {
                        auto it_ID = search_stations_by_ID.find(
                                i_search_stations(i_row, i_col));

                        if (it_ID != search_stations_by_ID.end()) {
                            i_search_stations(i_row, i_col) = std::distance(
                                    search_stations_by_insert.begin(),
                                    search_stations.project<anenSta::by_insert>(it_ID));
                        } else {
                            throw out_of_range("Can't find the station ID "
                                    + to_string((long long) i_search_stations(i_row, i_col)));
                        }
                    }

                } // End loop of columns
            } // End of loop of rows
        }
    } catch (...) {
        cout << BOLDRED << "Error occurred in computeSearchStations!" << RESET << endl;
        throw;
    }

    swap(i_search_stations, i_search_stations_ref);
    
    if (verbose_ >= 4) {
        cout << "i_search_stations: " << i_search_stations_ref << endl;
    }
    
    return (SUCCESS);
}

errorType
AnEn::computeSimilarity(
        const Forecasts_array& search_forecasts,
        const StandardDeviation& sds,
        SimilarityMatrices& sims,
        const Observations_array& search_observations,
        const AnEn::TimeMapMatrix & mapping,
        const AnEn::SearchStationMatrix & i_search_stations,
        size_t i_observation_parameter,
        double max_par_nan, double max_flt_nan) const {

    // Check input
    handleError(check_input_(search_forecasts, sds, sims,
            search_observations, mapping, i_observation_parameter));

    const Forecasts_array & test_forecasts = sims.getTargets();
    size_t num_parameters = test_forecasts.getParametersSize();
    size_t num_flts = test_forecasts.getFLTsSize();
    size_t num_test_stations = test_forecasts.getStationsSize();
    size_t num_test_times = test_forecasts.getTimesSize();
    size_t num_search_stations = i_search_stations.size2();
    size_t num_search_times = search_forecasts.getTimesSize();
    double num_par_nan = 0.0;

    using COL_TAG_SIM = SimilarityMatrices::COL_TAG;

    // Check i_search_stations
    if (i_search_stations.size1() == 0 || num_search_stations == 0) {
        if (verbose_ >= 1) cout << BOLDRED
                << "Error: The search station lookup table is empty." << RESET << endl;
        return (MISSING_VALUE);
    }
    
    if (i_search_stations.size1() != num_test_stations) {
        if (verbose_ >= 1) cout << BOLDRED
                << "Error: Number of rows in i_search_stations ("
                << i_search_stations.size1() << ") should equal the number of test stations ("
                << num_test_stations << ")." << RESET << endl;
        return (WRONG_SHAPE);
    }

    if (method_ == ONE_TO_ONE) {
        if (num_search_stations != 1) {
            cout << BOLDRED
                    << "Error: More than one search stations are assigned."
                    << RESET << endl;
            return (WRONG_METHOD);
        }
    }

    if (verbose_ >= 4) {
        cout << "Session information for computeSimilarity: " << endl
                << "# of parameters: " << num_parameters << endl
                << "# of FLTs: " << num_flts << endl
                << "# of test stations: " << num_test_stations << endl
                << "# of test times: " << num_test_times << endl
                << "# of maximum search stations: " << num_search_stations << endl
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
    vector<double> weights(num_parameters, 1);
    auto & parameters_by_insert = test_forecasts.getParameters().get<anenPar::by_insert>();
    for (size_t i_parameter = 0; i_parameter < num_parameters; i_parameter++) {
        circular_flags[i_parameter] = parameters_by_insert[i_parameter].getCircular();
        weights[i_parameter] = parameters_by_insert[i_parameter].getWeight();
        if (std::isnan(weights[i_parameter])) weights[i_parameter] = 1;
    }

    if (verbose_ >= 3) cout << "Computing similarity matrices ... " << endl;

    // Resize similarity matrices according to the search space
    sims.setMaxEntries(num_search_stations * num_search_times);
    sims.resize();
    fill(sims.data(), sims.data() + sims.num_elements(), NAN);

#if defined(_OPENMP)
#pragma omp parallel for default(none) schedule(static) collapse(4) \
shared(num_test_stations, num_flts, num_test_times, num_search_stations, \
num_search_times, circular_flags, num_parameters, data_search_observations, \
flts_window, mapping, weights, data_search_forecasts, data_test_forecasts, \
sims, sds, i_observation_parameter, i_search_stations, max_flt_nan, max_par_nan) \
firstprivate(num_par_nan)
#endif
    for (size_t i_test_station = 0; i_test_station < num_test_stations; i_test_station++) {
        for (size_t i_test_time = 0; i_test_time < num_test_times; i_test_time++) {
            for (size_t i_flt = 0; i_flt < num_flts; i_flt++) {
                for (size_t i_search_station_index = 0; i_search_station_index < num_search_stations; i_search_station_index++) {

                    double i_search_station = i_search_stations(i_test_station, i_search_station_index);

                    // Check if search station is NAN, which is the _FILL_VALUE
                    if (!std::isnan(i_search_station)) {
                        for (size_t i_search_time = 0; i_search_time < num_search_times; i_search_time++) {

                            if (!std::isnan(mapping(i_search_time, i_flt))) {

                                // TODO: potential bug here when extend_observations is set to false. We should check the observation
                                // from target station and this dependents on whether the target station is from the search stations
                                // or it is the test station.
                                //
                                if (!std::isnan(data_search_observations[i_observation_parameter][i_search_station]
                                        [mapping(i_search_time, i_flt)])) {

                                    size_t i_sim_row = i_search_station_index * num_search_times + i_search_time;
                                    if (std::isnan(sims[i_test_station][i_test_time][i_flt][i_sim_row][COL_TAG_SIM::VALUE]))
                                        sims[i_test_station][i_test_time][i_flt][i_sim_row][COL_TAG_SIM::VALUE] = 0;

                                    num_par_nan = 0;

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

                                                if (std::isnan(value_search) || std::isnan(value_test)) window[pos] = NAN;

                                                if (circular_flags[i_parameter]) {
                                                    window[pos] = pow(diffCircular(value_search, value_test), 2);
                                                } else {
                                                    window[pos] = pow(value_search - value_test, 2);
                                                }
                                            } // End loop of search window FLTs

                                            double average = mean(window, max_flt_nan);

                                            if (!std::isnan(average)) {
                                                if (sds[i_parameter][i_search_station][i_flt] > 0) {
                                                    sims[i_test_station][i_test_time][i_flt][i_sim_row][COL_TAG_SIM::VALUE]
                                                            += weights[i_parameter] * (sqrt(average) / sds[i_parameter][i_search_station][i_flt]);
                                                }
                                            } else {
                                                // Record this nan parameter average
                                                num_par_nan++;

                                                if (!std::isnan(max_par_nan)) {
                                                    // If a maximum of nan parameter is set
                                                    if (num_par_nan > max_par_nan) {
                                                        sims[i_test_station][i_test_time][i_flt][i_sim_row][COL_TAG_SIM::VALUE] = NAN;
                                                        break;
                                                    }
                                                }
                                            }
                                        }
                                    } // End loop of parameters

                                    sims[i_test_station][i_test_time][i_flt][i_sim_row][COL_TAG_SIM::STATION] = i_search_station;
                                    sims[i_test_station][i_test_time][i_flt][i_sim_row][COL_TAG_SIM::TIME] = i_search_time;
                                } // End of isnan(observation value)
                            } // End of isnan(mapping value)
                        } // End loop of search times
                    }
                } // End loop of search stations
            } // End loop of FLTs
        } // End loop of test times
    } // End loop of test stations

    return (SUCCESS);
}

errorType
AnEn::selectAnalogs(
        Analogs & analogs,
        SimilarityMatrices & sims,
        const Observations_array & search_observations,
        const AnEn::TimeMapMatrix & mapping,
        size_t i_parameter, size_t num_members,
        bool quick, bool extend_observations,
        bool preserve_real_time) const {

    if (i_parameter >= search_observations.getParametersSize()) {
        if (verbose_ >= 1) cout << BOLDRED << "Error: i_parameter exceeds the limits. "
                << "There are only " << search_observations.getParametersSize()
            << " parameters available." << RESET << endl;
        return (OUT_OF_RANGE);
    }

    size_t num_test_stations = sims.getTargets().getStationsSize();
    size_t num_test_times = sims.getTargets().getTimesSize();
    size_t num_flts = sims.getTargets().getFLTsSize();
    size_t max_members = sims.getMaxEntries();

    if (num_members >= max_members) {
        if (verbose_ >= 2) cout << BOLDRED << "Warning: Number of members is "
                << "bigger than the number of entries in SimilarityMatrices! "
                << " NAN will exist in Analogs." << RESET << endl;
    }

    Analogs::extent_gen extents;
    analogs.resize(extents[0][0][0][0][0]);
    analogs.resize(extents[num_test_stations][num_test_times][num_flts][num_members][3]);
    fill_n(analogs.data(), analogs.num_elements(), NAN);

    using COL_TAG_ANALOG = Analogs::COL_TAG;
    using COL_TAG_SIM = SimilarityMatrices::COL_TAG;

    sims.sortRows(quick, num_members);

    const auto & data_observations = search_observations.data();

    vector<size_t> test_stations_index_in_search(0);
    if (!extend_observations) {
        // If only observations from the main (center) station are used in the analogs, I need to
        // find the corresponding station index in the search stations to the main station which
        // comes from the test stations.
        //

        const auto & search_observation_stations = search_observations.getStations();
        if (search_observation_stations.size() == 0) {
            if (verbose_ >= 1) cout << BOLDRED
                << "Error: There is no location information provided for search stations in observations." << RESET << endl;
            return(MISSING_VALUE);
        }

        const auto & test_stations = sims.getTargets().getStations();
        if (test_stations.size() == 0) {
            if (verbose_ >= 1) cout << BOLDRED
                << "Error: There is no location information provided for test stations in SimilarityMatrices." << RESET << endl;
            return(MISSING_VALUE);
        }
        
        if (search_observation_stations.haveXY() && test_stations.haveXY()) {
            if (verbose_ >= 3) cout << "Finding the corresponding indices of test stations in search stations based on xs and ys ..." << endl;
            const auto & test_stations_by_insert = test_stations.get<anenSta::by_insert>();

            for_each(test_stations_by_insert.begin(), test_stations_by_insert.end(),
                    [&test_stations_index_in_search, &search_observation_stations](anenSta::Station test) {
                        size_t id = search_observation_stations.getNearestStationsId(test.getX(), test.getY(), 1)[0];
                        test_stations_index_in_search.push_back(search_observation_stations.getStationIndex(id));
                    });
        } else {
            if (verbose_ >= 3) cout << "Finding the corresponding indices of test stations in search stations based on indices ..." << endl;
            
            if (search_observation_stations.size() != test_stations.size()) {
                cout << BOLDRED << "Error: Search stations and test stations must be the same when no location info is provided."
                        << RESET << endl;
                return (WRONG_METHOD);
            }
            
            test_stations_index_in_search.resize(num_test_stations);
            iota(test_stations_index_in_search.begin(), test_stations_index_in_search.end(), 0);
        }
    }

    if (verbose_ >= 3) cout << "Selecting analogs ..." << endl;

    if (preserve_real_time) {
        auto & observation_times_by_insert =
                search_observations.getTimes().get<anenTime::by_insert>();

#if defined(_OPENMP)
#pragma omp parallel for default(none) schedule(dynamic) collapse(4) \
shared(data_observations, sims, num_members, mapping, analogs, num_test_stations, \
i_parameter, num_test_times, num_flts, observation_times_by_insert, max_members, \
extend_observations, test_stations_index_in_search)
#endif
        for (size_t i_test_station = 0; i_test_station < num_test_stations; i_test_station++) {
            for (size_t i_test_time = 0; i_test_time < num_test_times; i_test_time++) {
                for (size_t i_flt = 0; i_flt < num_flts; i_flt++) {
                    for (size_t i_member = 0; i_member < num_members; i_member++) {

                        if (i_member < max_members) {
                            if (!std::isnan(sims[i_test_station][i_test_time][i_flt][i_member][COL_TAG_SIM::VALUE])) {

                                size_t i_search_station = 0;
                                if (extend_observations) {
                                    i_search_station = (size_t) sims[i_test_station][i_test_time][i_flt][i_member][COL_TAG_SIM::STATION];
                                } else {
                                    i_search_station = test_stations_index_in_search[i_test_station];
                                }

                                size_t i_search_forecast_time = (size_t) sims[i_test_station][i_test_time][i_flt][i_member][COL_TAG_SIM::TIME];

                                if (!std::isnan(mapping(i_search_forecast_time, i_flt))) {
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
            }
        }

    } else {
#if defined(_OPENMP)
#pragma omp parallel for default(none) schedule(dynamic) collapse(4) \
shared(data_observations, sims, num_members, mapping, analogs, num_test_stations, \
i_parameter, num_test_times, num_flts, max_members, \
extend_observations, test_stations_index_in_search)
#endif
        for (size_t i_test_station = 0; i_test_station < num_test_stations; i_test_station++) {
            for (size_t i_test_time = 0; i_test_time < num_test_times; i_test_time++) {
                for (size_t i_flt = 0; i_flt < num_flts; i_flt++) {
                    for (size_t i_member = 0; i_member < num_members; i_member++) {
                        
                        if (i_member < max_members) {
                            if (!std::isnan(sims[i_test_station][i_test_time][i_flt][i_member][COL_TAG_SIM::VALUE])) {

                                size_t i_search_station = 0;
                                if (extend_observations) {
                                    i_search_station = (size_t) sims[i_test_station][i_test_time][i_flt][i_member][COL_TAG_SIM::STATION];
                                } else {
                                    i_search_station = test_stations_index_in_search[i_test_station];
                                }

                                size_t i_search_forecast_time = (size_t) sims[i_test_station][i_test_time][i_flt][i_member][COL_TAG_SIM::TIME];

                                if (!std::isnan(mapping(i_search_forecast_time, i_flt))) {
                                    analogs[i_test_station][i_test_time][i_flt][i_member][COL_TAG_ANALOG::STATION] = i_search_station;
                                    analogs[i_test_station][i_test_time][i_flt][i_member][COL_TAG_ANALOG::TIME] = mapping(i_search_forecast_time, i_flt);
                                    analogs[i_test_station][i_test_time][i_flt][i_member][COL_TAG_ANALOG::VALUE] =
                                        data_observations[i_parameter][i_search_station][mapping(i_search_forecast_time, i_flt)];
                                }
                            }
                        }

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
        throw runtime_error("Error code "
                + to_string((long long) indicator)
                + "\nCode reference: https://weiming-hu.github.io/AnalogsEnsemble/CXX/class_an_en.html#a0e256eb89d102d318a47d936b02242bf");
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
    if (method == ONE_TO_MANY || method == ONE_TO_ONE) this->method_ = method;
    else throw runtime_error("Failed to set an unknown method.");
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
AnEn::mean(const std::vector<double>& values, const double max_nan_allowed) const {
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
AnEn::variance(const std::vector<double>& values) const {
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
        const AnEn::TimeMapMatrix mapping,
        size_t i_observation_parameter) const {

    const Forecasts_array & test_forecasts = sims.getTargets();

    size_t num_parameters = test_forecasts.getParametersSize();
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
        if (verbose_ >= 1) {
            cout << BOLDRED << "Error: Standard deviation array has a different shape to search forecasts!" << endl;
            if (sds.shape()[0] != search_forecasts.getParametersSize())
                cout << "-- Number of parameters differs: sds have " << sds.shape()[0]
                    << " and search forecasts have " << search_forecasts.getParametersSize() << endl;
            if (sds.shape()[1] != search_forecasts.getStationsSize())
                cout << "-- Number of stations differs: sds have " << sds.shape()[1]
                   << " and search forecasts have " << search_forecasts.getStationsSize() << endl;
            if (sds.shape()[2] != search_forecasts.getFLTsSize())
                cout << "-- Number of FLTs differs: sds have " << sds.shape()[2]
                   << " and search forecasts have " << search_forecasts.getFLTsSize() << endl;
            cout << RESET << endl;
        }
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

    return (SUCCESS);
}
