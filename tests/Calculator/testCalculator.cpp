/*
 * File:   testCalculator.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on Jan 17, 2020, 10:12:57 AM
 */

#include "testCalculator.h"
#include "Calculator.h"

#include <cmath>

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION(testCalculator);

testCalculator::testCalculator() {
}

testCalculator::~testCalculator() {
}

void testCalculator::setUp() {
}

void testCalculator::tearDown() {
}

void testCalculator::testMean() {

    /*
     * Test the calculator of averages
     */

    Calculator calc;

    CPPUNIT_ASSERT(std::isnan(calc.mean()));

    calc.pushValue(100.01);
    CPPUNIT_ASSERT(calc.mean() == 100.01);

    calc.pushValue(50);
    CPPUNIT_ASSERT(calc.mean() == 75.005);

    calc.pushValue(NAN);
    CPPUNIT_ASSERT(std::isnan(calc.mean()));
}

void testCalculator::testVariance() {

    /*
     * Test the calculator of variances
     */

    Calculator calc;

    CPPUNIT_ASSERT(std::isnan(calc.variance()));

    calc.pushValue(100.01);
    CPPUNIT_ASSERT(std::isnan(calc.variance()));

    calc.pushValue(50);
    CPPUNIT_ASSERT(calc.variance() - 1250.5 < 1e-4);

    calc.pushValue(20);
    CPPUNIT_ASSERT(calc.variance() - 1633.767 < 1e-4);

    calc.pushValue(NAN);
    CPPUNIT_ASSERT(std::isnan(calc.variance()));
}


void testCalculator::testSd() {
    
    /*
     * Test the calculator of standard deviations
     */
    
    Calculator calc;

    CPPUNIT_ASSERT(std::isnan(calc.sd()));

    calc.pushValue(100.01);
    CPPUNIT_ASSERT(std::isnan(calc.sd()));

    calc.pushValue(50);
    CPPUNIT_ASSERT(calc.sd() * 1000 - 353624 < 1e-4);

    calc.pushValue(30);
    CPPUNIT_ASSERT(calc.sd() - 360610 < 1e-4);

    calc.pushValue(NAN);
    CPPUNIT_ASSERT(std::isnan(calc.sd()));
    
}