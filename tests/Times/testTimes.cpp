/*
 * File:   testTimes.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on Aug 5, 2018, 12:57:17 PM
 */

#include <cppunit/TestAssert.h>
#include <iostream>

#include "testTimes.h"
#include "Times.h"
#include "boost/assign/list_of.hpp"
#include "boost/assign/list_inserter.hpp"

using namespace std;
using namespace boost::bimaps;
using namespace boost;

CPPUNIT_TEST_SUITE_REGISTRATION(testTimes);

testTimes::testTimes() {
}

testTimes::~testTimes() {
}

void testTimes::testGetTimeIndex_() {

    /**
     * Test function getIndex.
     */

    Times times;
    assign::push_back(times.left)(0, Time(100))(1, Time(200))
            (2, Time(300))(3, Time(400))(4, Time(500));
    
    CPPUNIT_ASSERT(times.getIndex(Time(400)) == 3);
    CPPUNIT_ASSERT(times.getIndex(Time(500)) == 4);
    CPPUNIT_ASSERT(times.getIndex(Time(300)) == 2);
}