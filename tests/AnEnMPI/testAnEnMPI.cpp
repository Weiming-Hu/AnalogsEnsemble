/*
 * File:   testAnEnMPI.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on Mar 4, 2020, 4:09:20 PM
 */

#include "testAnEnMPI.h"
#include "AnEnISMPI.h"
#include "ObservationsPointer.h"
#include "ForecastsPointer.h"

#include <stdlib.h>

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION(testAnEnMPI);

testAnEnMPI::testAnEnMPI() {
}

testAnEnMPI::~testAnEnMPI() {
}

void
testAnEnMPI::testCompute_() {

    /*
     * Compare the results from serial and MPI AnEn
     */

    Parameters parameters;
    parameters.push_back(Parameter("wspd", false));
    parameters.push_back(Parameter("wdir", true));
    parameters.push_back(Parameter("temp", false));

    Stations stations;
    for (int i = 0; i < 1000; ++i) stations.push_back(Station(i, i));

    Times forecast_times;
    for (int i = 0; i < 1000; ++i) forecast_times.push_back(i * 10);

    Times flts;
    for (int i = 0; i < 10; ++i) flts.push_back(i);

    Times observation_times;
    for (int i = 0; i < 1010; ++i) observation_times.push_back(i);

    ForecastsPointer forecasts(parameters, stations, forecast_times, flts);
    ObservationsPointer observations(parameters, stations, observation_times);

    // Randomize values
    double *forecast_ptr = forecasts.getValuesPtr();
    for (int i = 0; i < forecasts.num_elements(); ++i) forecast_ptr[i] = rand() / 100.0;

    double *observation_ptr = observations.getValuesPtr();
    for (int i = 0; i < observations.num_elements(); ++i) observation_ptr[i] = rand() / 100.0;

    // Set config
    Config config;
    config.save_analogs = true;
    config.save_analogs_time_index = true;
    config.save_sims = true;
    config.save_sims_time_index = true;
    config.operation = true;

    Times test_times, search_times;
    for (int i = 800; i < 1000; ++i) test_times.push_back(i);
    for (int i = 0; i < 800; ++i) search_times.push_back(i);

    // Compute AnEn with serial library
    AnEnIS anen_serial(config);
    anen_serial.compute(forecasts, observations, test_times, search_times);

    // Compute AnEn with MPI library
    AnEnISMPI anen_mpi(config);
    anen_mpi.compute(forecasts, observations, test_times, search_times);

    // Compare results
    CPPUNIT_ASSERT(anen_serial.num_analogs() == anen_mpi.num_analogs());
    CPPUNIT_ASSERT(anen_serial.num_sims() == anen_mpi.num_sims());
    CPPUNIT_ASSERT(anen_serial.obs_var_index() == anen_mpi.obs_var_index());

    CPPUNIT_ASSERT(anen_serial.sds() == anen_mpi.sds());
    CPPUNIT_ASSERT(anen_serial.sims_metric() == anen_mpi.sims_metric());
    CPPUNIT_ASSERT(anen_serial.sims_time_index() == anen_mpi.sims_time_index());
    CPPUNIT_ASSERT(anen_serial.analogs_value() == anen_mpi.analogs_value());
    CPPUNIT_ASSERT(anen_serial.analogs_time_index() == anen_mpi.analogs_time_index());

    return;
}
