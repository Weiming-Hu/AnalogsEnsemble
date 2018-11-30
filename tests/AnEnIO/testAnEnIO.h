/*
 * File:   testAnEnIO.h
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on Jun 29, 2018, 6:02:11 PM
 */

#ifndef TESTANENIO_H
#define TESTANENIO_H

#include <cppunit/extensions/HelperMacros.h>

#include "AnEnIO.h"
#include <string>

// I cannot pass argument through command lines therefore I have to 
// use macro definitions to tell where the files are and then 
// the program can correctly read them.
//
#ifndef _PATH_OBSERVATIONS 
#define _PATH_OBSERVATIONS "tests/test_observations.nc"
#define _PATH_FORECASTS "tests/test_forecasts.nc"
#endif

class testAnEnIO : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(testAnEnIO);

    CPPUNIT_TEST(testReadObservationFile);
    CPPUNIT_TEST(testReadForecastFile);
    CPPUNIT_TEST(testWriteReadObservationFile);
    CPPUNIT_TEST(testWriteReadForecastFile);
    CPPUNIT_TEST(testReadPartParameters);
    CPPUNIT_TEST(testReadPartStations);
    CPPUNIT_TEST(testReadPartTimes);
    CPPUNIT_TEST(testReadPartFLTs);
    CPPUNIT_TEST(testReadPartObservations);
    CPPUNIT_TEST(testReadPartForecasts);
    CPPUNIT_TEST(testReadWriteSimilarityMatrices);
    CPPUNIT_TEST(testReadWriteAnalogs);
    CPPUNIT_TEST(testReadWriteTextMatrix);
    CPPUNIT_TEST(testReadWriteStandardDeviation);

    CPPUNIT_TEST_SUITE_END();

public:
    testAnEnIO();
    virtual ~testAnEnIO();
    void setUp();
    void tearDown();

    std::string file_observations = _PATH_OBSERVATIONS;
    std::string file_forecasts = _PATH_FORECASTS;

private:
    void testReadObservationFile();
    void testReadForecastFile();
    void testWriteReadObservationFile();
    void testWriteReadForecastFile();
    void testReadPartParameters();
    void testReadPartStations();
    void testReadPartTimes();
    void testReadPartFLTs();
    void testReadPartObservations();
    void testReadPartForecasts();
    void testReadWriteSimilarityMatrices();
    void testReadWriteAnalogs();
    void testReadWriteTextMatrix();
    void testReadWriteStandardDeviation();

};

#endif /* TESTANENIO_H */

