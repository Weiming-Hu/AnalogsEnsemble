/*
 * File:   testAnEnSSEMS.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on Mar. 10, 2021, 17:13
 */

#ifndef TESTANEN_H
#define TESTANEN_H

#include "AnEnSSEMS.h"
#include "Forecasts.h"
#include "Observations.h"

#include <cppunit/extensions/HelperMacros.h>

class testAnEnSSEMS : public CPPUNIT_NS::TestFixture, public AnEnSSEMS {
    CPPUNIT_TEST_SUITE(testAnEnSSEMS);

    CPPUNIT_TEST(testAnEn_);
    CPPUNIT_TEST(testOperation_);
    CPPUNIT_TEST(testFutureSearch_);
    CPPUNIT_TEST(testManualMatch_);

    CPPUNIT_TEST_SUITE_END();

public:
    testAnEnSSEMS();
    virtual ~testAnEnSSEMS();

    void setUp();
    void tearDown();

    void setUpAnEn();

private:
    Parameters parameters_;
    Stations obs_stations_;
    Stations fcst_stations_;
    Times fcst_times_;
    Times obs_times_;
    Times flts_;
    Config config_;

    void testAnEn_();
    void testOperation_();
    void testFutureSearch_();
    void testManualMatch_();

    void coreProcedures_();
};

#endif /* TESTANEN_H */
