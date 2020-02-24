/*
 * File:   testObservationsArra y.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on Jun 29, 2018, 5:37:58 PM
 */

#include "testObservationsPointer.h"
#include "ObservationsPointer.h"
#include "Functions.h"

#include <iostream>
#include <numeric>
#include <vector>
#include <iomanip>
#include <sstream>
#include <boost/assign/list_of.hpp>
#include <boost/assign/list_inserter.hpp>

using namespace std;
using namespace boost::bimaps;
using namespace boost;

CPPUNIT_TEST_SUITE_REGISTRATION(testObservationsPointer);

testObservationsPointer::testObservationsPointer() {
}

testObservationsPointer::~testObservationsPointer() {
}

void
testObservationsPointer::testObservationValueSequence_() {

    /**
     * Test the sequence of values stored in the Observation data type.
     * Data should be set in column-major order.
     */

    Station s1, s2(10, 20, "Hunan"), s3(10, 30, "Hubei"),
            s4(30, 40, "Guangdong"), s5(15, 23), s6(30, 30, "Beijing");
    Stations stations;
    assign::push_back(stations.left)(0, s1)(1, s2)(2, s3)(3, s4)(4, s5)(5, s6);

    Parameter p1, p2("temperature"), p3("humidity"), p4("wind direction", true);

    Parameters parameters;
    assign::push_back(parameters.left)(0, p1)(1, p2)(2, p3)(3, p4);

    Times times;
    for (size_t i = 1; i <= 10; ++i) {
        times.push_back(Time(i));
    }

    vector<double> values(parameters.size() * stations.size() * times.size());
    iota(values.begin(), values.end(), 0);

    ObservationsPointer observations(parameters, stations, times);

    double *ptr = observations.getValuesPtr();
    memcpy(ptr, values.data(), values.size() * sizeof (double));

    size_t l = 0;
    for (size_t m = 0; m < observations.getTimes().size(); m++)
        for (size_t j = 0; j < observations.getStations().size(); j++)
            for (size_t i = 0; i < observations.getParameters().size(); i++, l++)
                CPPUNIT_ASSERT(observations.getValue(i, j, m) == values[l]);
}

void
testObservationsPointer::testSubset_() {

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

    vector<double> values(parameters.size() * stations.size() * times.size());
    iota(values.begin(), values.end(), rand());

    ObservationsPointer observations(parameters, stations, times);

    // Create subset forecasts
    Stations stations_subset;
    stations_subset.push_back(s2);
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

    // Create a subset forecasts
    ObservationsPointer observations_susbet(parameters_subset, stations_subset, times_subset);

    // Subset forecasts data
    observations.subset(observations_susbet);

    // Get the subset indices
    vector<size_t> stations_index, parameters_index, times_index;

    parameters.getIndices(parameters_subset, parameters_index);
    stations.getIndices(stations_subset, stations_index);
    times.getIndices(times_subset, times_index);

    double subset_value, original_value;

    for (size_t dim0_i = 0; dim0_i < parameters_subset.size(); ++dim0_i)
        for (size_t dim1_i = 0; dim1_i < stations_subset.size(); ++dim1_i)
            for (size_t dim2_i = 0; dim2_i < times_subset.size(); ++dim2_i) {
                subset_value = observations_susbet.getValue(dim0_i, dim1_i, dim2_i);
                original_value = observations.getValue(
                        parameters.getIndex(parameters_subset.getParameter(dim0_i)),
                        stations.getIndex(stations_subset.getStation(dim1_i)),
                        times.getIndex(times_subset.getTime(dim2_i)));

                cout << subset_value << " " << original_value << endl;
                CPPUNIT_ASSERT(subset_value == original_value);
            }
}
