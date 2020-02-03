/*
 * File:   testAnEnSSE.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on Aug 4, 2018, 4:09:20 PM
 */

#include "AnEnSSE.h"
#include "Forecasts.h"
#include "Observations.h"
#include "testAnEnSSE.h"
#include "ForecastsPointer.h"
#include "ObservationsPointer.h"

#include "boost/bimap/bimap.hpp"
#include "boost/assign/list_of.hpp"
#include "boost/assign/list_inserter.hpp"
#include "boost/numeric/ublas/matrix_proxy.hpp"

using namespace std;
using namespace boost;
using namespace boost::bimaps;

CPPUNIT_TEST_SUITE_REGISTRATION(testAnEnSSE);

testAnEnSSE::testAnEnSSE() {
}

testAnEnSSE::~testAnEnSSE() {
}

void
testAnEnSSE::testCompute_() {

    /*
     * Test the search space extension algorithm
     */

    /**************************************************************************
     *                           Create Datasets                              *
     **************************************************************************/
    Parameters parameters;
    Stations stations;
    Times fcst_times, flts, obs_times;

    // Manually create 5 parameters
    assign::push_back(parameters.left)
            (0, Parameter("par_1")) // Linear parameter
            (1, Parameter("par_2")) // Parameter that will be assigned with 0 weight
            (2, Parameter("par_3", true)); // Circular parameter

    // Manually create 9 stations
    assign::push_back(stations.left)
            (0, Station(0, 0))
            (1, Station(0, 1))
            (2, Station(0, 2))
            (3, Station(1, 0))
            (4, Station(1, 1))
            (5, Station(1, 2))
            (6, Station(2, 0))
            (7, Station(2, 1))
            (8, Station(2, 2));

    // Manually create 20 forecast times
    for (size_t i = 0; i < 20; ++i) {
        fcst_times.push_back(Times::value_type(i, Time(i * 100)));
    }

    // Manually create 3 FLTs
    assign::push_back(flts.left)(0, Time(0))(1, Time(50))(0, Time(100));

    // Manually create 50 observation times
    for (size_t i = 0; i < 50; ++i) {
        obs_times.push_back(Times::value_type(i, Time(i * 50)));
    }

    // Create forecasts and observations
    ForecastsPointer fcsts(parameters, stations, fcst_times, flts);
    ObservationsPointer obs(parameters, stations, obs_times);
    
    // Randomize forecasts
    for (size_t par_i = 0; par_i < fcsts.getParameters().size(); ++par_i) {
        for (size_t time_i = 0; time_i < fcsts.getTimes().size(); ++time_i) {
            for (size_t flt_i = 0; flt_i < fcsts.getFLTs().size(); ++flt_i) {
                for (size_t sta_i = 0; sta_i < fcsts.getStations().size(); ++sta_i) {
                    double value = (rand() % 10000) / 100.0;
                    fcsts.setValue(value, par_i, sta_i, time_i, flt_i);
                }
            }
        }
    }

    // Randomize observations
    for (size_t par_i = 0; par_i < obs.getParameters().size(); ++par_i) {
        for (size_t sta_i = 0; sta_i < obs.getStations().size(); ++sta_i) {
            for (size_t time_i = 0; time_i < obs.getTimes().size(); ++time_i) {
                    obs.setValue((rand() % 10000) / 100.0, par_i, sta_i, time_i);
            }
        }
    }
    
    /**************************************************************************
     *                             Run AnEnSSE                                *
     **************************************************************************/
    Config config;
    config.extend_obs = true;
    config.num_sims = 30;
    config.num_nearest = 9;
    config.distance = 1;
    config.save_sims = true;
    config.save_sims_day_index = true;
    config.save_sims_station_index = true;
    config.save_analogs = true;
    config.save_analogs_day_index = true;
    config.weights = {1, 0, 1};

    // Define test indices
    vector<size_t> fcsts_test_index = {19};
    vector<size_t> fcsts_search_index(19);
    iota(fcsts_search_index.begin(), fcsts_search_index.end(), 0);
    
    // Run AnEnSSE
    AnEnSSE anen(config);
    anen.compute(fcsts, obs, fcsts_test_index, fcsts_search_index);
    const Array4DPointer & sims = anen.getSimsValue();
    const Array4DPointer & sims_time_index = anen.getSimsTimeIndex();
    const Array4DPointer & sims_station_index = anen.getSimsStationIndex();
    const Array4DPointer & analogs = anen.getAnalogsValue();
    const Array4DPointer & analogs_time_index = anen.getAnalogsTimeIndex();
    const Functions::Matrix & search_stations_index = anen.getSearchStationsIndex();

    /***************************************************************************
     *                              Check results                              *
     **************************************************************************/
    CPPUNIT_ASSERT(sims.shape()[3] == config.num_sims);
    CPPUNIT_ASSERT(sims_time_index.shape()[3] == config.num_sims);
    CPPUNIT_ASSERT(sims_station_index.shape()[3] == config.num_sims);
    CPPUNIT_ASSERT(analogs.shape()[3] == config.num_analogs);
    CPPUNIT_ASSERT(analogs_time_index.shape()[3] == config.num_analogs);
    CPPUNIT_ASSERT(search_stations_index.size1() == stations.size());
    CPPUNIT_ASSERT(search_stations_index.size2() == config.num_nearest);

    // Test that analogs values are directly taken from the corresponding observations
    for (size_t station_i = 0; station_i < analogs.shape()[0]; ++station_i) {
        for (size_t test_i = 0; test_i < analogs.shape()[1]; ++test_i) {
            for (size_t flt_i = 0; flt_i < analogs.shape()[2]; ++flt_i) {
                for (size_t member_i = 0; member_i < analogs.shape()[3]; ++member_i) {
                    
                    // Get the current analog values
                    double analog_value = analogs.getValue(station_i, test_i, flt_i, member_i);
                    
                    // Get the corresponding observation value
                    size_t obs_station_index = sims_station_index.getValue(station_i, test_i, flt_i, member_i);
                    size_t obs_time_index = analogs_time_index.getValue(station_i, test_i, flt_i, member_i);
                    double obs_value = obs.getValue(config.obs_var_index, obs_station_index, obs_time_index);
                    
                    // These two values should be the same
                    CPPUNIT_ASSERT(obs_value == analog_value);
                }
            }
        }
    }
    
    // Test that all the similarity stations index should exist in search stations index
    for (size_t station_i = 0; station_i < sims.shape()[0]; ++station_i) {
        const boost::numeric::ublas::matrix_row<const Functions::Matrix> row(search_stations_index, station_i);

        for (size_t test_i = 0; test_i < sims.shape()[1]; ++test_i) {
            for (size_t flt_i = 0; flt_i < sims.shape()[2]; ++flt_i) {
                for (size_t member_i = 0; member_i < sims.shape()[3]; ++member_i) {
                    size_t neighbor_index = sims_station_index.getValue(station_i, test_i, flt_i, member_i);
                    
                    if (std::isnan(neighbor_index)) continue;
                    
                    CPPUNIT_ASSERT(row.end() != find(row.begin(), row.end(), neighbor_index));
                }
            }
        }
    }

    return;
}