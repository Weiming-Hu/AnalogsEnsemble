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

    CPPUNIT_TEST(testLinearMean);
    CPPUNIT_TEST(testVariance);
    CPPUNIT_TEST(testLinearSd);
    CPPUNIT_TEST(testCircularMean);
    CPPUNIT_TEST(testCircularSd);

    CPPUNIT_TEST_SUITE_END();

public:
    testCalculator();
    virtual ~testCalculator();
    void setUp();
    void tearDown();

private:
    void testLinearMean();
    void testVariance();
    void testLinearSd();
    void testCircularMean();
    void testCircularSd();
};

#endif /* TESTCALCULATOR_H */

