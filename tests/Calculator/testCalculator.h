/*
 * File:   testCalculator.h
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on Jan 17, 2020, 10:12:56 AM
 */

#ifndef TESTCALCULATOR_H
#define TESTCALCULATOR_H

#include <cppunit/extensions/HelperMacros.h>

class testCalculator : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(testCalculator);

    CPPUNIT_TEST(testMean);
    CPPUNIT_TEST(testVariance);
    CPPUNIT_TEST(testSd);

    CPPUNIT_TEST_SUITE_END();

public:
    testCalculator();
    virtual ~testCalculator();
    void setUp();
    void tearDown();

private:
    void testMean();
    void testVariance();
    void testSd();
};

#endif /* TESTCALCULATOR_H */

