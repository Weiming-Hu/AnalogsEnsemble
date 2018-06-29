/*
 * File:   testObservationsArray.h
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on Jun 29, 2018, 1:36:09 PM
 */

#ifndef TESTOBSERVATIONSARRAY_H
#define TESTOBSERVATIONSARRAY_H

#include <cppunit/extensions/HelperMacros.h>
#include "../Observations.h"

class testObservationsArray : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(testObservationsArray);

    CPPUNIT_TEST(testTraverseTime);

    CPPUNIT_TEST_SUITE_END();

public:
    testObservationsArray();
    virtual ~testObservationsArray();
    void setUp();
    void tearDown();

private:
    void testTraverseTime();
};

#endif /* TESTOBSERVATIONSARRAY_H */

