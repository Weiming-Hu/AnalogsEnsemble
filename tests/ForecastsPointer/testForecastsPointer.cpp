/*
 * File:   testForecastsPointer.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on Jul 1, 2018, 10:47:47 AM
 */

#include "testForecastsPointer.h"
#include "ForecastsPointer.h"

#include <numeric>
#include <cstdlib>
#include <algorithm>
#include <boost/assign/list_of.hpp>
#include <boost/assign/list_inserter.hpp>

using namespace std;
using namespace boost::bimaps;
using namespace boost;

CPPUNIT_TEST_SUITE_REGISTRATION(testForecastsPointer);

testForecastsPointer::testForecastsPointer() {
}

testForecastsPointer::~testForecastsPointer() {
}

void testForecastsPointer::testCopy_() {
    /**
     * Test the copy assignment of forecasts
     */

    Station s1, s2(10, 20, "Hunan"), s3, s4(30, 40, "Guangdong"),
            s5(15, 23), s6(30, 30, "Beijing");
    Stations stations;
    assign::push_back(stations.left)(0, s1)(1, s2)(2, s3)(3, s4)(4, s5)(5, s6);

    Parameter p1, p2("temperature"), p3("humidity"),
            p4("wind direction", true);

    Parameters parameters;
    assign::push_back(parameters.left)(0, p1)(1, p2)(2, p3)(3, p4);

    Times times;
    for (size_t i = 0; i < 10; ++i) {
        times.push_back(Time(i + 1));
    }

    Times flts;
    assign::push_back(flts.left)
            (0, Time(100))(1, Time(200))(2, Time(300))(3, Time(400));

    vector<double> values(parameters.size() * stations.size() * times.size() * flts.size());
    generate(values.begin(), values.end(), rand);

    ForecastsPointer forecasts(parameters, stations, times, flts);
    double *ptr = forecasts.getValuesPtr();
    memcpy(ptr, values.data(), values.size() * sizeof (double));

    // Copy assignment
    ForecastsPointer forecasts_copy;
    forecasts_copy = forecasts;

    CPPUNIT_ASSERT(forecasts.getParameters() == forecasts_copy.getParameters());
    CPPUNIT_ASSERT(forecasts.getStations() == forecasts_copy.getStations());
    CPPUNIT_ASSERT(forecasts.getTimes() == forecasts_copy.getTimes());
    CPPUNIT_ASSERT(forecasts.getFLTs() == forecasts_copy.getFLTs());
    CPPUNIT_ASSERT(forecasts.num_elements() == forecasts_copy.num_elements());

    for (size_t i = 0; i < forecasts_copy.shape()[0]; ++i) 
        for (size_t j = 0; j < forecasts_copy.shape()[1]; ++j) 
            for (size_t k = 0; k < forecasts_copy.shape()[2]; ++k) 
                for (size_t m = 0; m < forecasts_copy.shape()[3]; ++m) 
                    CPPUNIT_ASSERT(forecasts.getValue(i, j, k, m) == forecasts_copy.getValue(i, j, k, m));
}

void testForecastsPointer::testForecastSetVectorValues_() {

    /**
     * Test the sequence of values stored in the Forecast data type.
     * Data should be set in column-major order.
     */

    Station s1, s2(10, 20, "Hunan"), s3, s4(30, 40, "Guangdong"),
            s5(15, 23), s6(30, 30, "Beijing");
    Stations stations;
    assign::push_back(stations.left)(0, s1)(1, s2)(2, s3)(3, s4)(4, s5)(5, s6);

    Parameter p1, p2("temperature"), p3("humidity"),
            p4("wind direction", true);

    Parameters parameters;
    assign::push_back(parameters.left)(0, p1)(1, p2)(2, p3)(3, p4);

    Times times;
    for (size_t i = 0; i < 10; ++i) {
        times.push_back(Time(i + 1));
    }

    Times flts;
    assign::push_back(flts.left)
            (0, Time(100))(1, Time(200))(2, Time(300))(3, Time(400));

    vector<double> values(parameters.size() * stations.size() *
            times.size() * flts.size());
    iota(values.begin(), values.end(), 0);

    ForecastsPointer forecasts(parameters, stations, times, flts);
    double *ptr = forecasts.getValuesPtr();
    memcpy(ptr, values.data(), values.size() * sizeof (double));

    size_t l = 0;

    /*
     * Make sure the data pointer is mapping to the multi_array by Fortran
     * order layout.
     */
    for (size_t n = 0; n < forecasts.getFLTs().size(); ++n)
        for (size_t m = 0; m < forecasts.getTimes().size(); ++m)
            for (size_t j = 0; j < forecasts.getStations().size(); ++j)
                for (size_t i = 0; i < forecasts.getParameters().size(); ++i, ++l)
                    CPPUNIT_ASSERT(forecasts.getValue(i, j, m, n) == values[l]);
}

void
testForecastsPointer::testSubset_() {
    
    /**
     * Test subset functionality
     */

    Station s1, s2(10, 20, "Hunan"), s3, s4(30, 40, "Guangdong"),
            s5(15, 23), s6(30, 30, "Beijing");
    Stations stations;
    assign::push_back(stations.left)(0, s1)(1, s2)(2, s3)(3, s4)(4, s5)(5, s6);

    Parameter p1, p2("temperature"), p3("humidity"),
            p4("wind direction", true);

    Parameters parameters;
    assign::push_back(parameters.left)(0, p1)(1, p2)(2, p3)(3, p4);

    Times times;
    for (size_t i = 0; i < 10; ++i) {
        times.push_back(Time(i + 1));
    }

    Times flts;
    assign::push_back(flts.left)
            (0, Time(100))(1, Time(200))(2, Time(300))(3, Time(400));

    ForecastsPointer forecasts(parameters, stations, times, flts);

    double * p_data = forecasts.getValuesPtr();
    iota(p_data, p_data + forecasts.num_elements(), 100);
    
    cout << forecasts;

    // Create subset forecasts
    Stations stations_subset;
    stations_subset.push_back(s1);
    stations_subset.push_back(s5);
    stations_subset.push_back(s6);

    Parameters parameters_subset;
    parameters_subset.push_back(p3);
    parameters_subset.push_back(p4);

    Times times_subset;
    times_subset.push_back(1);
    times_subset.push_back(2);
    times_subset.push_back(9);
    times_subset.push_back(10);

    Times flts_subset;
    flts_subset.push_back(300);

    // Create a subset forecasts
    ForecastsPointer forecasts_subset(parameters_subset,
            stations_subset, times_subset, flts_subset);

    // Subset forecasts data
    forecasts.subset(forecasts_subset);

    // Get the subset indices
    vector<size_t> stations_index, parameters_index, times_index, flts_index;

    parameters.getIndices(parameters_subset, parameters_index);
    stations.getIndices(stations_subset, stations_index);
    times.getIndices(times_subset, times_index);
    flts.getIndices(flts_subset, flts_index);

    double subset_value, original_value;

    for (size_t dim0_i = 0; dim0_i < parameters_subset.size(); ++dim0_i)
        for (size_t dim1_i = 0; dim1_i < stations_subset.size(); ++dim1_i)
            for (size_t dim2_i = 0; dim2_i < times_subset.size(); ++dim2_i)
                for (size_t dim3_i = 0; dim3_i < flts_subset.size(); ++dim3_i) {
                    subset_value = forecasts_subset.getValue(dim0_i, dim1_i, dim2_i, dim3_i);
                    original_value = forecasts.getValue(
                            parameters.getIndex(parameters_subset.getParameter(dim0_i)),
                            stations.getIndex(stations_subset.getStation(dim1_i)),
                            times.getIndex(times_subset.getTime(dim2_i)),
                            flts.getIndex(flts_subset.getTime(dim3_i)));

                    cout << subset_value << " " << original_value << endl;
                    CPPUNIT_ASSERT(subset_value == original_value);
                }
}

void
testForecastsPointer::testWind_() {
    
    /**
     * Test wind field calculator
     */

    Station s1, s2(10, 20, "Hunan");
    Stations stations;
    assign::push_back(stations.left)(0, s1)(1, s2);

    Parameter p1("my_V"), p2("angle", true), p3("my_U");
            
    Parameters parameters;
    assign::push_back(parameters.left)(0, p1)(1, p2)(2, p3);

    Times times;
    times.push_back(100);

    Times flts;
    flts.push_back(0);
    flts.push_back(10);
    flts.push_back(20);

    ForecastsPointer forecasts(parameters, stations, times, flts);

    // Assign values manually
    
    // Parameter my_V
    forecasts.setValue(0, 0, 0, 0, 0);
    forecasts.setValue(3, 0, 0, 0, 1);
    forecasts.setValue(3, 0, 0, 0, 2);
    forecasts.setValue(1, 0, 1, 0, 0);
    forecasts.setValue(0, 0, 1, 0, 1);
    forecasts.setValue(-1.5, 0, 1, 0, 2);

    // Parameter angle
    forecasts.setValue(20, 1, 0, 0, 0);
    forecasts.setValue(30, 1, 0, 0, 1);
    forecasts.setValue(40, 1, 0, 0, 2);
    forecasts.setValue(50, 1, 1, 0, 0);
    forecasts.setValue(60, 1, 1, 0, 1);
    forecasts.setValue(70, 1, 1, 0, 2);

    // Parameter my_U
    forecasts.setValue(0.5, 2, 0, 0, 0);
    forecasts.setValue(3, 2, 0, 0, 1);
    forecasts.setValue(0, 2, 0, 0, 2);
    forecasts.setValue(-1, 2, 1, 0, 0);
    forecasts.setValue(-1.5, 2, 1, 0, 1);
    forecasts.setValue(-1.5, 2, 1, 0, 2);

    ForecastsPointer original = forecasts;
    cout << original;

    forecasts.windTransform("my_U", "my_V", "wind_spd", "wind_dir");

    cout << forecasts;

    // Check results
    CPPUNIT_ASSERT(forecasts.getParameters().getParameter(0).getCircular());
    CPPUNIT_ASSERT(forecasts.getParameters().getParameter(1).getCircular());
    CPPUNIT_ASSERT(!forecasts.getParameters().getParameter(2).getCircular());

    CPPUNIT_ASSERT(forecasts.getValue(0, 0, 0, 0) == 0);
    CPPUNIT_ASSERT(forecasts.getValue(0, 0, 0, 1) == 45);
    CPPUNIT_ASSERT(forecasts.getValue(0, 0, 0, 2) == 90);
    CPPUNIT_ASSERT(forecasts.getValue(0, 1, 0, 0) == 135);
    CPPUNIT_ASSERT(forecasts.getValue(0, 1, 0, 1) == 180);
    CPPUNIT_ASSERT(forecasts.getValue(0, 1, 0, 2) == 225);

    // Parameter angle
    CPPUNIT_ASSERT(forecasts.getValue(1, 0, 0, 0) == original.getValue(1, 0, 0, 0));
    CPPUNIT_ASSERT(forecasts.getValue(1, 0, 0, 1) == original.getValue(1, 0, 0, 1));
    CPPUNIT_ASSERT(forecasts.getValue(1, 0, 0, 2) == original.getValue(1, 0, 0, 2));
    CPPUNIT_ASSERT(forecasts.getValue(1, 1, 0, 0) == original.getValue(1, 1, 0, 0));
    CPPUNIT_ASSERT(forecasts.getValue(1, 1, 0, 1) == original.getValue(1, 1, 0, 1));
    CPPUNIT_ASSERT(forecasts.getValue(1, 1, 0, 2) == original.getValue(1, 1, 0, 2));

    // Parameter my_U
    CPPUNIT_ASSERT(forecasts.getValue(2, 0, 0, 0) == 0.5);
    CPPUNIT_ASSERT(forecasts.getValue(2, 0, 0, 1) == sqrt(18));
    CPPUNIT_ASSERT(forecasts.getValue(2, 0, 0, 2) == 3);
    CPPUNIT_ASSERT(forecasts.getValue(2, 1, 0, 0) == sqrt(2));
    CPPUNIT_ASSERT(forecasts.getValue(2, 1, 0, 1) == 1.5);
    CPPUNIT_ASSERT(forecasts.getValue(2, 1, 0, 2) == sqrt(1.5 * 1.5 * 2));
}
