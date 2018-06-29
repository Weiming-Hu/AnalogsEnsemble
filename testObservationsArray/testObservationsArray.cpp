/*
 * File:   testObservationsArray.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on Jun 29, 2018, 1:36:09 PM
 */

#include "testObservationsArray.h"

#include <iostream>
#include <ctime>
#include <vector>
#include <iomanip>

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION(testObservationsArray);

testObservationsArray::testObservationsArray() {
}

testObservationsArray::~testObservationsArray() {
}

void testObservationsArray::setUp() {
}

void testObservationsArray::tearDown() {
}

void testObservationsArray::testTraverseTime() {

    /*
     * Test the traverse order and the traverse time of the data structure 
     * Observations_array. Because Observations_array has a column-major order.
     * Traverse should be faster in column-major order than in row-major order.
     */

    // Create a test data set
    size_t num_pars = 10;
    size_t num_stations = 4000;
    size_t num_times = 500;

    vector<anenPar::Parameter> vec_pars(num_pars);
    vector<anenSta::Station> vec_stations(num_stations);
    vector<double> vec_times(num_times);

    for (auto & par : vec_pars) {
        anenPar::Parameter tmp;
        par = tmp;
    }

    for (auto & station : vec_stations) {
        anenSta::Station tmp;
        station = tmp;
    }

    double tmp = 0.0;
    for (auto & time : vec_times) {
        time = tmp;
        tmp++;
    }

    anenPar::Parameters parameters;
    parameters.insert(parameters.end(), vec_pars.begin(), vec_pars.end());

    anenSta::Stations stations;
    stations.insert(stations.end(), vec_stations.begin(), vec_stations.end());

    anenTime::Times times;
    times.insert(times.end(), vec_times.begin(), vec_times.end());

    vector<double> data(num_pars * num_stations * num_times, 0);

    Observations_array observations(parameters, stations, times, data);

    // This variable is used to read the value. It does no real computation.
    double read_only;

    // Traverse the data structure in row-major order
    clock_t begin_row_major = clock();
    for (size_t i = 0; i < num_pars; i++) {
        for (size_t j = 0; j < num_stations; j++) {
            for (size_t k = 0; k < num_times; k++) {
                read_only = observations.getValue(i, j, k);
            }
        }
    }
    clock_t end_row_major = clock();

    // Traverse the data structure in column-major order
    clock_t begin_column_major = clock();
    for (size_t k = 0; k < num_times; k++) {
        for (size_t j = 0; j < num_stations; j++) {
            for (size_t i = 0; i < num_pars; i++) {
                read_only = observations.getValue(i, j, k);
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
}
