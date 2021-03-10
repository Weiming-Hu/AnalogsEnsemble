/*
 * File:   testFunctions.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on Feb 7, 2019, 2:28:24 PM
 */

#include <vector>
#include <numeric>
#include <iostream>

#include "AnEnReadNcdf.h"
#include "testFunctions.h"
#include "ForecastsPointer.h"
#include "ObservationsPointer.h"

#include "boost/bimap/bimap.hpp"
#include "boost/numeric/ublas/io.hpp"
#include "boost/assign/list_of.hpp"
#include "boost/assign/list_inserter.hpp"
#include "boost/numeric/ublas/matrix_proxy.hpp"
#include "boost/lambda/lambda.hpp"

using namespace std;
using namespace boost::bimaps;
using namespace boost;

CPPUNIT_TEST_SUITE_REGISTRATION(testFunctions);

testFunctions::testFunctions() {
}

testFunctions::~testFunctions() {
}

void
testFunctions::setUp() {
}

void
testFunctions::tearDown() {
}

void
testFunctions::testSearchStations_() {

    /**
     * Tests the functionality to find nearest neighbors with and without
     * setting a distance threshold.
     * 
     * The spatial distribution of stations looks like below. The numbers
     * shown on the diagram are the index of each station.
     * 
     *           (Y)  ^
     *              8 |             13
     *              7 |       11          12
     *              6 |    10
     *              5 |     7  8  9
     *              4 |        5        6
     *              3 |  2        3           4
     *              2 |
     *              1 |  0              1
     *                .-------------------------->
     *                0  1  2  3  4  5  6  7  8  (X)
     */

    // Create stations based on the above diagram
    Station s0(1, 1), s1(6, 1), s2(1, 3),
            s3(4, 3), s4(8, 3), s5(3, 4),
            s6(6, 4), s7(2, 5), s8(3, 5),
            s9(4, 5), s10(2, 6), s11(3, 7),
            s12(7, 7), s13(5, 8);
    Stations stations;

    assign::push_back(stations.left)(0, s0)(1, s1)(2, s2)(3, s3)(4, s4)(5, s5)
            (6, s6)(7, s7)(8, s8)(9, s9)(10, s10)(11, s11)(12, s12)(13, s13);
    
    /***************************************************************************
     *          Test the search station without a distance threshold           *
     **************************************************************************/

    // Define how many neighbors to find
    size_t num_nearest = 2;

    // Define the search stations index table
    Functions::Matrix table_nn(stations.size(), num_nearest);

    // Compute search stations without distance threshold
    Functions::setSearchStations(stations, table_nn, NAN);

    // Results should have the station itself as search
    for (size_t station_i = 0; station_i < stations.size(); ++station_i) {
        neighborExists_(table_nn, station_i, station_i);
    }

    CPPUNIT_ASSERT(neighborExists_(table_nn, 0, 2));
    CPPUNIT_ASSERT(neighborExists_(table_nn, 5, 8));
    CPPUNIT_ASSERT(neighborExists_(table_nn, 11, 10));
    CPPUNIT_ASSERT(neighborExists_(table_nn, 2, 0));
    CPPUNIT_ASSERT(neighborExists_(table_nn, 4, 6));
    
    cout << "Search stations are expected without a distance threshold." << endl;

    /***************************************************************************
     *            Test the search station with a distance threshold            *
     **************************************************************************/

    // Define a distance threshold
    double distance = 1;

    // Define the search stations index table
    Functions::Matrix table_dist(stations.size(), num_nearest);
    auto & storage = table_dist.data();
    fill_n(storage.begin(), storage.size(), NAN);

    // Compute search stations without distance threshold
    Functions::setSearchStations(stations, table_dist, distance);
    
    // cout << table_dist << endl;

    // Results should have the station itself as search
    for (size_t station_i = 0; station_i < stations.size(); ++station_i) {
        neighborExists_(table_dist, station_i, station_i);
    }

    CPPUNIT_ASSERT(!neighborExists_(table_dist, 0, 2));
    CPPUNIT_ASSERT(neighborExists_(table_dist, 5, 8));
    CPPUNIT_ASSERT(!neighborExists_(table_dist, 11, 10));
    CPPUNIT_ASSERT(!neighborExists_(table_dist, 2, 0));
    CPPUNIT_ASSERT(!neighborExists_(table_dist, 4, 6));
    
    cout << "Search stations are expected with a distance threshold." << endl;

    return;
}

void
testFunctions::testComputeObservationTimeIndices1_() {

    /**
     * Test the function of updateTimeTable().
     */

    Times times_forecasts;
    assign::push_back(times_forecasts.left)
            (0, Time(100))(1, Time(200))(2, Time(300))(3, Time(600));

    vector<size_t> test_times_index(times_forecasts.size());
    iota(test_times_index.begin(), test_times_index.end(), 0);

    Times flts_forecasts;
    for (size_t i = 0; i < 10; ++i) {
        flts_forecasts.push_back(i);
    }

    vector<double> values(700);
    iota(values.begin(), values.end(), 90);
    Times times_observations;
    for (size_t i = 0; i < values.size(); ++i) {
        times_observations.push_back(values[i]);
    }

    Functions::Matrix mapping(times_forecasts.size(), flts_forecasts.size());
    Functions::updateTimeTable(times_forecasts, test_times_index,
            flts_forecasts, times_observations, mapping);

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

void
testFunctions::testComputeObservationTimeIndices2_() {

    /**
     * Test the function of updateTimeTable() by reading files.
     */

    AnEnReadNcdf io;

    cout << "Reading forecasts ..." << endl;
    ForecastsPointer forecasts;
    io.readForecasts(file_forecasts, forecasts);

    cout << "Reading observations ..." << endl;
    ObservationsPointer observations;
    io.readObservations(file_observations, observations);

    vector<size_t> test_times_index(forecasts.getTimes().size());
    iota(test_times_index.begin(), test_times_index.end(), 0);

    Functions::Matrix mapping(test_times_index.size(),
            forecasts.getFLTs().size());
    Functions::updateTimeTable(forecasts.getTimes(), test_times_index,
            forecasts.getFLTs(), observations.getTimes(), mapping);

    size_t i_start = 0;
    for (size_t i_row = 0; i_row < mapping.size1(); i_row++) {
        for (size_t i_col = 0; i_col < mapping.size2(); i_col++) {
            CPPUNIT_ASSERT(mapping(i_row, i_col) == i_start);
            i_start++;
        }
    }
}

void
testFunctions::testCollapseLeadTimes_() {

    /**
     * This function tests the conversion from forecasts to observations
     */
    // Parameters
    Parameters parameters;
    Parameter p1, p2("temperature"), p3("Direction"), p4("wind direction", true);
    assign::push_back(parameters.left)(0, p1)(1, p2)(2, p3)(3, p4);

    // Stations
    Stations stations;
    Station s0, s1(5, 3), s2(10, 20, "station3"), s3(10, 20, "station2");
    assign::push_back(stations.left)(0, s0)(1, s1)(2, s2)(3, s3);

    // Times and FLTs
    Times times, flts;
    assign::push_back(times.left)(0, 0)(1, 6)(2, 12);
    assign::push_back(flts.left)(0, 0)(1, 3)(2, 6)(3, 9)(4, 12);

    // Forecasts
    ForecastsPointer forecasts(parameters, stations, times, flts);

    double* p_data = forecasts.getValuesPtr();
    for (size_t value_i = 0; value_i < forecasts.num_elements(); ++value_i) {
        p_data[value_i] = value_i;
    }

    cout << "Forecasts initialized" << endl;

    // Call the method
    ObservationsPointer observations;
    Functions::collapseLeadTimes(observations, forecasts);

    cout << "Forecasts collapsed to observations" << endl;

    // Check results
    double obs, fcst;
    CPPUNIT_ASSERT(observations.num_elements() == 4 * 4 * 9);

    for (size_t parameter_i = 0; parameter_i < parameters.size(); ++parameter_i) {
        for (size_t station_i = 0; station_i < stations.size(); ++station_i) {
            for (size_t time_i = 0; time_i < observations.getTimes().size(); ++time_i) {
                obs = observations.getValue(parameter_i, station_i, time_i);

                /*
                 * If you fail this test, you need to check whether your algorithm
                 * prefers values that are from an earlier forecast lead time
                 */
                if (time_i == 0) fcst = forecasts.getValue(parameter_i, station_i, 0, 0);
                else if (time_i == 1) fcst = forecasts.getValue(parameter_i, station_i, 0, 1);
                else if (time_i == 2) fcst = forecasts.getValue(parameter_i, station_i, 1, 0);
                else if (time_i == 3) fcst = forecasts.getValue(parameter_i, station_i, 1, 1);
                else if (time_i == 4) fcst = forecasts.getValue(parameter_i, station_i, 2, 0);
                else if (time_i == 5) fcst = forecasts.getValue(parameter_i, station_i, 2, 1);
                else if (time_i == 6) fcst = forecasts.getValue(parameter_i, station_i, 2, 2);
                else if (time_i == 7) fcst = forecasts.getValue(parameter_i, station_i, 2, 3);
                else if (time_i == 8) fcst = forecasts.getValue(parameter_i, station_i, 2, 4);
                else throw runtime_error("Wrong time index");

                CPPUNIT_ASSERT(obs == fcst);

            }
        }
    }
}

void
testFunctions::testUnwrapTimeSeries_() {
    /**
     * This function tests unwrapping a time series
     */
    // Parameters
    Parameters parameters;
    Parameter p1;
    assign::push_back(parameters.left)(0, p1);

    // Stations
    Stations stations;
    Station s0;
    assign::push_back(stations.left)(0, s0);

    // Times and FLTs
    Times times, flts1, flts2, ts;
    assign::push_back(times.left)(0, 0)(1, 4);

    assign::push_back(flts1.left)(0, 0)(1, 2);
    assign::push_back(flts2.left)(0, 0)(1, 4);

    assign::push_back(ts.left)(0, 0)(1, 1)(2, 2)(3, 3)(4, 4)(5, 5)(6, 6)(7, 7);

    // Observations
    ObservationsPointer observations(parameters, stations, ts);

    double* p_data = observations.getValuesPtr();
    for (size_t value_i = 0; value_i < observations.num_elements(); ++value_i) {
        p_data[value_i] = value_i;
    }

    cout << "observations initialized" << endl;

    // Call the method
    ForecastsPointer forecasts;
    Functions::unwrapTimeSeries(forecasts, times, flts1, observations);
    CPPUNIT_ASSERT(forecasts.getValue(0, 0, 0, 0) == observations.getValue(0, 0, 0));
    CPPUNIT_ASSERT(forecasts.getValue(0, 0, 0, 1) == observations.getValue(0, 0, 2));
    CPPUNIT_ASSERT(forecasts.getValue(0, 0, 1, 0) == observations.getValue(0, 0, 4));
    CPPUNIT_ASSERT(forecasts.getValue(0, 0, 1, 1) == observations.getValue(0, 0, 6));

    Functions::unwrapTimeSeries(forecasts, times, flts2, observations);
    CPPUNIT_ASSERT(forecasts.getValue(0, 0, 0, 0) == observations.getValue(0, 0, 0));
    CPPUNIT_ASSERT(forecasts.getValue(0, 0, 0, 1) == observations.getValue(0, 0, 4));
    CPPUNIT_ASSERT(forecasts.getValue(0, 0, 1, 0) == observations.getValue(0, 0, 4));
    CPPUNIT_ASSERT(std::isnan(forecasts.getValue(0, 0, 1, 1)));
}

void
testFunctions::testConvertToIndex_() {

    /**
     * Test the function of toIndex().
     */

    Parameters parameters;
    assign::push_back(parameters.left)
            (0, Parameter())
            (1, Parameter("par_1"))
            (1, Parameter("par_1"))
            (2, Parameter("par_2"))
            (2, Parameter("par_2", true));

    Parameter p0, p1("par_1"), p2("par_2");
    Parameters query;
    assign::push_back(query.left)(0, p0)(1, p1)(2, p2);

    vector<size_t> indices;
    Functions::toIndex(indices, query, parameters);

    CPPUNIT_ASSERT(parameters.size() == 3);
    CPPUNIT_ASSERT(indices.size() == 3);
    CPPUNIT_ASSERT(indices[0] == 0);
    CPPUNIT_ASSERT(indices[1] == 1);
    CPPUNIT_ASSERT(indices[2] == 2);
    CPPUNIT_ASSERT(p0 == parameters.left[0].second);
    CPPUNIT_ASSERT(p1 == parameters.left[1].second);
    CPPUNIT_ASSERT(p2 == parameters.left[2].second);
}

void
testFunctions::testSdCircular_() {

    /**
     * Test the behavior of sdCircular function with and without NAN values.
     */

    vector<double> values;

    values = {1, 2, 3, 4, 5};
    CPPUNIT_ASSERT((int) (Functions::sdCircular(values) * 1000) == 1414);

    values = {359, 360, 1, 2, 3};
    CPPUNIT_ASSERT((int) (Functions::sdCircular(values) * 1000) == 1414);

    values = {1, 2, 3, NAN, 4, 5};
    CPPUNIT_ASSERT((int) (Functions::sdCircular(values) * 1000) == 1414);

    values = {NAN, NAN, NAN, NAN};
    CPPUNIT_ASSERT(std::isnan(Functions::sdCircular(values)));

    values = {1, 3, 5, NAN, 2, 4, NAN};
    CPPUNIT_ASSERT((int) (Functions::sdCircular(values) * 1000) == 1414);

    values = {359, NAN, 360, 1, NAN, 2, 3};
    CPPUNIT_ASSERT((int) (Functions::sdCircular(values) * 1000) == 1414);
}

void
testFunctions::testSdLinear_() {

    /**
     * Test the behavior of sdLinear function with and without NAN values.
     */

    vector<double> values;

    values = {1, 2, 3, 4, 5};
    CPPUNIT_ASSERT((int) (Functions::sdLinear(values) * 1000) == 1581);

    values = {1, 350, 359, 4, 5};
    CPPUNIT_ASSERT((int) (Functions::sdLinear(values) * 100) == 19237);

    values = {1, 2, 3, NAN, 4, 5};
    CPPUNIT_ASSERT((int) (Functions::sdLinear(values) * 1000) == 1581);

    values = {NAN, NAN, NAN, NAN};
    CPPUNIT_ASSERT(std::isnan(Functions::sdLinear(values)));

    values = {1, 3, 5, NAN, 2, 4, NAN};
    CPPUNIT_ASSERT((int) (Functions::sdLinear(values) * 1000) == 1581);

    values = {350, 359, 2, NAN, 4};
    CPPUNIT_ASSERT((int) (Functions::sdLinear(values) * 100) == 20297);
}

void
testFunctions::testMean_() {

    /**
     * Test the behavior of mean function.
     */

    vector<double> v1 = {1, 2, 3}, v2 = {1, NAN, 2},
    v3 = {NAN, 1, NAN}, v4 = {NAN, NAN, NAN};

    CPPUNIT_ASSERT(Functions::mean(v1) == 2);
    CPPUNIT_ASSERT(Functions::mean(v2) == 1.5);
    CPPUNIT_ASSERT(Functions::mean(v3) == 1);
    CPPUNIT_ASSERT(std::isnan(Functions::mean(v4)));

    CPPUNIT_ASSERT(std::isnan(Functions::mean(v2, 0)));
    CPPUNIT_ASSERT(std::isnan(Functions::mean(v3, 1)));
    CPPUNIT_ASSERT(Functions::mean(v3, 2) == 1);
}

void
testFunctions::testFindClosest_() {
    
    /*
     * Test the behavior of finding the cloest station
     */
    Station s0(1, 1), s1(2, 1), s2(1.3, NAN), s3(2, 2), s4(100, NAN);

    Stations stations;
    assign::push_back(stations.left)(0, s0)(1, s1)(2, s2)(3, s3)(4, s4);

    CPPUNIT_ASSERT(0 == Functions::findClosest(Station(1.1, 1.1), stations));
    CPPUNIT_ASSERT(1 == Functions::findClosest(Station(2, 0.9), stations));
    CPPUNIT_ASSERT(3 == Functions::findClosest(Station(2, 2), stations));

    bool caught_error = false;

    try {
        Functions::findClosest(Station(0, NAN), stations);
    } catch (runtime_error & e) {
        CPPUNIT_ASSERT(e.what() == string("Target station must have valid coordinates!"));
        caught_error = true;
    }

    CPPUNIT_ASSERT(caught_error);
}

bool
testFunctions::neighborExists_(const Functions::Matrix & table,
        size_t test_index, size_t neighbor_index) const {
    using namespace boost::numeric::ublas;

    const matrix_row<const Functions::Matrix> row(table, test_index);
    return row.end() != find(row.begin(), row.end(), neighbor_index);
}
