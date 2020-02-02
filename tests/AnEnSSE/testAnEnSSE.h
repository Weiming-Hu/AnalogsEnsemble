/*
 * File:   testAnEnSSE.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on Aug 4, 2018, 4:09:20 PM
 */

#ifndef TESTANEN_H
#define TESTANEN_H

#include "AnEnSSE.h"
#include "Forecasts.h"
#include "Observations.h"

#include <cppunit/extensions/HelperMacros.h>

class testAnEnSSE : public CPPUNIT_NS::TestFixture, public AnEnSSE {
    CPPUNIT_TEST_SUITE(testAnEnSSE);

    //CPPUNIT_TEST(compareComputeOperational_);

    CPPUNIT_TEST_SUITE_END();

public:
    testAnEnSSE();
    virtual ~testAnEnSSE();

private:
    
    ///// TODO Working on AnEnSSE test !!!
};

#endif /* TESTANEN_H */
