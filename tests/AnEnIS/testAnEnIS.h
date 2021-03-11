/*
 * File:   testAnEnIS.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on Aug 4, 2018, 4:09:20 PM
 */

#ifndef TESTANEN_H
#define TESTANEN_H

#include "AnEnIS.h"
#include "Forecasts.h"
#include "Observations.h"

#include <cppunit/extensions/HelperMacros.h>

class testAnEnIS : public CPPUNIT_NS::TestFixture, public AnEnIS {
    CPPUNIT_TEST_SUITE(testAnEnIS);

    CPPUNIT_TEST(testOpenMP_);
    CPPUNIT_TEST(testMultiAnEn_);
    CPPUNIT_TEST(testFixedLengthSds_);
    CPPUNIT_TEST(compareOperationalSds_);
    CPPUNIT_TEST(compareComputeLeaveOneOut_);
    CPPUNIT_TEST(compareComputeOperational_);

    CPPUNIT_TEST_SUITE_END();

public:
    testAnEnIS();
    virtual ~testAnEnIS();

    void setUp();
    void tearDown();

    void setUpSds();
    void tearDownSds();

    void setUpCompute();
    void tearDownCompute();

private:
    Parameters parameters_;
    Stations stations_;
    Times fcst_times_;
    Times obs_times_;
    Times flts_;

    void testOpenMP_();
    void testMultiAnEn_();
    void testFixedLengthSds_();
    void compareOperationalSds_();
    void compareComputeOperational_();
    void compareComputeLeaveOneOut_();
};

#endif /* TESTANEN_H */
