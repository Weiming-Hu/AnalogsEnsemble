/*
 * File:   testStations.cpp
 * Author: Weiming Hu (Weiming Hu)
 *
 * Created on May 30, 2018, 2:40:21 PM
 */

#include <cppunit/TestAssert.h>

#include "testStations.h"

#include <iostream>

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION(testStations);

testStations::testStations() {
}

testStations::~testStations() {
}

void
testStations::setUp() {
}

void
testStations::tearDown() {
}

void
testStations::testUnique() {

    /*
     * Station in Stations should be unique.
     */

    Station s0;

    Station s1("station2");
    Station s2("station3", 10, 20);
    Station s3("station2", 10, 20);
    Station s4("station2");
    Station s5 = s3;
    Station s6(s1);

    Stations stations;

    // All stations should be able to be inserted into Stations.
    CPPUNIT_ASSERT(stations.insert(s0).second);
    CPPUNIT_ASSERT(stations.insert(s5).second);
    CPPUNIT_ASSERT(stations.insert(s6).second);
    CPPUNIT_ASSERT(stations.insert(s4).second);
    CPPUNIT_ASSERT(stations.insert(s2).second);
    
    // s3 is a duplicate of s5.
    CPPUNIT_ASSERT(!stations.insert(s3).second);
    
    // s1 is a duplicate of s6.
    CPPUNIT_ASSERT(!stations.insert(s1).second);
    
    CPPUNIT_ASSERT(stations.size() == 5);

    // No stations should be able to be inserted into Stations.
    CPPUNIT_ASSERT(!stations.insert(s0).second);
    CPPUNIT_ASSERT(!stations.insert(s5).second);
    CPPUNIT_ASSERT(!stations.insert(s6).second);
    CPPUNIT_ASSERT(!stations.insert(s4).second);
    CPPUNIT_ASSERT(!stations.insert(s2).second);
    CPPUNIT_ASSERT(stations.size() == 5);

}