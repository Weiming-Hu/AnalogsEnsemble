/*
 * File:   testObservationsArra y.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on Jun 29, 2018, 5:37:58 PM
 */

#include "testObservationsArray.h"
#include "ObservationsArray.h"

#include <iostream>
#include <ctime>
#include <vector>
#include <iomanip>
#include <sstream>
#include <boost/assign/list_of.hpp>
#include <boost/assign/list_inserter.hpp>

using namespace std;
using namespace boost::bimaps;
using namespace boost;

CPPUNIT_TEST_SUITE_REGISTRATION(testObservationsArray);

testObservationsArray::testObservationsArray() {
}

testObservationsArray::~testObservationsArray() {
}

void testObservationsArray::testColumnMajor_() {

    /*
     * Test the traverse order and the traverse time of the data structure 
     * Observations_array. Because Observations_array has a column-major order.
     * Traverse should be faster in column-major order than in row-major order.
     */

    // Create a test data set
    size_t num_pars = 10;
    size_t num_stations = 4000;
    size_t num_times = 500;
    
    stringstream ss;

    Parameters parameters;
    for (size_t i = 0; i < num_pars; ++i) {
        ss << "Parameter_" << i;
        parameters.push_back(Parameters::value_type(i, Parameter(ss.str())));
        ss.clear();
    }

    Stations stations;
    for (size_t i = 0; i < num_stations; ++i) {
        ss << "Station_" << i;
        stations.push_back(Stations::value_type(i, Station(i, i, ss.str())));
        ss.clear();
    }

    Times times;
    for (size_t i = 0; i < num_times; ++i) {
        times.push_back(Times::value_type(i, Time(i)));
    }

    vector<double> data(num_pars * num_stations * num_times, 0);

    ObservationsArray observations(parameters, stations, times);
    
    double *ptr = observations.getValuesPtr();
    memcpy(ptr, data.data(), data.size() * sizeof (double));

    // This variable is used to read the value. It does no real computation.
    double read_only = 0.0;

    // Traverse the data structure in row-major order
    clock_t begin_row_major = clock();
    for (size_t i = 0; i < num_pars; i++) {
        for (size_t j = 0; j < num_stations; j++) {
            for (size_t k = 0; k < num_times; k++) {
                read_only += observations.getValue(i, j, k);
            }
        }
    }
    clock_t end_row_major = clock();

    // Traverse the data structure in column-major order
    clock_t begin_column_major = clock();
    for (size_t k = 0; k < num_times; k++) {
        for (size_t j = 0; j < num_stations; j++) {
            for (size_t i = 0; i < num_pars; i++) {
                read_only += observations.getValue(i, j, k);
            }
        }
    }
    clock_t end_column_major = clock();

    // Compute and compare time
    double elapsed_row_major = double(
            end_row_major - begin_row_major) / CLOCKS_PER_SEC;
    double elapsed_column_major = double(
            end_column_major - begin_column_major) / CLOCKS_PER_SEC;

    cout << setprecision(3) << "Row-major traverse time: "
            << elapsed_row_major << "s" << endl
            << "Column-major traverse time: "
            << elapsed_column_major << "s" << endl;

    CPPUNIT_ASSERT(elapsed_column_major < elapsed_row_major);
    
    cout << "Done!" << endl;
}

void testObservationsArray::testObservationValueSequence_() {

    /**
     * Test the sequence of values stored in the Observation data type.
     * Data should be set in column-major order.
     */

    Station s1, s2(10, 20, "Hunan"), s3(10, 30, "Hubei"),
            s4(30, 40, "Guangdong"), s5(15, 23), s6(30, 30, "Beijing");
    Stations stations;
    assign::push_back(stations.left)(0, s1)(1, s2)(2, s3)(3, s4)(4, s5)(5, s6);

    Parameter p1, p2("temperature", 0.6), p3("humidity", 0.3),
            p4("wind direction", 0.05, true);
    p1.setWeight(0.05);

    Parameters parameters;
    assign::push_back(parameters.left)(0, p1)(1, p2)(2, p3)(3, p4);

    Times times;
    for (size_t i = 1; i <= 10; ++i) {
        times.push_back(Times::value_type(i, Time(i)));
    }
    
    vector<double> values(parameters.size() * stations.size() * times.size());
    iota(values.begin(), values.end(), 0);

    ObservationsArray observations(parameters, stations, times);

    double *ptr = observations.getValuesPtr();
    memcpy(ptr, values.data(), values.size() * sizeof (double));

    size_t l = 0;
    for (size_t m = 0; m < observations.getTimes().size(); m++)
        for (size_t j = 0; j < observations.getStations().size(); j++)
            for (size_t i = 0; i < observations.getParameters().size(); i++, l++)
                CPPUNIT_ASSERT(observations.getValue(i, j, m) == values[l]);
}
