/*
 * File:   testFunctions.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on Feb 7, 2019, 2:28:24 PM
 */

#include "testFunctions.h"
#include "Functions.h"

#include <vector>
#include <iostream>

#include "boost/bimap/bimap.hpp"
#include "boost/numeric/ublas/io.hpp"
#include "boost/lambda/lambda.hpp"
#include "boost/assign/list_of.hpp"
#include "boost/assign/list_inserter.hpp"
#include "ForecastsArray.h"
#include "AnEnReadNcdf.h"
#include "ObservationsArray.h"

using namespace std;
using namespace boost::bimaps;
using namespace boost;

CPPUNIT_TEST_SUITE_REGISTRATION(testFunctions);

testFunctions::testFunctions() {
}

testFunctions::~testFunctions() {
}

void testFunctions::setUp() {
}

void testFunctions::tearDown() {
}

void testFunctions::testSdCircular_() {

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

void testFunctions::testSdLinear_() {

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

void testFunctions::testMean_() {

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

void testFunctions::testComputeObservationTimeIndices1_() {

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
        flts_forecasts.push_back(Times::value_type(i, Time(i)));
    }

    vector<double> values(700);
    iota(values.begin(), values.end(), 90);
    Times times_observations;
    for (size_t i = 0; i < values.size(); ++i) {
        times_observations.push_back(Times::value_type(i, Time(values[i])));
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

void testFunctions::testComputeObservationTimeIndices2_() {

    /**
     * Test the function of updateTimeTable() by reading files.
     */

    AnEnReadNcdf io;
    
    ForecastsArray forecasts;
    io.readForecasts(file_forecasts, forecasts);
    
    ObservationsArray observations;
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

void testFunctions::testConvertToIndex_() {
    
    /**
     * Test the function of toIndex().
     */
    
    Parameters parameters;
    assign::push_back(parameters.left)
            (0, Parameter())
            (1, Parameter("par_1", 0.3))
            (1, Parameter("par_1"))
            (2, Parameter("par_2"))
            (2, Parameter("par_2", 0.2, true));
    
    Parameter p0, p1("par_1", 0.3), p2("par_2");
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
