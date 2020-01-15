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

    CPPUNIT_TEST(testComputeObservationTimeIndices1_);
    CPPUNIT_TEST(testComputeObservationTimeIndices2_);
    CPPUNIT_TEST(testConvertToIndex_);
    CPPUNIT_TEST(testSdCircular_);
    CPPUNIT_TEST(testSdLinear_);
    CPPUNIT_TEST(testMean_);

    CPPUNIT_TEST_SUITE_END();

public:
    testFunctions();
    virtual ~testFunctions();
    void setUp();
    void tearDown();

    std::string file_observations = _PATH_OBSERVATIONS;
    std::string file_forecasts = _PATH_FORECASTS;

private:

    void testComputeObservationTimeIndices1_();
    void testComputeObservationTimeIndices2_();
    void testConvertToIndex_();
    void testSdCircular_();
    void testSdLinear_();
    void testMean_();
    
};

#endif /* TESTFUNCTIONS_H */

