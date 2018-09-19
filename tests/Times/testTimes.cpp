/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   testTimes.cpp
 * Author: wuh20
 *
 * Created on Aug 5, 2018, 12:57:17 PM
 */

#include <cppunit/TestAssert.h>

#include "testTimes.h"


CPPUNIT_TEST_SUITE_REGISTRATION(testTimes);

using namespace anenTime;

testTimes::testTimes() {
}

testTimes::~testTimes() {
}

void testTimes::testGetTimeIndex() {

    /**
     * Test function getTimeIndex.
     */

    Times times;
    times.insert(times.end(),{100, 105, 110, 115, 120,
        200, 205, 210, 215, 220, 300, 305, 310, 315, 320});

    size_t i_time;
    i_time = times.getTimeIndex(110);
    CPPUNIT_ASSERT(i_time == 2);
    i_time = times.getTimeIndex(320);
    CPPUNIT_ASSERT(i_time == 14);
}
