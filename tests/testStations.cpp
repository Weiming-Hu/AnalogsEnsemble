/*
 * File:   testStations.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on May 30, 2018, 2:40:21 PM
 */

#include <cppunit/TestAssert.h>

#include "testStations.h"

#include <iostream>
#include <boost/lambda/lambda.hpp>

using namespace std;
using namespace anenSta;
using namespace boost::lambda;

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

    vector<Station> vec{s0, s1, s2, s3, s4, s5};

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
    vector<Station> vec{s0, s3, s4, s5, s1, s2};

    Stations stations;
    stations.insert(stations.end(), vec.begin(), vec.end());

    CPPUNIT_ASSERT(stations.getStationIndex(s0.getID()) == 0);
    CPPUNIT_ASSERT(stations.getStationIndex(s3.getID()) == 1);
    CPPUNIT_ASSERT(stations.getStationIndex(s5.getID()) == 2);
    CPPUNIT_ASSERT(stations.getStationIndex(s2.getID()) == 3);

    // Test if there is no duplicate
    vec = {s3, s2, s1, s0};
    stations.clear();
    stations.insert(stations.end(), vec.begin(), vec.end());

    CPPUNIT_ASSERT(stations.getStationIndex(s0.getID()) == 3);
    CPPUNIT_ASSERT(stations.getStationIndex(s1.getID()) == 2);
    CPPUNIT_ASSERT(stations.getStationIndex(s2.getID()) == 1);
    CPPUNIT_ASSERT(stations.getStationIndex(s3.getID()) == 0);
}

void testStations::testGetStationsInSquare() {

    /**
     * Tests the multi-index container to get nearby stations in a square.
     * 
     * The spatial distribution of stations looks like below. The numbers
     * shown on the diagram are the ID of stations.
     * 
     *           (Y)  ^
     *              8 |             14
     *              7 |       12          13
     *              6 |    11
     *              5 |     8  9 10
     *              4 |        6        7
     *              3 |  3        4           5
     *              2 |
     *              1 |  1              2
     *                .-------------------------->
     *                0  1  2  3  4  5  6  7  8  (X)
     */

    // Create an example forecast object
    anenSta::Station s1("1", 1, 1), s2("2", 6, 1), s3("3", 1, 3),
            s4("4", 4, 3), s5("5", 8, 3), s6("6", 3, 4),
            s7("7", 6, 4), s8("8", 2, 5), s9("9", 3, 5),
            s10("10", 4, 5), s11("11", 2, 6), s12("12", 3, 7),
            s13("13", 7, 7), s14("14", 5, 8);
    anenSta::Stations stations;
    stations.insert(stations.end(),{s1, s2, s3, s4, s5, s6,
        s7, s8, s9, s10, s11, s12, s13, s14});

    // Arrange stations in order of coordinates
    const multiIndexStations::index<by_x>::type &
            stations_by_x = stations.get<by_x>();

    double half_edge = 1.5;

    size_t main_station_ID = s9.getID();
    string main_station_name = s9.getName();
    double main_station_x = s9.getX();
    double main_station_y = s9.getY();

    auto search_stations_x_lower =
            stations_by_x.lower_bound(main_station_x - half_edge);
    auto search_stations_x_upper =
            stations_by_x.upper_bound(main_station_x + half_edge);

    cout << "Nearby stations of the main station (" << main_station_name
            << ") with the half edge (" << half_edge << ") are: " << endl;
    for (auto it = search_stations_x_lower;
            it != search_stations_x_upper; it++) {
        cout << *it;
    }

    //std::pair<double_set::iterator,double_set::iterator> p= s.range(100.0<=_1,_1<=200); // 100<= x <=200
    auto p = stations_by_x.range(main_station_x - half_edge <= boost::lambda::_1, 
                boost::lambda::_1 <= main_station_x + half_edge);

    cout << "----------" << endl;
    cout << endl << *(p.first) << " AND " << endl << *(prev(p.second)) << endl;

}