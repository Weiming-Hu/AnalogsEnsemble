/*
 * File:   testStation.cpp
 * Author: Weiming Hu (weiming@psu.edu)
 *
 * Created on May 30, 2018, 1:39:24 PM
 */

#include "testStation.h"

#include <ostream>
using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION(testStation);

testStation::testStation() {
}

testStation::~testStation() {
}

void
testStation::setUp() {
}

void
testStation::tearDown() {
}

void
testStation::testStationComparison() {

    /*
     * Stations should be compared solely based on their IDs.
     */

    Station s1("station1", 10, 20);
    Station s2("station1", 10, 20);

    CPPUNIT_ASSERT(!(s1 == s2));
    CPPUNIT_ASSERT(s1 < s2);
}

void
testStation::testStationAssignment() {

    /*
     * When a station is assigned to another station, all member variables
     * except IDs should be copied. 
     */

    Station s1("station1", 10, 20);
    Station s2 = s1;

    CPPUNIT_ASSERT(s1.getName() == s2.getName());
    CPPUNIT_ASSERT(s1.getX() == s2.getX());
    CPPUNIT_ASSERT(s1.getY() == s2.getY());
    CPPUNIT_ASSERT(s1.getID() == s2.getID());

    // The ID count should be 3 because 2 were created in previous tests,
    // and only 1 new objects are created here.
    //
    CPPUNIT_ASSERT(Station::_countIDs() == 3);
}