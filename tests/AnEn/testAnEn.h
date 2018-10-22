/*
 * File:   testAnEn.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on Aug 4, 2018, 4:09:20 PM
 */

#ifndef TESTANEN_H
#define TESTANEN_H

#include <cppunit/extensions/HelperMacros.h>

#include "AnEn.h"

class testAnEn : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(testAnEn);

    CPPUNIT_TEST(testComputeStandardDeviation);
    CPPUNIT_TEST(testComputeSearchWindows);
    CPPUNIT_TEST(testComputeSimilarity);
    CPPUNIT_TEST(testSdCircular);
    CPPUNIT_TEST(testSdLinear);
    CPPUNIT_TEST(testComputeObservationTimeIndices);
    CPPUNIT_TEST(testSelectAnalogs);
    CPPUNIT_TEST(testComputeSearchStations);
    CPPUNIT_TEST(testOpenMP);
    CPPUNIT_TEST(testMean);

    CPPUNIT_TEST_SUITE_END();

public:
    testAnEn();
    virtual ~testAnEn();

private:
    void testComputeStandardDeviation();
    void testComputeSearchWindows();
    void testComputeSimilarity();
    void testSdCircular();
    void testMean();
    void testSdLinear();
    void testComputeObservationTimeIndices();
    void testSelectAnalogs();
    void testComputeSearchStations();
    void testOpenMP();
};

#endif /* TESTANEN_H */

