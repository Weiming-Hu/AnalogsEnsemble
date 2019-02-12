/*
 * File:   testFunctions.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on Feb 7, 2019, 2:28:24 PM
 */

#include "testFunctions.h"
#include "../../CAnEnIO/include/AnEnIO.h"
#include "../../CAnEn/include/Functions.h"
#include "../../CAnEn/include/colorTexts.h"

#include <vector>
#include <iostream>
#include <boost/numeric/ublas/io.hpp>

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION(testFunctions);

testFunctions::testFunctions() {
}

testFunctions::~testFunctions() {
}

void testFunctions::setUp() {
}

void testFunctions::tearDown() {
}

void testFunctions::testComputeStandardDeviation() {

    /**
     * Test the function computeStandardDeviation.
     */

    Functions functions(2);

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
    functions.computeStandardDeviation(forecasts, sds);

    for (auto p = sds.data(); p != sds.data() + sds.num_elements(); p++) {
        CPPUNIT_ASSERT((int) (*p * 1000) == 18165);
    }
}

void testFunctions::testComputeSearchWindows() {

    /**
     * Test the function computeSearchWindows().
     */

    boost::numeric::ublas::matrix<size_t> test;
    size_t num_flts = 5, half_window_size = 2;

    Functions functions(2);
    functions.computeSearchWindows(test, num_flts, half_window_size);

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

void testFunctions::testSdCircular() {

    /**
     * Test the behavior of sdCircular function with and without NAN values.
     */

    Functions functions(2);
    vector<double> values;

    values = {1, 2, 3, 4, 5};
    CPPUNIT_ASSERT((int) (functions.sdCircular(values) * 1000) == 1414);

    values = {359, 360, 1, 2, 3};
    CPPUNIT_ASSERT((int) (functions.sdCircular(values) * 1000) == 1414);

    values = {1, 2, 3, NAN, 4, 5};
    CPPUNIT_ASSERT((int) (functions.sdCircular(values) * 1000) == 1414);

    values = {NAN, NAN, NAN, NAN};
    CPPUNIT_ASSERT(std::isnan(functions.sdCircular(values)));

    values = {1, 3, 5, NAN, 2, 4, NAN};
    CPPUNIT_ASSERT((int) (functions.sdCircular(values) * 1000) == 1414);

    values = {359, NAN, 360, 1, NAN, 2, 3};
    CPPUNIT_ASSERT((int) (functions.sdCircular(values) * 1000) == 1414);
}

void testFunctions::testSdLinear() {

    /**
     * Test the behavior of sdLinear function with and without NAN values.
     */

    Functions functions(2);
    vector<double> values;

    values = {1, 2, 3, 4, 5};
    CPPUNIT_ASSERT((int) (functions.sdLinear(values) * 1000) == 1581);

    values = {1, 350, 359, 4, 5};
    CPPUNIT_ASSERT((int) (functions.sdLinear(values) * 100) == 19237);

    values = {1, 2, 3, NAN, 4, 5};
    CPPUNIT_ASSERT((int) (functions.sdLinear(values) * 1000) == 1581);

    values = {NAN, NAN, NAN, NAN};
    CPPUNIT_ASSERT(std::isnan(functions.sdLinear(values)));

    values = {1, 3, 5, NAN, 2, 4, NAN};
    CPPUNIT_ASSERT((int) (functions.sdLinear(values) * 1000) == 1581);

    values = {350, 359, 2, NAN, 4};
    CPPUNIT_ASSERT((int) (functions.sdLinear(values) * 100) == 20297);
}

void testFunctions::testMean() {

    /**
     * Test the behavior of mean function.
     */

    Functions functions(2);

    vector<double> v1 = {1, 2, 3}, v2 = {1, NAN, 2},
    v3 = {NAN, 1, NAN}, v4 = {NAN, NAN, NAN};

    CPPUNIT_ASSERT(functions.mean(v1) == 2);
    CPPUNIT_ASSERT(functions.mean(v2) == 1.5);
    CPPUNIT_ASSERT(functions.mean(v3) == 1);
    CPPUNIT_ASSERT(std::isnan(functions.mean(v4)));

    CPPUNIT_ASSERT(std::isnan(functions.mean(v2, 0)));
    CPPUNIT_ASSERT(std::isnan(functions.mean(v3, 1)));
    CPPUNIT_ASSERT(functions.mean(v3, 2) == 1);
}

void testFunctions::testComputeObservationTimeIndices1() {

    /**
     * Test the function of computeObservationTimeIndices().
     */

    anenTime::Times times_forecasts;
    times_forecasts.insert(times_forecasts.end(),{100, 200, 300, 600});

    anenTime::Times flts_forecasts;
    flts_forecasts.insert(flts_forecasts.end(),{0, 1, 2, 3, 4, 5, 6, 7, 8, 9});

    vector<double> values(700);
    iota(values.begin(), values.end(), 90);
    anenTime::Times times_observations;
    times_observations.insert(times_observations.end(),
            values.begin(), values.end());

    Functions functions(2);
    Functions::TimeMapMatrix mapping;

    functions.computeObservationsTimeIndices(times_forecasts, flts_forecasts,
            times_observations, mapping);

    size_t pos = 0;
    vector<size_t> results{
        10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
        110, 111, 112, 113, 114, 115, 116, 117, 118, 119,
        210, 211, 212, 213, 214, 215, 216, 217, 218, 219,
        510, 511, 512, 513, 514, 515, 516, 517, 518, 519};

    for (size_t i_row = 0; i_row < mapping.size1(); i_row++) {
        for (size_t i_col = 0; i_col < mapping.size2(); i_col++, pos++) {
            CPPUNIT_ASSERT(mapping(i_row, i_col) == results[pos]);
        }
    }
}

void testFunctions::testComputeObservationTimeIndices2() {

    /**
     * Test the function of computeObservationTimeIndices() by reading files.
     */

    AnEnIO io("Read", file_forecasts, "Forecasts", 2);
    Forecasts_array forecasts;
    io.readForecasts(forecasts);

    io.setFilePath(file_observations);
    io.setFileType("Observations");
    Observations_array observations;
    io.readObservations(observations);

    Functions functions(2);
    Functions::TimeMapMatrix mapping;

    functions.computeObservationsTimeIndices(
            forecasts.getTimes(), forecasts.getFLTs(),
            observations.getTimes(), mapping, 0);

    cout << "Forecasts times: " << forecasts.getTimes()
        << "Forecasts FLTs: " << forecasts.getFLTs()
        << "Observations times: " << observations.getTimes()
        << "Mapping: " << endl << mapping << endl;

    size_t i_start = 0;
    for (size_t i_row = 0; i_row < mapping.size1(); i_row++) {
        for (size_t i_col = 0; i_col < mapping.size2(); i_col++) {
            CPPUNIT_ASSERT(mapping(i_row, i_col) == i_start);
            i_start++;
        }
    }
}

void testFunctions::testConvertToIndex() {
    
    /**
     * Test the function of convertToIndex().
     */
    
    vector<double> vec_targets = {1, 4, 7, 3, 9},
            vec_container = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
    anenTime::Times targets, container;
    targets.insert(targets.end(), vec_targets.begin(), vec_targets.end());
    container.insert(container.end(), vec_container.begin(), vec_container.end());
    
    vector<size_t> indexes, results = {0, 3, 6, 2, 8};

    Functions functions(2);
    functions.convertToIndex(targets, container, indexes);
    
    for (size_t i = 0; i < targets.size(); i++) {
        CPPUNIT_ASSERT(indexes[i] == results[i]);
    }
    
    // Catch the exception
    targets.clear();
    vec_targets = {5, 3, 10};
    results = {4, 2, 9};
    targets.insert(targets.end(), vec_targets.begin(), vec_targets.end());
    functions.convertToIndex(targets, container, indexes);
    for (size_t i = 0; i < targets.size(); i++) {
        CPPUNIT_ASSERT(indexes[i] == results[i]);
    }
}
