/*
 * File:   testAnEnIS.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on Aug 4, 2018, 4:09:20 PM
 */

#ifndef TESTANEN_H
#define TESTANEN_H

#include "AnEnIS.h"
#include "ForecastsArray.h"
#include "ObservationsArray.h"

#include <cppunit/extensions/HelperMacros.h>

class testAnEnIS : public CPPUNIT_NS::TestFixture, public AnEnIS {
    CPPUNIT_TEST_SUITE(testAnEnIS);

    //    CPPUNIT_TEST(testAutomaticDelteOverlappingTimes);
    //    CPPUNIT_TEST(testCompute);
    //    CPPUNIT_TEST(testOperationalSearch);
    //    CPPUNIT_TEST(testLeaveOneOut);
    //    CPPUNIT_TEST(testOpenMP);
    CPPUNIT_TEST(testFixedLengthSds_);
    CPPUNIT_TEST(compareOperationalSds_);

    CPPUNIT_TEST_SUITE_END();

public:
    testAnEnIS();
    virtual ~testAnEnIS();
    
    void setUp();
    void tearDown();
    
    void setUpSds();
    void tearDownSds();

private:
    Parameters parameters_;
    Stations stations_;
    Times times_;
    Times flts_;
    
    void testFixedLengthSds_();
    void compareOperationalSds_();
    //    void testAutomaticDeleteOverlappingTimes();
    //    void testCompute();
    //    void testOperationalSearch();
    //    void testLeaveOneOut();
    //    void testOpenMP();
};

#endif /* TESTANEN_H */

