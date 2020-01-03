/*
 * File:   testObservationsArra y.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on Jun 29, 2018, 5:37:58 PM
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

void testObservationsArray::testColumnMajor() {

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

void testObservationsArray::testObservationValueSequence() {

    /**
     * Test the sequence of values stored in the Observation data type.
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

    vector<double> values(parameters.size() * stations.size() * times.size());
    iota(values.begin(), values.end(), 0);

    Observations_array observations(parameters, stations, times, values);

    size_t l = 0;
        for (size_t m = 0; m < observations.getTimes().size(); m++)
            for (size_t j = 0; j < observations.getStations().size(); j++)
                for (size_t i = 0; i < observations.getParameters().size();
                        i++, l++)
                    CPPUNIT_ASSERT(observations.getValue(i, j, m)
                        == values[l]);
}
