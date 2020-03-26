/*
 * File:   testFunctions.h
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on Feb 7, 2019, 2:28:24 PM
 */

#ifndef TESTFUNCTIONS_H
#define TESTFUNCTIONS_H

#include <cppunit/extensions/HelperMacros.h>
#include "Functions.h"

class testFunctions : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(testFunctions);

    CPPUNIT_TEST(showNumThreads_);
    CPPUNIT_TEST(testSearchStations_);
    CPPUNIT_TEST(testComputeObservationTimeIndices1_);
    CPPUNIT_TEST(testComputeObservationTimeIndices2_);
    CPPUNIT_TEST(testCollapseLeadTimes_);
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

    void showNumThreads_();
    void testSearchStations_();
    void testComputeObservationTimeIndices1_();
    void testComputeObservationTimeIndices2_();
    void testCollapseLeadTimes_();
    void testConvertToIndex_();
    void testSdCircular_();
    void testSdLinear_();
    void testMean_();
    
    bool neighborExists_(const Functions::Matrix & table,
            size_t test_index, size_t neighbor_index) const;
};

#endif /* TESTFUNCTIONS_H */

