/*
 * File:   testStation.h
 * Author: Weiming Hu (weiming@psu.edu)
 *
 * Created on May 30, 2018, 1:39:24 PM
 */

#ifndef TESTSTATION_H
#define TESTSTATION_H

#include <cppunit/extensions/HelperMacros.h>

#include "Stations.h"

class testStation : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(testStation);

    // Add our tests here
    CPPUNIT_TEST(testStationComparison_);
    CPPUNIT_TEST(testStationAssignment_);

    CPPUNIT_TEST_SUITE_END();

public:
    testStation();
    virtual ~testStation();

private:
    void testStationComparison_();
    void testStationAssignment_();
};

#endif /* TESTSTATION_H */

