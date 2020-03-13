/*
 * File:   testStations.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on May 30, 2018, 2:40:21 PM
 */

#include <cppunit/TestAssert.h>
#include <iostream>
#include <stdexcept>
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
testStations::testShift_() {
    
    /*
     * Test shifting station longitudes
     */

    Stations stations1;
    stations1.push_back(Station(100, 40));
    stations1.push_back(Station(361, 40));
    CPPUNIT_ASSERT_THROW(stations1.shiftLongitudes(), runtime_error);

    Stations stations2;
    stations2.push_back(Station(100, 40));
    stations2.push_back(Station(-30, 40));
    CPPUNIT_ASSERT_THROW(stations2.shiftLongitudes(), runtime_error);

    Stations stations3;
    stations3.push_back(Station(100, 40));
    stations3.push_back(Station(181, 40));
    stations3.push_back(Station(350, 40));
    stations3.shiftLongitudes();

    CPPUNIT_ASSERT(stations3.size() == 3);
    CPPUNIT_ASSERT(stations3.getStation(0).getX() == 100);
    CPPUNIT_ASSERT(stations3.getStation(1).getX() == -179);
    CPPUNIT_ASSERT(stations3.getStation(2).getX() == -10);
    cout << stations3 << endl;
}

void
testStations::testUnique_() {

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

void testStations::testSubset_() {

    /*
     * Test the index subset functionality.
     */

    // Unique values
    Station s0;
    Station s1(5, 3);
    Station s2(10, 20, "station3");
    Station s3(10, 20, "station2");
    
    Stations stations_all, stations_subset;
    assign::push_back(stations_all.left)(0, s0)(1, s1)(2, s2)(3, s3);
    assign::push_back(stations_subset.left)(0, s0)(1, s3);

    vector<size_t> indices;
    stations_all.getIndices(stations_subset, indices);
    
    CPPUNIT_ASSERT(indices.size() == stations_subset.size());
    CPPUNIT_ASSERT(indices[0] == 0);
    CPPUNIT_ASSERT(indices[1] == 3);
}
