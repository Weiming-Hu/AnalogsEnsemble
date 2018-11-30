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

    string file = file_observations;
    string mode("Read");

    AnEnIO io(mode, file);
    io.setVerbose(0);
    io.setFileType("Observations");

    Observations_array observations;
    io.handleError(io.readObservations(observations));

    size_t num_pars = observations.getParametersSize();
    size_t num_stations = observations.getStationsSize();
    size_t num_times = observations.getTimesSize();

    //    cout << "Observation data dimensions:" << endl
    //            << "parameters: " << num_pars << endl
    //            << "stations: " << num_stations << endl
    //            << "times: " << num_times << endl;

    double count = 1;
    for (size_t k = 0; k < num_times; k++) {
        for (size_t j = 0; j < num_stations; j++) {
            for (size_t i = 0; i < num_pars; i++) {
                CPPUNIT_ASSERT(count == observations.data()[i][j][k]);
                count++;
            }
        }
    }
}

void testAnEnIO::testReadForecastFile() {

    /*
     * Test reading a forecast file.
     */

    string file = file_forecasts;

    AnEnIO io("Read", file);
    io.setVerbose(0);
    io.setFileType("Forecasts");

    Forecasts_array forecasts;
    io.handleError(io.readForecasts(forecasts));

    size_t num_pars = forecasts.getParametersSize();
    size_t num_stations = forecasts.getStationsSize();
    size_t num_times = forecasts.getTimesSize();
    size_t num_flts = forecasts.getFLTsSize();

    double count = 1;
    for (size_t l = 0; l < num_flts; l++) {
        for (size_t k = 0; k < num_times; k++) {
            for (size_t j = 0; j < num_stations; j++) {
                for (size_t i = 0; i < num_pars; i++) {
                    CPPUNIT_ASSERT(count == forecasts.getValueByIndex(i, j, k, l));
                    count++;
                }
            }
        }
    }
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
                (std::isnan(stations_read_by_insert[i].getX()) &&
                std::isnan(stations_write_by_insert[i].getX())));
        CPPUNIT_ASSERT(stations_read_by_insert[i].getY() ==
                stations_write_by_insert[i].getY() ||
                (std::isnan(stations_read_by_insert[i].getY()) &&
                std::isnan(stations_write_by_insert[i].getY())));
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
    auto data_read = observations_read.data(),
            data_write = observations_write.data();
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
    flts_write.insert(flts_write.end(),{100, 200, 300, 400, 500});

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
                (std::isnan(stations_read_by_insert[i].getX()) &&
                std::isnan(stations_write_by_insert[i].getX())));
        CPPUNIT_ASSERT(stations_read_by_insert[i].getY() ==
                stations_write_by_insert[i].getY() ||
                (std::isnan(stations_read_by_insert[i].getY()) &&
                std::isnan(stations_write_by_insert[i].getY())));
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
    auto data_read = forecasts_read.data(),
            data_write = forecasts_write.data();
    CPPUNIT_ASSERT(data_read == data_write);

    // Remove the file
    remove(file_path.c_str());
}

void testAnEnIO::testReadPartParameters() {

    /**
     * Tests reading part of parameters
     */

    string file = file_observations;

    AnEnIO io("Read", file, "Observations", 1);

    anenPar::Parameters parameters;
    io.readParameters(parameters, 1, 2);

    CPPUNIT_ASSERT(parameters.size() == 2);
    const anenPar::multiIndexParameters::index<anenPar::by_insert>::type&
            parameters_by_insert_1 = parameters.get<anenPar::by_insert>();
    CPPUNIT_ASSERT(parameters_by_insert_1[0].getName() == "par_2");
    CPPUNIT_ASSERT(parameters_by_insert_1[1].getName() == "par_3");

    parameters.clear();
    io.readParameters(parameters, 0, 2, 3);

    CPPUNIT_ASSERT(parameters.size() == 2);
    const anenPar::multiIndexParameters::index<anenPar::by_insert>::type&
            parameters_by_insert_2 = parameters.get<anenPar::by_insert>();
    CPPUNIT_ASSERT(parameters_by_insert_2[0].getName() == "par_1");
    CPPUNIT_ASSERT(parameters_by_insert_2[1].getName() == "par_4");
}

void testAnEnIO::testReadPartStations() {

    /**
     * Tests reading part of stations
     */

    string file = file_observations;
    AnEnIO io("Read", file, "Observations", 1);

    anenSta::Stations stations;

    io.readStations(stations, 3, 2);
    const anenSta::multiIndexStations::index<anenSta::by_insert>::type &
            stations_by_insert_1 = stations.get<anenSta::by_insert>();
    CPPUNIT_ASSERT(stations_by_insert_1[0].getName() == "station_4");
    CPPUNIT_ASSERT(stations_by_insert_1[1].getName() == "station_5");

    io.setVerbose(0);
    CPPUNIT_ASSERT(io.readStations(stations, 1, 10, 4)
            == AnEnIO::errorType::WRONG_INDEX_SHAPE);

    stations.clear();
    io.setVerbose(2);
    io.readStations(stations, 1, 3, 6);
    const anenSta::multiIndexStations::index<anenSta::by_insert>::type &
            stations_by_insert_2 = stations.get<anenSta::by_insert>();
    CPPUNIT_ASSERT(stations_by_insert_2[0].getName() == "station_2");
    CPPUNIT_ASSERT(stations_by_insert_2[0].getX() == 2);
    CPPUNIT_ASSERT(stations_by_insert_2[1].getName() == "station_8");
    CPPUNIT_ASSERT(stations_by_insert_2[1].getY() == 13);
    CPPUNIT_ASSERT(stations_by_insert_2[2].getName() == "station_14");
    CPPUNIT_ASSERT(stations_by_insert_2[2].getX() == 14);
}

void testAnEnIO::testReadPartTimes() {

    /**
     * Tests reading part of times
     */

    string file = file_observations;
    AnEnIO io("Read", file, "Observations", 1);

    anenTime::Times times;
    io.readTimes(times, 1, 3, 10);
    const anenTime::multiIndexTimes::index<anenTime::by_insert>::type &
            times_by_insert = times.get<anenTime::by_insert>();
    CPPUNIT_ASSERT(times_by_insert[0] == 2);
    CPPUNIT_ASSERT(times_by_insert[1] == 12);
    CPPUNIT_ASSERT(times_by_insert[2] == 22);
}

void testAnEnIO::testReadPartFLTs() {

    /**
     * Tests reading part of FLTs
     */

    string file = file_forecasts;
    AnEnIO io("Read", file, "Forecasts", 1);

    anenTime::FLTs flts, flts_full;
    io.readFLTs(flts_full);

    io.readFLTs(flts, 1, 3, 2);
    const anenTime::multiIndexTimes::index<anenTime::by_insert>::type &
            flts_by_insert = flts.get<anenTime::by_insert>();
    CPPUNIT_ASSERT(flts_by_insert[0] == flts_full[1]);
    CPPUNIT_ASSERT(flts_by_insert[1] == flts_full[3]);
    CPPUNIT_ASSERT(flts_by_insert[2] == flts_full[5]);
}

void testAnEnIO::testReadPartObservations() {

    /**
     * Tests reading part of observations
     */

    string file = file_observations;
    AnEnIO io("Read", file, "Observations", 1);

    Observations_array observations, observations_full;

    vector<size_t> vec_start{0, 5, 5}, vec_count{2, 3, 1};
    vector<ptrdiff_t> vec_stride{2, 5, 1};

    io.readObservations(observations_full);

    io.readObservations(observations, vec_start, vec_count);
    CPPUNIT_ASSERT(observations.getValueByIndex(0, 0, 0)
            == observations_full.getValueByIndex(0, 5, 5));
    CPPUNIT_ASSERT(observations.getValueByIndex(1, 0, 0)
            == observations_full.getValueByIndex(1, 5, 5));
    CPPUNIT_ASSERT(observations.getValueByIndex(0, 1, 0)
            == observations_full.getValueByIndex(0, 6, 5));
    CPPUNIT_ASSERT(observations.getValueByIndex(1, 1, 0)
            == observations_full.getValueByIndex(1, 6, 5));
    CPPUNIT_ASSERT(observations.getValueByIndex(0, 2, 0)
            == observations_full.getValueByIndex(0, 7, 5));
    CPPUNIT_ASSERT(observations.getValueByIndex(1, 2, 0)
            == observations_full.getValueByIndex(1, 7, 5));

    io.readObservations(observations, vec_start, vec_count, vec_stride);
    CPPUNIT_ASSERT(observations.getValueByIndex(0, 0, 0)
            == observations_full.getValueByIndex(0, 5, 5));
    CPPUNIT_ASSERT(observations.getValueByIndex(1, 0, 0)
            == observations_full.getValueByIndex(2, 5, 5));
    CPPUNIT_ASSERT(observations.getValueByIndex(0, 1, 0)
            == observations_full.getValueByIndex(0, 10, 5));
    CPPUNIT_ASSERT(observations.getValueByIndex(1, 1, 0)
            == observations_full.getValueByIndex(2, 10, 5));
    CPPUNIT_ASSERT(observations.getValueByIndex(0, 2, 0)
            == observations_full.getValueByIndex(0, 15, 5));
    CPPUNIT_ASSERT(observations.getValueByIndex(1, 2, 0)
            == observations_full.getValueByIndex(2, 15, 5));
}

void testAnEnIO::testReadPartForecasts() {

    /**
     * Tests reading part of forecasts
     */

    string file = file_forecasts;
    AnEnIO io("Read", file, "Forecasts", 1);

    Forecasts_array forecasts, forecasts_full;

    vector<size_t> vec_start{0, 5, 5, 4}, vec_count{2, 3, 1, 1};
    vector<ptrdiff_t> vec_stride{2, 5, 1, 1};

    io.readForecasts(forecasts_full);

    io.readForecasts(forecasts, vec_start, vec_count);
    CPPUNIT_ASSERT(forecasts.getValueByIndex(0, 0, 0, 0)
            == forecasts_full.getValueByIndex(0, 5, 5, 4));
    CPPUNIT_ASSERT(forecasts.getValueByIndex(1, 0, 0, 0)
            == forecasts_full.getValueByIndex(1, 5, 5, 4));
    CPPUNIT_ASSERT(forecasts.getValueByIndex(0, 1, 0, 0)
            == forecasts_full.getValueByIndex(0, 6, 5, 4));
    CPPUNIT_ASSERT(forecasts.getValueByIndex(1, 1, 0, 0)
            == forecasts_full.getValueByIndex(1, 6, 5, 4));
    CPPUNIT_ASSERT(forecasts.getValueByIndex(0, 2, 0, 0)
            == forecasts_full.getValueByIndex(0, 7, 5, 4));
    CPPUNIT_ASSERT(forecasts.getValueByIndex(1, 2, 0, 0)
            == forecasts_full.getValueByIndex(1, 7, 5, 4));

    io.readForecasts(forecasts, vec_start, vec_count, vec_stride);
    CPPUNIT_ASSERT(forecasts.getValueByIndex(0, 0, 0, 0)
            == forecasts_full.getValueByIndex(0, 5, 5, 4));
    CPPUNIT_ASSERT(forecasts.getValueByIndex(1, 0, 0, 0)
            == forecasts_full.getValueByIndex(2, 5, 5, 4));
    CPPUNIT_ASSERT(forecasts.getValueByIndex(0, 1, 0, 0)
            == forecasts_full.getValueByIndex(0, 10, 5, 4));
    CPPUNIT_ASSERT(forecasts.getValueByIndex(1, 1, 0, 0)
            == forecasts_full.getValueByIndex(2, 10, 5, 4));
    CPPUNIT_ASSERT(forecasts.getValueByIndex(0, 2, 0, 0)
            == forecasts_full.getValueByIndex(0, 15, 5, 4));
    CPPUNIT_ASSERT(forecasts.getValueByIndex(1, 2, 0, 0)
            == forecasts_full.getValueByIndex(2, 15, 5, 4));
}

void testAnEnIO::testReadWriteSimilarityMatrices() {

    /**
     * Test the read and write functions for SimilarityMatrices
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
    flts_write.insert(flts_write.end(),{100, 200, 300, 400, 500});

    vector<double> values_write(parameters_write.size()
            * stations_write.size() * times_write.size() * flts_write.size());
    generate(values_write.begin(), values_write.end(), rand);

    Forecasts_array forecasts_write(
            parameters_write, stations_write,
            times_write, flts_write, values_write);

    SimilarityMatrices sims_write(forecasts_write);

    generate(sims_write.data(),
            sims_write.data() + sims_write.num_elements(), rand);

    string file_path = "read-write-similarity.nc";
    remove(file_path.c_str());
    AnEnIO io("Write", file_path, "Similarity", 2);
    io.writeSimilarityMatrices(sims_write);

    SimilarityMatrices sims_read;
    io.setMode("Read");
    io.readSimilarityMatrices(sims_read);

    CPPUNIT_ASSERT(sims_read.num_elements() == sims_write.num_elements());
    for (size_t dim0 = 0; dim0 < sims_read.shape()[0]; dim0++) {
        for (size_t dim1 = 0; dim1 < sims_read.shape()[1]; dim1++) {
            for (size_t dim2 = 0; dim2 < sims_read.shape()[2]; dim2++) {
                for (size_t dim3 = 0; dim3 < sims_read.shape()[3]; dim3++) {
                    for (size_t dim4 = 0; dim4 < sims_read.shape()[4]; dim4++) {
                        CPPUNIT_ASSERT(sims_read[dim0][dim1][dim2][dim3][dim4]
                                == sims_write[dim0][dim1][dim2][dim3][dim4]);
                    }
                }
            }
        }
    }

    remove(file_path.c_str());
}

void testAnEnIO::testReadWriteAnalogs() {

    /**
     * Test functions for reading and writing analogs
     */

    Analogs analogs_write(10, 20, 15, 11);
    generate(analogs_write.data(), analogs_write.data() +
            analogs_write.num_elements(), rand);

    string file_path = "read-write-analogs.nc";
    remove(file_path.c_str());

    AnEnIO io("Write", file_path, "Analogs", 1);
    io.writeAnalogs(analogs_write);

    Analogs analogs_read;
    io.setMode("Read");
    io.readAnalogs(analogs_read);
    
    for (size_t i = 0; i < analogs_read.num_elements(); i++)
        CPPUNIT_ASSERT(analogs_read.data()[i] == analogs_write.data()[i]);
    
    remove(file_path.c_str());
}

void testAnEnIO::testReadWriteTextMatrix() {
    
    /**
     * Test writing and reading text matrix
     */
    string file_path = "read-write-text-matrix.txt";
    remove(file_path.c_str());
    
    boost::numeric::ublas::matrix<size_t> mat_write(5, 10);
    
    for (size_t i = 0; i < mat_write.size1(); i++)
        for (size_t j = 0; j < mat_write.size2(); j++)
            mat_write(i, j) = i * 1000 + j;
    
    
    AnEnIO io("Write", file_path, "Matrix", 1);
    io.writeTextMatrix(mat_write);
    
    boost::numeric::ublas::matrix<size_t> mat_read;
    io.setMode("Read");
    io.readTextMatrix(mat_read);
    
    CPPUNIT_ASSERT(mat_read.size1() == mat_write.size1());
    CPPUNIT_ASSERT(mat_read.size2() == mat_write.size2());

    for (size_t i = 0; i < mat_write.size1(); i++)
        for (size_t j = 0; j < mat_write.size2(); j++)
            CPPUNIT_ASSERT(mat_read(i, j) == mat_write(i, j));
    
    remove(file_path.c_str());
}

void testAnEnIO::testReadWriteStandardDeviation() {
    
    /**
     * Test writing and reading StandardDeviation
     */

    string file_path = "read-write-standard-deviation.nc";
    remove(file_path.c_str());
    
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
    flts_write.insert(flts_write.end(),{100, 200, 300, 400, 500});

    vector<double> values_write(parameters_write.size()
            * stations_write.size() * times_write.size() * flts_write.size());
    generate(values_write.begin(), values_write.end(), rand);

    Forecasts_array forecasts_write(
            parameters_write, stations_write,
            times_write, flts_write, values_write);
    
    // Create standard deviation
    StandardDeviation sds_write;
    StandardDeviation::extent_gen extents;
    sds_write.resize(extents[4][6][5]);

    for (size_t i = 0; i < 4; i++)
        for (size_t j = 0; j < 6; j++)
            for (size_t k = 0; k < 5; k++)
                sds_write[i][j][k] = i * 100 + j * 10 + k;

    AnEnIO io("Write", file_path, "StandardDeviation", 2);
    io.writeStandardDeviation(sds_write, forecasts_write.getParameters());

    StandardDeviation sds_read;
    io.setMode("Read");
    io.readStandardDeviation(sds_read);

    CPPUNIT_ASSERT(sds_read.shape()[0] == sds_write.shape()[0]);
    CPPUNIT_ASSERT(sds_read.shape()[1] == sds_write.shape()[1]);
    CPPUNIT_ASSERT(sds_read.shape()[2] == sds_write.shape()[2]);
    
    for (size_t i = 0; i < 4; i++)
        for (size_t j = 0; j < 6; j++)
            for (size_t k = 0; k < 5; k++)
                sds_write[i][j][k] = sds_read[i][j][k];

    remove(file_path.c_str());
}
