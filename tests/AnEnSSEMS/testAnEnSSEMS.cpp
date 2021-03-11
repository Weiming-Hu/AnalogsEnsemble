/*
 * File:   testAnEnSSEMS.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on Mar. 10, 2021, 17:13
 */
#include <numeric>

#include "AnEnSSEMS.h"
#include "Forecasts.h"
#include "Observations.h"
#include "testAnEnSSEMS.h"
#include "ForecastsPointer.h"
#include "ObservationsPointer.h"

#include "boost/bimap/bimap.hpp"
#include "boost/assign/list_of.hpp"
#include "boost/assign/list_inserter.hpp"
#include "boost/numeric/ublas/matrix_proxy.hpp"

using namespace std;
using namespace boost;
using namespace boost::bimaps;

CPPUNIT_TEST_SUITE_REGISTRATION(testAnEnSSEMS);

testAnEnSSEMS::testAnEnSSEMS() {
}

testAnEnSSEMS::~testAnEnSSEMS() {
}

void
testAnEnSSEMS::setUp() {
    CPPUNIT_ASSERT(parameters_.size() == 0);
    CPPUNIT_ASSERT(obs_stations_.size() == 0);
    CPPUNIT_ASSERT(fcst_stations_.size() == 0);
    CPPUNIT_ASSERT(fcst_times_.size() == 0);
    CPPUNIT_ASSERT(obs_times_.size() == 0);
    CPPUNIT_ASSERT(flts_.size() == 0);
    return;
}

void
testAnEnSSEMS::tearDown() {
    parameters_.clear();
    obs_stations_.clear();
    fcst_stations_.clear();
    fcst_times_.clear();
    obs_times_.clear();
    flts_.clear();
    config_.reset();
    return;
}

void
testAnEnSSEMS::setUpAnEn() {
    setUp();

    // Manually create parameters
    assign::push_back(parameters_.left)
            (0, Parameter("par_1")) // Linear parameter
            (1, Parameter("par_2")) // Parameter with 0 weight
            (2, Parameter("par_3", true)); // Circular parameter

    // Manually create stations
    assign::push_back(obs_stations_.left)
            (0, Station(1.5, 0))
            (1, Station(0, 1.5));

    assign::push_back(fcst_stations_.left)
            (0, Station(2, 0))
            (1, Station(0, 0))
            (2, Station(0, 2, "station 3"));

    // Manually create 20 forecast times
    for (size_t i = 0; i < 20; ++i) {
        fcst_times_.push_back(Time(i * 100));
    }

    // Manually create 3 FLTs
    assign::push_back(flts_.left)(0, Time(0))(1, Time(50))(0, Time(100));

    // Manually create 50 observation times
    for (size_t i = 0; i < 50; ++i) {
        obs_times_.push_back(i * 50);
    }

    config_.num_analogs = 5;
    config_.operation = false;
    config_.prevent_search_future = true;
    config_.save_analogs = true;
    config_.save_analogs_time_index = true;
    config_.save_sims = true;
    config_.save_sims_time_index = true;
    config_.save_sims_station_index = true;
    config_.verbose = Verbose::Warning;
    config_.quick_sort = false;
    config_.flt_radius = 1;
    config_.max_par_nan = parameters_.size();
    config_.max_flt_nan = flts_.size();
    config_.extend_obs = false;
    config_.num_nearest = 2;

    return;
}

void
testAnEnSSEMS::testAnEn_() {

    /*
     * Test analog generation of AnEnSSEMS. Compare results with AnEnSSE.
     */
    setUpAnEn();
    coreProcedures_();
    tearDown();
}

void
testAnEnSSEMS::testOperation_() {

    /*
     * Test analog generation of AnEnSSEMS with operational mode ON. Compare results with AnEnSSE.
     */
    setUpAnEn();
    config_.operation = true;
    coreProcedures_();
    tearDown();
}

void
testAnEnSSEMS::testFutureSearch_() {

    /*
     * Test analog generation of AnEnSSEMS with future search. Compare results with AnEnSSE.
     */
    setUpAnEn();
    config_.prevent_search_future = false;
    coreProcedures_();
    tearDown();
}

void
testAnEnSSEMS::testManualMatch_() {

    /*
     * Test analog generation of AnEnSSEMS with and without matching stations specified.
     */
    setUpAnEn();

    ForecastsPointer fcsts(parameters_, fcst_stations_, fcst_times_, flts_);
    ObservationsPointer obs(parameters_, obs_stations_, obs_times_);

    Functions::randomizeForecasts(fcsts, 0);
    Functions::randomizeObservations(obs, 0);

    // Define test indices
    vector<size_t> fcsts_test_index = {13, 15, 19};
    vector<size_t> fcsts_search_index(10);
    iota(fcsts_search_index.begin(), fcsts_search_index.end(), 0);

    AnEnSSEMS anen_sse_ms1(config_);
    AnEnSSEMS anen_sse_ms2(config_);

    anen_sse_ms1.compute(fcsts, obs, fcsts_test_index, fcsts_search_index);
    anen_sse_ms2.compute(fcsts, obs, fcsts_test_index, fcsts_search_index, {0, 2});

    const auto & analogs_1 = anen_sse_ms1.analogs_value();
    const auto & analogs_idx_1 = anen_sse_ms1.analogs_time_index();
    const auto & sims_1 = anen_sse_ms1.sims_metric();
    const auto & sims_idx_1 = anen_sse_ms1.sims_time_index();
    const auto & sims_station_idx_1 = anen_sse_ms1.sims_station_index();

    const auto & analogs_2 = anen_sse_ms2.analogs_value();
    const auto & analogs_idx_2 = anen_sse_ms2.analogs_time_index();
    const auto & sims_2 = anen_sse_ms2.sims_metric();
    const auto & sims_idx_2 = anen_sse_ms2.sims_time_index();
    const auto & sims_station_idx_2 = anen_sse_ms2.sims_station_index();

    CPPUNIT_ASSERT(analogs_1 == analogs_2);
    CPPUNIT_ASSERT(analogs_idx_1 == analogs_idx_2);
    CPPUNIT_ASSERT(sims_1 == sims_2);
    CPPUNIT_ASSERT(sims_idx_1 == sims_idx_2);
    CPPUNIT_ASSERT(sims_station_idx_1 == sims_station_idx_2);

    tearDown();
}

void
testAnEnSSEMS::coreProcedures_() {

    ForecastsPointer fcsts(parameters_, fcst_stations_, fcst_times_, flts_);
    ObservationsPointer obs_more(parameters_, fcst_stations_, obs_times_);
    ObservationsPointer obs_fewer(parameters_, obs_stations_, obs_times_);

    Functions::randomizeForecasts(fcsts, 0);

    for (size_t par_i = 0; par_i < parameters_.size(); ++par_i) {
        for (size_t time_i = 0; time_i < obs_times_.size(); ++time_i) {

            double val = rand() / double(RAND_MAX);
            obs_more.setValue(val, par_i, 0, time_i);
            obs_fewer.setValue(val, par_i, 0, time_i);

            val = rand() / double(RAND_MAX);
            obs_more.setValue(val, par_i, 1, time_i);

            val = rand() / double(RAND_MAX);
            obs_more.setValue(val, par_i, 2, time_i);
            obs_fewer.setValue(val, par_i, 1, time_i);
        }
    }

    // Define test indices
    vector<size_t> fcsts_test_index = {13, 15, 19};
    vector<size_t> fcsts_search_index(10);
    iota(fcsts_search_index.begin(), fcsts_search_index.end(), 0);

    AnEnSSE anen_sse(config_);
    anen_sse.compute(fcsts, obs_more, fcsts_test_index, fcsts_search_index);

    fcsts_search_index.resize(10);
    iota(fcsts_search_index.begin(), fcsts_search_index.end(), 0);

    AnEnSSEMS anen_sse_ms(config_);
    anen_sse_ms.compute(fcsts, obs_fewer, fcsts_test_index, fcsts_search_index);

    const auto & analogs_sse = anen_sse.analogs_value();
    const auto & analogs_sse_ms = anen_sse_ms.analogs_value();

    const auto & analogs_idx_sse = anen_sse.analogs_time_index();
    const auto & analogs_idx_sse_ms = anen_sse_ms.analogs_time_index();

    const auto & sims_sse = anen_sse.sims_metric();
    const auto & sims_sse_ms = anen_sse_ms.sims_metric();

    const auto & sims_idx_sse = anen_sse.sims_time_index();
    const auto & sims_idx_sse_ms = anen_sse_ms.sims_time_index();

    const auto & sims_station_idx_sse = anen_sse.sims_station_index();
    const auto & sims_station_idx_sse_ms = anen_sse_ms.sims_station_index();

    for (size_t i = 0; i < fcsts_test_index.size(); ++i) {
        for (size_t j = 0; j < flts_.size(); ++j) {
            for (size_t k = 0; k < config_.num_analogs; ++k) {
                size_t s = 0;
                CPPUNIT_ASSERT(analogs_sse.getValue(s, i, j, k) == analogs_sse_ms.getValue(s, i, j, k));
                CPPUNIT_ASSERT(analogs_idx_sse.getValue(s, i, j, k) == analogs_idx_sse_ms.getValue(s, i, j, k));
                CPPUNIT_ASSERT(sims_sse.getValue(s, i, j, k) == sims_sse_ms.getValue(s, i, j, k));
                CPPUNIT_ASSERT(sims_idx_sse.getValue(s, i, j, k) == sims_idx_sse_ms.getValue(s, i, j, k));
                CPPUNIT_ASSERT(sims_station_idx_sse.getValue(s, i, j, k) == sims_station_idx_sse_ms.getValue(s, i, j, k));

                s = 2;
                CPPUNIT_ASSERT(analogs_sse.getValue(s, i, j, k) == analogs_sse_ms.getValue(s - 1, i, j, k));
                CPPUNIT_ASSERT(analogs_idx_sse.getValue(s, i, j, k) == analogs_idx_sse_ms.getValue(s - 1, i, j, k));
                CPPUNIT_ASSERT(sims_sse.getValue(s, i, j, k) == sims_sse_ms.getValue(s - 1, i, j, k));
                CPPUNIT_ASSERT(sims_idx_sse.getValue(s, i, j, k) == sims_idx_sse_ms.getValue(s - 1, i, j, k));
                CPPUNIT_ASSERT(sims_station_idx_sse.getValue(s, i, j, k) == sims_station_idx_sse_ms.getValue(s - 1, i, j, k));
            }
        }
    }
}

