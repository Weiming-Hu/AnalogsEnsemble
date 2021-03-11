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

    CPPUNIT_TEST(testMatchStations_);

    CPPUNIT_TEST_SUITE_END();

public:
    testAnEnSSEMS();
    virtual ~testAnEnSSEMS();

private:
    void testMatchStations_();
};

#endif /* TESTANEN_H */
