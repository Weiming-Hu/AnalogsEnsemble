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

#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <cstdio>

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION(testAnEnIO);

testAnEnIO::testAnEnIO() {
}

testAnEnIO::~testAnEnIO() {
}

void testAnEnIO::setUp() {
}

void testAnEnIO::tearDown() {
}

void testAnEnIO::testReadObservationFile() {

    /*
     * Test reading an observation file.
     */

    string file = "tests/test_observations.nc";
    string mode("Read");

    AnEnIO io(mode, file);
    io.setVerbose(0);
    io.setFileType("Observations");

    Observations_array observations;
    io.handleError(io.readObservations(observations));

    size_t num_pars = observations.get_parameters_size();
    size_t num_stations = observations.get_stations_size();
    size_t num_times = observations.get_times_size();

    //    cout << "Observation data dimensions:" << endl
    //            << "parameters: " << num_pars << endl
    //            << "stations: " << num_stations << endl
    //            << "times: " << num_times << endl;

    double count = 1;
    for (size_t k = 0; k < num_times; k++)
        for (size_t j = 0; j < num_stations; j++)
            for (size_t i = 0; i < num_pars; i++) {
                CPPUNIT_ASSERT(count == observations.getValues()[i][j][k]);
                count++;
            }

    cout << "Done!";
}

void testAnEnIO::testReadForecastFile() {

    /*
     * Test reading a forecast file.
     */

    string file = "tests/test_forecasts.nc";

    AnEnIO io("Read", file);
    io.setVerbose(0);
    io.setFileType("Forecasts");

    Forecasts_array forecasts;
    io.handleError(io.readForecasts(forecasts));

    size_t num_pars = forecasts.get_parameters_size();
    size_t num_stations = forecasts.get_stations_size();
    size_t num_times = forecasts.get_times_size();
    size_t num_flts = forecasts.get_flts_size();

    double count = 1;
    for (size_t l = 0; l < num_flts; l++)
        for (size_t k = 0; k < num_times; k++)
            for (size_t j = 0; j < num_stations; j++)
                for (size_t i = 0; i < num_pars; i++) {
                    CPPUNIT_ASSERT(count == forecasts.getValue(i, j, k, l));
                    count++;
                }

    cout << "Done!";
}

void testAnEnIO::testWriteReadObservationFile() {

    /* 
     * Test writing and reading of an observation file.
     */

    // Create an example observation object
    anenSta::Station s1, s2("Hunan", 10, 20), s3("Hubei"),
            s4("Guangdong", 30, 40), s5("Zhejiang"),
            s6("Beijing", 30, 30);
    anenSta::Stations stations_write;
    stations_write.insert(stations_write.end(),{s1, s2, s3, s4, s5, s6});

    anenPar::Parameter p1, p2("temperature", 0.6), p3("humidity", 0.3),
            p4("wind direction", 0.05, true);
    p1.setWeight(0.05);

    anenPar::Parameters parameters_write;
    parameters_write.insert(parameters_write.end(),{p1, p2, p3, p4});

    anenTime::Times times_write;
    times_write.insert(times_write.end(),{1, 2, 3, 4, 5, 6, 7, 8, 9, 10});

    vector<double> values_write(parameters_write.size()
            * stations_write.size() * times_write.size());
    generate(values_write.begin(), values_write.end(), rand);

    Observations_array observations_write(
            parameters_write, stations_write, times_write, values_write);

    // Remove file if exists
    string file_path("read-write-observations.nc");
    remove(file_path.c_str());
    
    AnEnIO io("Write", file_path, "Observations", 2);

    io.handleError(io.writeObservations(observations_write));

    Observations_array observations_read;
    io.setMode("Read");
    io.handleError(io.readObservations(observations_read));

    /* I have to write my own comparing functions because the comparison
     * function (==) provided by the class is based on ID. But here I
     * want to compare actual attribute values.
     */
    
    // Verify parameters
    auto parameters_read = observations_read.getParameters();
    const anenPar::multiIndexParameters::index<anenPar::by_insert>::type&
            parameters_read_by_insert =
            parameters_read.get<anenPar::by_insert>();
    const anenPar::multiIndexParameters::index<anenPar::by_insert>::type&
            parameters_write_by_insert =
            parameters_write.get<anenPar::by_insert>();
    for (size_t i = 0; i < parameters_read.size(); i++) {
        CPPUNIT_ASSERT(parameters_read_by_insert[i].getName() ==
                parameters_write_by_insert[i].getName());
        CPPUNIT_ASSERT(parameters_read_by_insert[i].getWeight() ==
                parameters_write_by_insert[i].getWeight());
        CPPUNIT_ASSERT(parameters_read_by_insert[i].getCircular() ==
                parameters_write_by_insert[i].getCircular());
    }

    // Verify stations
    auto stations_read = observations_read.getStations();
    const anenSta::multiIndexStations::index<anenSta::by_insert>::type &
            stations_read_by_insert =
            stations_read.get<anenSta::by_insert>();
    const anenSta::multiIndexStations::index<anenSta::by_insert>::type &
            stations_write_by_insert =
            stations_write.get<anenSta::by_insert>();
    for (size_t i = 0; i < stations_read.size(); i++) {
        CPPUNIT_ASSERT(stations_read_by_insert[i].getName() ==
                stations_write_by_insert[i].getName());
        CPPUNIT_ASSERT(stations_read_by_insert[i].getX() ==
                stations_write_by_insert[i].getX() ||
                (isnan(stations_read_by_insert[i].getX()) &&
                isnan(stations_write_by_insert[i].getX())));
        CPPUNIT_ASSERT(stations_read_by_insert[i].getY() ==
                stations_write_by_insert[i].getY() ||
                (isnan(stations_read_by_insert[i].getY()) &&
                isnan(stations_write_by_insert[i].getY())));
    }

    // Verify times
    auto times_read = observations_read.getTimes();
    const anenTime::multiIndexTimes::index<anenTime::by_insert>::type &
            times_read_by_insert = times_read.get<anenTime::by_insert>();
    const anenTime::multiIndexTimes::index<anenTime::by_insert>::type &
            times_write_by_insert = times_write.get<anenTime::by_insert>();
    for (size_t i = 0; i < times_read.size(); i++) {
        CPPUNIT_ASSERT(times_read_by_insert[i] == times_write_by_insert[i]);
    }

    // Verify values
    auto data_read = observations_read.getValues(),
            data_write = observations_write.getValues();
    CPPUNIT_ASSERT(data_read == data_write);

    // Remove the file
    remove(file_path.c_str());
}

void testAnEnIO::testWriteReadForecastFile() {

    /* 
     * Test writing and reading of a forecast file.
     */
    
    // Create an example forecast object
    anenSta::Station s1, s2("Hunan", 10, 20), s3("Hubei"),
            s4("Guangdong", 30, 40), s5("Zhejiang"),
            s6("Beijing", 30, 30);
    anenSta::Stations stations_write;
    stations_write.insert(stations_write.end(),{s1, s2, s3, s4, s5, s6});

    anenPar::Parameter p1, p2("temperature", 0.6), p3("humidity", 0.3),
            p4("wind direction", 0.05, true);
    p1.setWeight(0.05);

    anenPar::Parameters parameters_write;
    parameters_write.insert(parameters_write.end(),{p1, p2, p3, p4});

    anenTime::Times times_write;
    times_write.insert(times_write.end(),{1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
    
    anenTime::FLTs flts_write;
    flts_write.insert(flts_write.end(), {100, 200, 300, 400, 500});

    vector<double> values_write(parameters_write.size()
    * stations_write.size() * times_write.size() * flts_write.size());
    generate(values_write.begin(), values_write.end(), rand);

    Forecasts_array forecasts_write(
            parameters_write, stations_write,
            times_write, flts_write, values_write);

    // Remove file if exists
    string file_path("read-write-forecasts.nc");
    remove(file_path.c_str());
    
    AnEnIO io("Write", file_path, "Forecasts", 2);

    io.handleError(io.writeForecasts(forecasts_write));

    Forecasts_array forecasts_read;
    io.setMode("Read");
    io.handleError(io.readForecasts(forecasts_read));

    /* I have to write my own comparing functions because the comparison
     * function (==) provided by the class is based on ID. But here I
     * want to compare actual attribute values.
     */
    
    // Verify parameters
    auto parameters_read = forecasts_read.getParameters();
    const anenPar::multiIndexParameters::index<anenPar::by_insert>::type&
            parameters_read_by_insert =
            parameters_read.get<anenPar::by_insert>();
    const anenPar::multiIndexParameters::index<anenPar::by_insert>::type&
            parameters_write_by_insert =
            parameters_write.get<anenPar::by_insert>();
    for (size_t i = 0; i < parameters_read.size(); i++) {
        CPPUNIT_ASSERT(parameters_read_by_insert[i].getName() ==
                parameters_write_by_insert[i].getName());
        CPPUNIT_ASSERT(parameters_read_by_insert[i].getWeight() ==
                parameters_write_by_insert[i].getWeight());
        CPPUNIT_ASSERT(parameters_read_by_insert[i].getCircular() ==
                parameters_write_by_insert[i].getCircular());
    }

    // Verify stations
    auto stations_read = forecasts_read.getStations();
    const anenSta::multiIndexStations::index<anenSta::by_insert>::type &
            stations_read_by_insert =
            stations_read.get<anenSta::by_insert>();
    const anenSta::multiIndexStations::index<anenSta::by_insert>::type &
            stations_write_by_insert =
            stations_write.get<anenSta::by_insert>();
    for (size_t i = 0; i < stations_read.size(); i++) {
        CPPUNIT_ASSERT(stations_read_by_insert[i].getName() ==
                stations_write_by_insert[i].getName());
        CPPUNIT_ASSERT(stations_read_by_insert[i].getX() ==
                stations_write_by_insert[i].getX() ||
                (isnan(stations_read_by_insert[i].getX()) &&
                isnan(stations_write_by_insert[i].getX())));
        CPPUNIT_ASSERT(stations_read_by_insert[i].getY() ==
                stations_write_by_insert[i].getY() ||
                (isnan(stations_read_by_insert[i].getY()) &&
                isnan(stations_write_by_insert[i].getY())));
    }

    // Verify times
    auto times_read = forecasts_read.getTimes();
    const anenTime::multiIndexTimes::index<anenTime::by_insert>::type &
            times_read_by_insert = times_read.get<anenTime::by_insert>();
    const anenTime::multiIndexTimes::index<anenTime::by_insert>::type &
            times_write_by_insert = times_write.get<anenTime::by_insert>();
    for (size_t i = 0; i < times_read.size(); i++) {
        CPPUNIT_ASSERT(times_read_by_insert[i] == times_write_by_insert[i]);
    }

    // Verify FLTs
    auto flts_read = forecasts_read.getFLTs();
    const anenTime::multiIndexTimes::index<anenTime::by_insert>::type &
            flts_read_by_insert = flts_read.get<anenTime::by_insert>();
    const anenTime::multiIndexTimes::index<anenTime::by_insert>::type &
            flts_write_by_insert = flts_write.get<anenTime::by_insert>();
    for (size_t i = 0; i < flts_read.size(); i++) {
        CPPUNIT_ASSERT(flts_read_by_insert[i] == flts_write_by_insert[i]);
    }

    // Verify values
    auto data_read = forecasts_read.getValues(),
            data_write = forecasts_write.getValues();
    CPPUNIT_ASSERT(data_read == data_write);

    // Remove the file
    remove(file_path.c_str());
}