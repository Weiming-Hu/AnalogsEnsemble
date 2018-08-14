/*
 * File:   testAnEnIO.h
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on Jun 29, 2018, 6:02:11 PM
 */

#ifndef TESTANENIO_H
#define TESTANENIO_H

#include <cppunit/extensions/HelperMacros.h>

#include "../AnEnIO.h"

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

    CPPUNIT_TEST_SUITE_END();

public:
    testAnEnIO();
    virtual ~testAnEnIO();
    void setUp();
    void tearDown();

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
};

#endif /* TESTANENIO_H */

