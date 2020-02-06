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

void testCalculator::testLinearMean() {

    /*
     * Test the calculator of linear averages
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
    CPPUNIT_ASSERT(abs(calc.variance() - 1250.5) < 1e-4);

    calc.pushValue(20);
    CPPUNIT_ASSERT(abs(calc.variance() - 1633.767) < 1e-3);

    calc.pushValue(NAN);
    CPPUNIT_ASSERT(std::isnan(calc.variance()));
}


void testCalculator::testLinearSd() {
    
    /*
     * Test the calculator of linear standard deviations
     */
    
    Calculator calc;

    CPPUNIT_ASSERT(std::isnan(calc.sd()));

    calc.pushValue(100.01);
    CPPUNIT_ASSERT(std::isnan(calc.sd()));

    calc.pushValue(50);
    CPPUNIT_ASSERT(abs(calc.sd() - 35.36241) < 1e-6);

    calc.pushValue(30);
    CPPUNIT_ASSERT(abs(calc.sd() - 36.06106) < 1e-6);

    calc.pushValue(NAN);
    CPPUNIT_ASSERT(std::isnan(calc.sd()));
}

void testCalculator::testCircularMean() {
    
    /*
     * Test the calculation of circular averages
     */
    
    Calculator calc;
    calc.setCircular(true);

    CPPUNIT_ASSERT(std::isnan(calc.mean()));

    calc.pushValue(350);
    calc.pushValue(10);
    CPPUNIT_ASSERT(abs(calc.mean() - 0) < 1e-6);

    calc.clearValues();
    calc.pushValue(90);
    calc.pushValue(180);
    calc.pushValue(270);
    calc.pushValue(360);
    CPPUNIT_ASSERT(abs(calc.mean() + 90) < 1e-6);

    calc.reset();
    CPPUNIT_ASSERT(!calc.isCircular());
    calc.setCircular(true);
    calc.pushValue(10);
    calc.pushValue(20);
    calc.pushValue(30);
    CPPUNIT_ASSERT(abs(calc.mean() - 20) < 1e-6);
}

void testCalculator::testCircularSd() {
    
    /*
     * Test the calculation of circular standard deviations
     */
    Calculator calc;
    calc.setCircular(true);

    CPPUNIT_ASSERT(std::isnan(calc.sd()));

    calc.pushValue(350);
    calc.pushValue(10);
    CPPUNIT_ASSERT(abs(calc.sd() - 10.0081) < 1e-4);

    calc.clearValues();
    calc.pushValue(90);
    calc.pushValue(180);
    calc.pushValue(270);
    calc.pushValue(360);
    CPPUNIT_ASSERT(abs(calc.sd() - 103.923) < 1e-3);

    calc.reset();
    CPPUNIT_ASSERT(!calc.isCircular());
    calc.setCircular(true);
    calc.pushValue(10);
    calc.pushValue(20);
    calc.pushValue(30);
    CPPUNIT_ASSERT(abs(calc.sd() - 8.165117) < 1e-6);
}

