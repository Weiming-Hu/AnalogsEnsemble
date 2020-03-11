/*
 * File:   testAnEnIOMPI.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on Mar 4, 2020, 4:09:20 PM
 */

#include <cmath>

#include "ForecastsPointer.h"
#include "FunctionsIO.h"
#include "AnEnReadGrib.h"
#include "testAnEnIOMPI.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION(testAnEnIOMPI);

testAnEnIOMPI::testAnEnIOMPI() {
    setUp();
}

testAnEnIOMPI::~testAnEnIOMPI() {
    tearDown();
}

void
testAnEnIOMPI::setUp() {

    // Set up data files
    data_folder = _TEST_DATA_FOLDER;
    FunctionsIO::listFiles(data_files, data_folder, ".*nam_218_\\d{8}_\\d{4}_\\d{3}\\.grb2$");

    if (data_files.size() != 5) {
        cerr << "Wrong number of data files when testing AnEnIOMPI. This probably caused by FunctionsIO::listFiles" << endl;
        exit(1);
    }

    // Set up grib parameters
    vector<string> parameters_name = {"temperature", "relative humidity", "min temperature", "min relative humidity"};
    vector<string> parameters_level_type(4, "heightAboveGround");
    vector<long> parameters_id = {167, 260242, 3016, 260261};
    vector<long> parameters_level(4, 2);
    vector<bool> parameters_circular(4, false);

    FunctionsIO::toParameterVector(grib_parameters,
            parameters_name, parameters_id, parameters_level,
            parameters_level_type, parameters_circular, Verbose::Warning);
    
    // Set up the rest of the class members
    regex_str = ".*nam_218_(?P<day>\\d{8})_(?P<cycle>\\d{2})\\d{2}_(?P<flt>\\d{3})\\.grb2$";
    stations_index = {0, 30, 50, 60};
    unit_in_seconds = 3600;
    delimited = false;

    // Set up MPI environment
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_proc);

    if (num_proc <= 2) {
        MPI_Finalize();
        cerr << "Error: Please launch test with MPI launcher program, e.g. mpirun, with at least 3 processes" << endl;
        exit(1);
    }
    
    return;
}

void
testAnEnIOMPI::tearDown() {

    data_folder.clear();
    data_files.clear();
    grib_parameters.clear();
    regex_str.clear();
    stations_index.clear();

    return;
}

void
testAnEnIOMPI::testReadGrib_() {

    /*
     * This function test the reading results from the serial
     * library AnEnReadGrib and the MPI version AnEnReadGribMPI
     */

    // Read files in parallel
    if (world_rank == 0) cout << "************** Read with MPI **************" << endl;

    ForecastsPointer forecasts_mpi;
    AnEnReadGribMPI anen_read_mpi(Verbose::Debug, Verbose::Debug);
    anen_read_mpi.readForecasts(forecasts_mpi, grib_parameters,
            data_files, regex_str, unit_in_seconds, delimited, stations_index);

    // Terminate worker processes
    if (world_rank != 0) {
        MPI_Finalize();
        exit(0);
    }
    cout << "************** End reading with MPI **************" << endl;

    // Read files in serial
    cout << "************** Read in serial **************" << endl;
    ForecastsPointer forecasts_serial;
    AnEnReadGrib anen_read_serial(Verbose::Progress);
    anen_read_serial.readForecasts(forecasts_serial, grib_parameters,
            data_files, regex_str, unit_in_seconds, delimited, stations_index);
    cout << "************** End reading in serial **************" << endl;

    /*
     * Results from the serial and the MPI versions should be identical
     */

    // Initialize variables
    Parameters parameters_serial, parameters_mpi;
    Stations stations_serial, stations_mpi;
    Times times_serial, times_mpi;
    Times flts_serial, flts_mpi;
    size_t num_values_serial, num_values_mpi;
    double* p_value_serial = nullptr;
    double* p_value_mpi = nullptr;


    // Get values
    parameters_serial = forecasts_serial.getParameters();
    parameters_mpi = forecasts_mpi.getParameters();
    stations_serial = forecasts_serial.getStations();
    stations_mpi = forecasts_mpi.getStations();
    times_serial = forecasts_serial.getTimes();
    times_mpi = forecasts_mpi.getTimes();
    flts_serial = forecasts_serial.getFLTs();
    flts_mpi = forecasts_mpi.getFLTs();
    num_values_serial = forecasts_serial.num_elements();
    num_values_mpi = forecasts_mpi.num_elements();
    p_value_serial = forecasts_serial.getValuesPtr();
    p_value_mpi = forecasts_mpi.getValuesPtr();

    cout << "Serial forecasts: " << forecasts_serial << endl
        << "MPI forecasts: " << forecasts_mpi << endl;


    // Compare results
    CPPUNIT_ASSERT(parameters_serial == parameters_mpi);
    CPPUNIT_ASSERT(stations_serial == stations_mpi);
    CPPUNIT_ASSERT(times_serial == times_mpi);
    CPPUNIT_ASSERT(flts_serial == flts_mpi);
    CPPUNIT_ASSERT(num_values_serial == num_values_mpi);

    for (size_t i = 0; i < num_values_mpi; ++i) {
        cout << "Comparing " << p_value_serial[i] << " " << p_value_mpi[i] << endl;

        if (std::isnan(p_value_serial[i])) CPPUNIT_ASSERT(std::isnan(p_value_mpi[i]));
        else CPPUNIT_ASSERT(p_value_serial[i] == p_value_mpi[i]);
    }

    return;
}

