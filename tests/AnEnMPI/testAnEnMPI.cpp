/*
 * File:   testAnEnMPI.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on Mar 4, 2020, 4:09:20 PM
 */
#include "Profiler.h"
#include "AnEnISMPI.h"
#include "testAnEnMPI.h"
#include "ForecastsPointer.h"
#include "ObservationsPointer.h"

#include <stdlib.h>

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION(testAnEnMPI);

testAnEnMPI::testAnEnMPI() {
    getProcInfo();
}

testAnEnMPI::~testAnEnMPI() {
}

void
testAnEnMPI::getProcInfo() {
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    return;
}

void
testAnEnMPI::testCompute_() {

    /*
     * Compare the results from serial and MPI AnEn
     */

    // These information are both available on the master and wokers
    Config config;
    config.save_analogs = true;
    config.save_analogs_time_index = true;
    config.save_sims = true;
    config.save_sims_time_index = true;
    config.operation = true;
    config.verbose = Verbose::Debug;

    AnEnIS anen_serial(config);

    // These information are only created on master process
    Parameters parameters;
    Stations stations;
    Times forecast_times;
    Times observation_times;
    Times flts;
    ForecastsPointer forecasts;
    ObservationsPointer observations;
    Times test_times, search_times;
    Profiler prof;


    if (rank == 0) {
        // Master is reading data. This part, in operation, is usually parallelized with AnEnIOMPI

        parameters.push_back(Parameter("wspd", false));
        parameters.push_back(Parameter("wdir", true));
        parameters.push_back(Parameter("temp", false));

        // Don't worry about the numbers. They are just random.
        for (int i = 0; i < 21; ++i) stations.push_back(Station(i, i));
        for (int i = 0; i < 100; ++i) forecast_times.push_back(i * 10);
        for (int i = 0; i < 1010; ++i) observation_times.push_back(i);
        for (int i = 0; i < 3; ++i) flts.push_back(i);

        forecasts.setDimensions(parameters, stations, forecast_times, flts);
        observations.setDimensions(parameters, stations, observation_times);

        // Randomize values
        double *forecast_ptr = forecasts.getValuesPtr();
        for (int i = 0; i < forecasts.num_elements(); ++i) forecast_ptr[i] = rand() / 100.0;

        double *observation_ptr = observations.getValuesPtr();
        for (int i = 0; i < observations.num_elements(); ++i) observation_ptr[i] = rand() / 100.0;

        for (int i = 80; i < 100; ++i) test_times.push_back(i * 10);
        for (int i = 0; i < 80; ++i) search_times.push_back(i * 10);

        // Compute AnEn with serial library
        prof.start();
        cout << "Calculating analogs using AnEnIS ..." << endl;
        anen_serial.compute(forecasts, observations, test_times, search_times);
        cout << "AnEnIS results ready" << endl;
        prof.log_time_session("AnEnIS");
    }

    // Compute AnEn with MPI library
    if (rank == 0) cout << "Calculating analogs using AnEnISMPI..." << endl;
    else cout << "Worker #" << rank << " waiting to receive data from the master and to start AnEnISMPI ..." << endl;

    AnEnISMPI anen_mpi(config);
    anen_mpi.compute(forecasts, observations, test_times, search_times);
    if (rank == 0) cout << "AnEnISMPI results ready" << endl;

    // Compare results
    if (rank == 0) {
        prof.log_time_session("AnEnISMPI");
        // Only the master needs to compare results because the serial version is only executaed on the master process
        CPPUNIT_ASSERT(anen_serial.sds() == anen_mpi.sds());
        CPPUNIT_ASSERT(anen_serial.sims_metric() == anen_mpi.sims_metric());
        CPPUNIT_ASSERT(anen_serial.sims_time_index() == anen_mpi.sims_time_index());
        CPPUNIT_ASSERT(anen_serial.analogs_value() == anen_mpi.analogs_value());
        CPPUNIT_ASSERT(anen_serial.analogs_time_index() == anen_mpi.analogs_time_index());

        prof.summary(cout);
    }

    return;
}
