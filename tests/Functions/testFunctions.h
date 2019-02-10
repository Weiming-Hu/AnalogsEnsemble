/*
 * File:   testFunctions.h
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on Feb 7, 2019, 2:28:24 PM
 */

#ifndef TESTFUNCTIONS_H
#define TESTFUNCTIONS_H

#include <cppunit/extensions/HelperMacros.h>

class testFunctions : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(testFunctions);

    CPPUNIT_TEST(testComputeObservationTimeIndices);
    CPPUNIT_TEST(testComputeStandardDeviation);
    CPPUNIT_TEST(testComputeSearchWindows);
    CPPUNIT_TEST(testConvertToIndex);
    CPPUNIT_TEST(testSdCircular);
    CPPUNIT_TEST(testSdLinear);
    CPPUNIT_TEST(testMean);

    CPPUNIT_TEST_SUITE_END();

public:
    testFunctions();
    virtual ~testFunctions();
    void setUp();
    void tearDown();

private:

    void testComputeObservationTimeIndices();
    void testComputeStandardDeviation();
    void testComputeSearchWindows();
    void testConvertToIndex();
    void testSdCircular();
    void testSdLinear();
    void testMean();
    
};

#endif /* TESTFUNCTIONS_H */

