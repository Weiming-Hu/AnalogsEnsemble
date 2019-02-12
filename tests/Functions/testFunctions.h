/*
 * File:   testFunctions.h
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on Feb 7, 2019, 2:28:24 PM
 */

#ifndef TESTFUNCTIONS_H
#define TESTFUNCTIONS_H

#include <cppunit/extensions/HelperMacros.h>

// I cannot pass argument through command lines therefore I have to 
// use macro definitions to tell where the files are and then 
// the program can correctly read them.
//
#ifndef _PATH_OBSERVATIONS 
#define _PATH_OBSERVATIONS "tests/test_observations.nc"
#define _PATH_FORECASTS "tests/test_forecasts.nc"
#endif

class testFunctions : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(testFunctions);

    CPPUNIT_TEST(testComputeObservationTimeIndices1);
    CPPUNIT_TEST(testComputeObservationTimeIndices2);
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

    std::string file_observations = _PATH_OBSERVATIONS;
    std::string file_forecasts = _PATH_FORECASTS;

private:

    void testComputeObservationTimeIndices1();
    void testComputeObservationTimeIndices2();
    void testComputeStandardDeviation();
    void testComputeSearchWindows();
    void testConvertToIndex();
    void testSdCircular();
    void testSdLinear();
    void testMean();
    
};

#endif /* TESTFUNCTIONS_H */

