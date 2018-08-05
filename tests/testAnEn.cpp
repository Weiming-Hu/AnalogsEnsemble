/*
 * File:   testAnEn.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on Aug 4, 2018, 4:09:20 PM
 */

#include "testAnEn.h"

#include <numeric>
#include <cppunit/TestAssert.h>

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION(testAnEn);

testAnEn::testAnEn() {
}

testAnEn::~testAnEn() {
}

void testAnEn::testComputeStandardDeviation() {

    /**
     * Test the function computeStandardDeviation.
     */

    AnEn anen(1);

    anenSta::Station s1, s2("Hunan", 10, 20);
    anenSta::Stations stations;
    stations.insert(stations.end(),{s1, s2});

    anenPar::Parameter p1, p2("temperature"), p3("humidity");
    anenPar::Parameters parameters;
    parameters.insert(parameters.end(),{p1, p2, p3});

    anenTime::Times times;
    times.insert(times.end(),{1, 2, 3, 4, 5, 6, 7, 8, 9, 10});

    anenTime::FLTs flts;
    flts.insert(flts.end(),{100, 200, 300, 400, 500});

    vector<double> values(parameters.size()
            * stations.size() * times.size() * flts.size());
    iota(values.begin(), values.end(), 0);

    Forecasts_array forecasts(parameters, stations, times, flts, values);

    StandardDeviation sds;
    anen.computeStandardDeviation(forecasts, sds);

    for (auto p = sds.data(); p != sds.data() + sds.num_elements(); p++) {
        CPPUNIT_ASSERT((int) (*p * 1000) == 18165);
    }
}

void testAnEn::testComputeSearchWindows() {

    /**
     * Test the function computeSearchWindows().
     */

    boost::numeric::ublas::matrix<size_t> test;
    size_t num_flts = 5, half_window_size = 2;

    AnEn anen;
    anen.computeSearchWindows(test, num_flts, half_window_size);

    CPPUNIT_ASSERT(test(0, 0) == 0);
    CPPUNIT_ASSERT(test(0, 1) == 2);
    CPPUNIT_ASSERT(test(1, 0) == 0);
    CPPUNIT_ASSERT(test(1, 1) == 3);
    CPPUNIT_ASSERT(test(2, 0) == 0);
    CPPUNIT_ASSERT(test(2, 1) == 4);
    CPPUNIT_ASSERT(test(3, 0) == 1);
    CPPUNIT_ASSERT(test(3, 1) == 4);
    CPPUNIT_ASSERT(test(4, 0) == 2);
    CPPUNIT_ASSERT(test(4, 1) == 4);
}

void testAnEn::testComputeSimilarity() {

    /**
     * Test the function computeSimilarity().
     * 
     * The data contains no NAN values. This test is designed for correctness.
     */

    // tick is created for assigning values
    double tick;

    // Construct meta information
    anenSta::Station s1;

    anenPar::Parameter p1;
    anenPar::Parameters parameters;
    parameters.insert(parameters.end(),{p1});

    anenTime::FLTs flts;
    flts.insert(flts.end(),{1, 2, 3, 4});

    vector<double> values;

    // Construct test forecasts
    anenSta::Stations test_stations;
    test_stations.insert(test_stations.end(),{s1});
    anenTime::Times test_times;
    test_times.insert(test_times.end(),{800, 900});
    values.resize(parameters.size() * test_stations.size() *
            test_times.size() * flts.size());
    tick = 0.0;
    for (auto & value : values) {
        value = tick * 10;
        tick++;
    }
    Forecasts_array test_forecasts(parameters,
            test_stations, test_times, flts, values);

    // Construct search forecasts
    anenSta::Stations search_stations;
    search_stations.insert(search_stations.end(),{s1});
    anenTime::Times search_times;
    search_times.insert(search_times.end(),{
        100, 200, 300, 400, 500, 600, 700
    });
    values.resize(parameters.size() * search_stations.size() *
            search_times.size() * flts.size());
    tick = 200.0;
    for (auto & value : values) {
        value = tick / 10;
        tick -= 2;
    }
    Forecasts_array search_forecasts(parameters,
            search_stations, search_times, flts, values);

    // Construct search observations
    anenTime::Times search_observation_times;
    search_observation_times.insert(search_observation_times.end(),{
        101, 102, 103, 104, 201, 202, 203, 204, 301, 302, 303, 304,
        401, 402, 403, 404, 501, 502, 503, 504, 601, 602, 603, 604,
        701, 702, 703, 704
    });
    tick = 20.0;
    for (auto & value : values) {
        value = tick / 2;
        tick += 2;
    }
    values.resize(parameters.size() * search_stations.size() *
            search_observation_times.size());
    Observations_array search_observations(parameters,
            search_stations, search_observation_times, values);

    // Construct AnEn object
    AnEn io(4);

    // Construct SimilarityMatrices
    SimilarityMatrices sims(test_forecasts);

    // Construct standard deviation
    StandardDeviation sds(parameters.size(),
            search_stations.size(), flts.size());
    io.computeStandardDeviation(search_forecasts, sds);

    // Compute similarity
    io.computeSimilarity(search_forecasts, sds, sims, search_observations);

    vector<double> results{
        32.81278, 32.51099, 32.21302, 31.91898, 31.62899, 31.34315, 31.06157,
        40.57181, 40.61140, 40.65622, 40.70626, 40.76150, 40.82191, 40.88748,
        66.48630, 66.85432, 67.22351, 67.59385, 67.96533, 68.33792, 68.71161,
        81.39652, 81.83760, 82.27892, 82.72048, 83.16228, 83.60430, 84.04654,
        24.81863, 24.85314, 24.89621, 24.94780, 25.00786, 25.07631, 25.15310,
        48.28857, 48.54306, 48.80061, 49.06119, 49.32473, 49.59119, 49.86052,
        86.01744, 86.42627, 86.83564, 87.24555, 87.65598, 88.06694, 88.47841,
        103.6841, 104.1337, 104.5834, 105.0332, 105.4831, 105.9331, 106.3832};

    size_t index = 0;
    for (size_t i_station = 0; i_station < sims.shape()[0]; i_station++) {
        for (size_t i_time = 0; i_time < sims.shape()[1]; i_time++) {
            for (size_t i_flt = 0; i_flt < sims.shape()[2]; i_flt++) {
                for (size_t i_search_time = 0;
                        i_search_time < search_times.size();
                        i_search_time++, index++) {
                    CPPUNIT_ASSERT((int) (sims[i_station][i_time][i_flt](
                            i_search_time, SimilarityMatrix::TAG::VALUE) * 100)
                            == (int) (results[index] * 100));
                }
            }
        }
    }
}