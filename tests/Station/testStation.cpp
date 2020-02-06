/*
 * File:   testStation.cpp
 * Author: Weiming Hu (weiming@psu.edu)
 *
 * Created on May 30, 2018, 1:39:24 PM
 */

#include "testStation.h"
#include "Stations.h"

#include <ostream>
using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION(testStation);

testStation::testStation() {
}

testStation::~testStation() {
}

void
testStation::testStationComparison_() {

    Station s1(10, 20, "station1");
    Station s2(10, 20, "station1");
    
    CPPUNIT_ASSERT(s1 == s2);
    CPPUNIT_ASSERT(!(s1 < s2));
    
    Station s3(20, 0, "a");
    CPPUNIT_ASSERT(s1 != s3);
    CPPUNIT_ASSERT(s1 < s3);
    
    Station s4(10, 30, "a");
    CPPUNIT_ASSERT(s1 != s4);
    CPPUNIT_ASSERT(s1 < s4);
    
    Station s5(10, 20, "stb");
    CPPUNIT_ASSERT(s1 != s5);
    CPPUNIT_ASSERT(s1 < s5);
}

void
testStation::testStationAssignment_() {

    /*
     * When a station is assigned to another station, all member variables
     * should be copied. 
     */

    Station s1(10, 20, "station1");
    Station s2 = s1;

    CPPUNIT_ASSERT(s1.getName() == s2.getName());
    CPPUNIT_ASSERT(s1.getX() == s2.getX());
    CPPUNIT_ASSERT(s1.getY() == s2.getY());
}