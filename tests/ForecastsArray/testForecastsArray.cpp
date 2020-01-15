/*
 * File:   testForecastsArray.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on Jul 1, 2018, 10:47:47 AM
 */

#include "testForecastsArray.h"
#include "ForecastsArray.h"

#include <numeric>
#include <boost/assign/list_of.hpp>
#include <boost/assign/list_inserter.hpp>

using namespace std;
using namespace boost::bimaps;
using namespace boost;

CPPUNIT_TEST_SUITE_REGISTRATION(testForecastsArray);

testForecastsArray::testForecastsArray() {
}

testForecastsArray::~testForecastsArray() {
}

void testForecastsArray::testForecastSetVectorValues_() {

    /**
     * Test the sequence of values stored in the Forecast data type.
     * Data should be set in column-major order.
     */
    
    Station s1, s2(10, 20, "Hunan"), s3, s4(30, 40, "Guangdong"),
            s5(15, 23), s6(30, 30, "Beijing");
    Stations stations;
    assign::push_back(stations.left)(0, s1)(1, s2)(2, s3)(3, s4)(4, s5)(5, s6);

    Parameter p1, p2("temperature", 0.6), p3("humidity", 0.3),
            p4("wind direction", 0.05, true);
    p1.setWeight(0.05);

    Parameters parameters;
    assign::push_back(parameters.left)(0, p1)(1, p2)(2, p3)(3, p4);

    Times times;
    for (size_t i = 0; i < 10; ++i) {
        times.push_back(Times::value_type(i, Time(i + 1)));
    }

    Times flts;
    assign::push_back(flts.left)
            (0, Time(100))(1, Time(200))(2, Time(300))(3, Time(400));

    vector<double> values(parameters.size() * stations.size() *
            times.size() * flts.size());
    iota(values.begin(), values.end(), 0);

    ForecastsArray forecasts(parameters, stations, times, flts);
    double *ptr = forecasts.getValuesPtr();
    memcpy(ptr, values.data(), values.size() * sizeof(double));

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
