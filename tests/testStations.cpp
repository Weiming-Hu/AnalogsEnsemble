/*
 * File:   testStations.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on May 30, 2018, 2:40:21 PM
 */

#include <cppunit/TestAssert.h>

#include "testStations.h"

#include <iostream>

using namespace std;
using namespace anenSta;

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
    CPPUNIT_ASSERT(stations.push_back(s0).second);
    CPPUNIT_ASSERT(stations.push_back(s5).second);
    CPPUNIT_ASSERT(stations.push_back(s6).second);
    CPPUNIT_ASSERT(stations.push_back(s4).second);
    CPPUNIT_ASSERT(stations.push_back(s2).second);
    
    // s3 is a duplicate of s5.
    CPPUNIT_ASSERT(!stations.push_back(s3).second);
    
    // s1 is a duplicate of s6.
    CPPUNIT_ASSERT(!stations.push_back(s1).second);
    
    CPPUNIT_ASSERT(stations.size() == 5);

    // No stations should be able to be inserted into Stations.
    CPPUNIT_ASSERT(!stations.push_back(s0).second);
    CPPUNIT_ASSERT(!stations.push_back(s5).second);
    CPPUNIT_ASSERT(!stations.push_back(s6).second);
    CPPUNIT_ASSERT(!stations.push_back(s4).second);
    CPPUNIT_ASSERT(!stations.push_back(s2).second);
    CPPUNIT_ASSERT(stations.size() == 5);
}

void 
testStations::testMultiIndex() {
    
    /*
     * Stations should be able to index Station in different orders.
     */
    
    Station s0("station0");
    Station s1("station1", 10, 20);
    Station s2("station2", 10, 20);
    Station s3("station3");
    Station s4 = s3;
    Station s5(s1);
    
    vector<Station> vec {s0, s1, s2, s3, s4, s5};
    
    Stations stations;
    stations.insert(stations.end(), vec.begin(), vec.end());
    
    CPPUNIT_ASSERT(stations.size() == 4);
    
    // Test insertion sequence
    multiIndexStations::index<by_insert>::type&
            stations_by_insert = stations.get<by_insert>();
    
    CPPUNIT_ASSERT(stations_by_insert[0].getName() == "station0");
    CPPUNIT_ASSERT(stations_by_insert[1].getName() == "station1");
    CPPUNIT_ASSERT(stations_by_insert[2].getName() == "station2");
    CPPUNIT_ASSERT(stations_by_insert[3].getName() == "station3");
    
    // Test order by ID
    multiIndexStations::index<by_ID>::type&
            stations_by_ID = stations.get<by_ID>();
    
    CPPUNIT_ASSERT(stations_by_ID.find(s0.getID())->getName() == "station0");
    CPPUNIT_ASSERT(stations_by_ID.find(s1.getID())->getName() == "station1");
    CPPUNIT_ASSERT(stations_by_ID.find(s2.getID())->getName() == "station2");
    CPPUNIT_ASSERT(stations_by_ID.find(s3.getID())->getName() == "station3");
}

void
testStations::testGetIndex() {
    
    /*
     * Test function getStationIndex
     */
    
    Station s0("station0");
    Station s1("station1", 10, 20);
    Station s2("station2", 10, 20);
    Station s3("station3");
    Station s4 = s3;
    Station s5(s1);
    
    // Test if there is duplicate
    vector<Station> vec {s0, s3, s4, s5, s1, s2};
    
    Stations stations;
    stations.insert(stations.end(), vec.begin(), vec.end());

    CPPUNIT_ASSERT(stations.getStationIndex(s0.getID()) == 0);
    CPPUNIT_ASSERT(stations.getStationIndex(s3.getID()) == 1);
    CPPUNIT_ASSERT(stations.getStationIndex(s5.getID()) == 2);
    CPPUNIT_ASSERT(stations.getStationIndex(s2.getID()) == 3);

    // Test if there is no duplicate
    vec =  {s3, s2, s1, s0};
    stations.clear();
    stations.insert(stations.end(), vec.begin(), vec.end());
    
    CPPUNIT_ASSERT(stations.getStationIndex(s0.getID()) == 3);
    CPPUNIT_ASSERT(stations.getStationIndex(s1.getID()) == 2);
    CPPUNIT_ASSERT(stations.getStationIndex(s2.getID()) == 1);
    CPPUNIT_ASSERT(stations.getStationIndex(s3.getID()) == 0);
}