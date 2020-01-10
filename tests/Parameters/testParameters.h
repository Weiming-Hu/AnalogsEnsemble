/*
 * File:   testParameters.h
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on Jan 10, 2020, 4:00:11 PM
 */

#ifndef TESTPARAMETERS_H
#define TESTPARAMETERS_H

#include <cppunit/extensions/HelperMacros.h>

class testParameters : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(testParameters);

    CPPUNIT_TEST(testCreation);
    CPPUNIT_TEST(testUnique);

    CPPUNIT_TEST_SUITE_END();

public:
    testParameters();
    virtual ~testParameters();

    void testCreation();
    void testUnique();
};

#endif /* TESTPARAMETERS_H */

