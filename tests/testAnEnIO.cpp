/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   testAnEnIO.cpp
 * Author: wuh20
 *
 * Created on Jun 29, 2018, 6:02:11 PM
 */

#include "testAnEnIO.h"


CPPUNIT_TEST_SUITE_REGISTRATION(testAnEnIO);

testAnEnIO::testAnEnIO() {
}

testAnEnIO::~testAnEnIO() {
}

void testAnEnIO::setUp() {
}

void testAnEnIO::tearDown() {
}

void testAnEnIO::testReadFile() {

    /*
     * Test reading an observation file.
     */

    string file = "/Users/wuh20/github/AnalogsEnsemble/tests/test_observations.nc";

    AnEnIO io(file);
    io.setVerbose(0);
    io.setFileType("Observations");

    Observations_array observations;
    io.handleError(io.readObservations(observations));

    size_t num_pars = observations.get_parameters_size();
    size_t num_stations = observations.get_stations_size();
    size_t num_times = observations.get_times_size();

    cout << "Observation data dimensions:" << endl
            << "parameters: " << num_pars << endl
            << "stations: " << num_stations << endl
            << "times: " << num_times << endl;

    double count = 1;
    for (size_t k = 0; k < num_times; k++) {
        for (size_t j = 0; j < num_stations; j++) {
            for (size_t i = 0; i < num_pars; i++) {
                CPPUNIT_ASSERT(count == observations.getValues()[i][j][k]);
                count++;
            }
        }
    }
    
    cout << "Done!" << endl;
}