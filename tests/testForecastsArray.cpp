/*
 * File:   testForecastsArray.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on Jul 1, 2018, 10:47:47 AM
 */

#include "testForecastsArray.h"

#include "../Forecasts.h"

#include <numeric>


CPPUNIT_TEST_SUITE_REGISTRATION(testForecastsArray);

testForecastsArray::testForecastsArray() {
}

testForecastsArray::~testForecastsArray() {
}

void testForecastsArray::testForecastValueSequence() {

    /**
     * Test the sequence of values stored in the Forecast data type.
     * Data should be set in column-major order.
     */

    anenSta::Station s1, s2("Hunan", 10, 20), s3("Hubei"),
            s4("Guangdong", 30, 40), s5("Zhejiang"),
            s6("Beijing", 30, 30);
    anenSta::Stations stations;
    stations.insert(stations.end(),{s1, s2, s3, s4, s5, s6});

    anenPar::Parameter p1, p2("temperature", 0.6), p3("humidity", 0.3),
            p4("wind direction", 0.05, true);
    p1.setWeight(0.05);

    anenPar::Parameters parameters;
    parameters.insert(parameters.end(),{p1, p2, p3, p4});

    anenTime::Times times;
    times.insert(times.end(),{1, 2, 3, 4, 5, 6, 7, 8, 9, 10});

    anenTime::FLTs flts;
    flts.insert(flts.end(),{100, 200, 300, 400});

    vector<double> values(parameters.size() * stations.size() *
            times.size() * flts.size());
    iota(values.begin(), values.end(), 0);

    Forecasts_array forecasts(parameters, stations, times, flts, values);

    size_t l = 0;
    for (size_t n = 0; n < forecasts.getFLTs().size(); n++)
        for (size_t m = 0; m < forecasts.getTimes().size(); m++)
            for (size_t j = 0; j < forecasts.getStations().size(); j++)
                for (size_t i = 0; i < forecasts.getParameters().size();
                        i++, l++)
                    CPPUNIT_ASSERT(forecasts.getValueByIndex(i, j, m, n)
                        == values[l]);
}
