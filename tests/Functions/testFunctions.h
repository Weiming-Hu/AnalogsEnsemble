/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   testFunctions.h
 * Author: wuh20
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
};

#endif /* TESTFUNCTIONS_H */

