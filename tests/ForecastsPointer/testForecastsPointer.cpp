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
