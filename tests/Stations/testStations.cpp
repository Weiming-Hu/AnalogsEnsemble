/*
 * File:   testStations.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on May 30, 2018, 2:40:21 PM
 */

#include <cppunit/TestAssert.h>
#include <iostream>
#include <algorithm>

#include "testStations.h"
#include "Stations.h"
#include "boost/lambda/lambda.hpp"
#include "boost/assign/list_of.hpp"
#include "boost/assign/list_inserter.hpp"

using namespace std;
using namespace boost::bimaps;
using namespace boost;

CPPUNIT_TEST_SUITE_REGISTRATION(testStations);

testStations::testStations() {
}

testStations::~testStations() {
}

void
testStations::testUnique() {

    /*
     * Station in Stations should be unique.
     */

    // Unique stations
    Station s0;
    Station s1(5, 3);
    Station s2(10, 20, "station3");
    Station s3(10, 20, "station2");
    
    // Duplicated stations
    Station s4(10, 20, "station2");
    Station s5 = s3;
    Station s6(s1);

    Stations stations;

    // These stations should be able to be inserted into Stations.
    assign::push_back(stations.left)(0, s0)(1, s1)(2, s2)(3, s3);
    CPPUNIT_ASSERT(stations.size() == 4);
    cout << "After inserting the unique stations: " << stations;

    // These stations will not be able to be inserted
    assign::push_back(stations.left)(4, s4)(5, s5)(6, s6);
    CPPUNIT_ASSERT(stations.size() == 4);
    
    // The stations still remain the same because the previous insertion
    // had no effect to the container.
    //
    CPPUNIT_ASSERT(stations.left[0].second == s0);
    CPPUNIT_ASSERT(stations.left[1].second == s1);
    CPPUNIT_ASSERT(stations.left[2].second == s2);
    CPPUNIT_ASSERT(stations.left[3].second == s3);
}