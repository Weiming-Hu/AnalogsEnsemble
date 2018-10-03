/*
 * File:   testStations.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on May 30, 2018, 2:40:21 PM
 */

#include <cppunit/TestAssert.h>

#include "testStations.h"

#include <iostream>
#include <algorithm>
#include <boost/lambda/lambda.hpp>

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
     * This is only a test of concept, not using any provided functions.
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


    using namespace boost::lambda;

    // Create an example forecast object
    Station s1("1", 1, 1), s2("2", 6, 1), s3("3", 1, 3),
            s4("4", 4, 3), s5("5", 8, 3), s6("6", 3, 4),
            s7("7", 6, 4), s8("8", 2, 5), s9("9", 3, 5),
            s10("10", 4, 5), s11("11", 2, 6), s12("12", 3, 7),
            s13("13", 7, 7), s14("14", 5, 8);
    Stations stations;
    stations.insert(stations.end(),{s1, s2, s3, s4, s5, s6,
        s7, s8, s9, s10, s11, s12, s13, s14});

    // Arrange stations in order of coordinates
    const multiIndexStations::index<by_x>::type &
            stations_by_x = stations.get<by_x>();

    double half_edge = 1.5;

    double main_station_x = s9.getX();
    double main_station_y = s9.getY();

    auto search_stations_x_lower =
            stations_by_x.lower_bound(main_station_x - half_edge);
    auto search_stations_x_upper =
            stations_by_x.upper_bound(main_station_x + half_edge);

    auto stations_range_1 = stations_by_x.range(
            main_station_x - half_edge <= boost::lambda::_1,
            boost::lambda::_1 <= main_station_x + half_edge);

    vector<Station> answers{s8, s11, s6, s9, s12, s4, s10};

    auto it1 = search_stations_x_lower;
    auto it2 = stations_range_1.first;
    auto it3 = answers.begin();
    for (; (it1 != search_stations_x_upper) &&
            (it2 != stations_range_1.second) &&
            (it3 != answers.end());
            it1++, it2++, it3++) {
        CPPUNIT_ASSERT(*it1 == *it2);
        CPPUNIT_ASSERT(*it2 == *it3);
    }

    Stations stations_subset;
    stations_subset.insert(stations_subset.end(),
            stations_range_1.first, stations_range_1.second);

    auto & stations_subset_by_y = stations_subset.get<by_y>();

    auto stations_range_2 = stations_subset_by_y.range(
            main_station_y - half_edge <= boost::lambda::_1,
            boost::lambda::_1 <= main_station_y + half_edge);

    answers = {s6, s8, s9, s10, s11};
    auto it4 = stations_range_2.first;
    for (it3 = answers.begin();
            (it4 != stations_range_2.second) &&
            (it3 != answers.end()); it3++, it4++) {
        CPPUNIT_ASSERT(*it3 == *it4);
    }
}

void testStations::testAddStation() {

    /**
     * Test adding Station to Stations.
     */

    Station s1("test1", 20, 40), s2("test 2");
    Stations stations;
    stations.push_back(s1);
    stations.push_back(s2);

    auto & stations_by_insert = stations.get<by_insert>();
    CPPUNIT_ASSERT(stations_by_insert[0] == s1);
    CPPUNIT_ASSERT(stations_by_insert[1] == s2);
}

void testStations::testGetStationsIdBySquare() {

    /**
     * Tests the multi-index container to get nearby stations in a square.
     * This is the correctness test for function getStationsIdBySquare();
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
    Station s1("1", 1, 1), s2("2", 6, 1), s3("3", 1, 3),
            s4("4", 4, 3), s5("5", 8, 3), s6("6", 3, 4),
            s7("7", 6, 4), s8("8", 2, 5), s9("9", 3, 5),
            s10("10", 4, 5), s11("11", 2, 6), s12("12", 3, 7),
            s13("13", 7, 7), s14("14", 5, 8);
    Stations stations;
    stations.insert(stations.end(),{s1, s2, s3, s4, s5, s6,
        s7, s8, s9, s10, s11, s12, s13, s14});

    // Test 1
    // Main station is s9; half size window is 0.5
    //
    double half_size = 0.5;
    vector<size_t> results = stations.getStationsIdBySquare(
            s9.getX(), s9.getY(), half_size);
    CPPUNIT_ASSERT(results.size() == 1);

    // Test 2
    // Main station is s9; half size window is 1.5
    //
    half_size = 1.5;
    results = stations.getStationsIdBySquare(
            s9.getX(), s9.getY(), half_size);
    CPPUNIT_ASSERT(results.size() == 5);
    CPPUNIT_ASSERT(find(results.begin(), results.end(),
            s11.getID()) != results.end());
    CPPUNIT_ASSERT(find(results.begin(), results.end(),
            s8.getID()) != results.end());
    CPPUNIT_ASSERT(find(results.begin(), results.end(),
            s10.getID()) != results.end());
    CPPUNIT_ASSERT(find(results.begin(), results.end(),
            s6.getID()) != results.end());
    CPPUNIT_ASSERT(find(results.begin(), results.end(),
            s9.getID()) != results.end());

    // Test 3
    // Main station is s1; half size window is 2
    //
    half_size = 2;
    results = stations.getStationsIdBySquare(
            s1.getX(), s1.getY(), half_size);
    CPPUNIT_ASSERT(results.size() == 2);
    CPPUNIT_ASSERT(find(results.begin(), results.end(),
            s1.getID()) != results.end());
    CPPUNIT_ASSERT(find(results.begin(), results.end(),
            s3.getID()) != results.end());

}

void testStations::testGetStationsIdByDistance() {

    /**
     * Tests the multi-index container to get nearby stations with distance.
     * This is the correctness test for function getStationsIdByDistance();
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
    Station s1("1", 1, 1), s2("2", 6, 1), s3("3", 1, 3),
            s4("4", 4, 3), s5("5", 8, 3), s6("6", 3, 4),
            s7("7", 6, 4), s8("8", 2, 5), s9("9", 3, 5),
            s10("10", 4, 5), s11("11", 2, 6), s12("12", 3, 7),
            s13("13", 7, 7), s14("14", 5, 8);
    Stations stations;
    stations.insert(stations.end(),{s1, s2, s3, s4, s5, s6,
        s7, s8, s9, s10, s11, s12, s13, s14});

    // Test 1
    // Main station is s2; distance is 2
    //
    double distance = 2;
    vector<size_t> results = stations.getStationsIdByDistance(
            s2.getX(), s2.getY(), distance);
    CPPUNIT_ASSERT(results.size() == 1);

    // Test 2
    // Main station is s9; half size window is 1.1
    //
    distance = 1.1;
    results = stations.getStationsIdByDistance(
            s9.getX(), s9.getY(), distance);
    CPPUNIT_ASSERT(results.size() == 4);
}

void testStations::testGetNearestStationsId() {

    /**
     * Tests the multi-index container to get K-nearest stations.
     * This is the correctness test for function getNearestStationsId();
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
    Station s1("1", 1, 1), s2("2", 6, 1), s3("3", 1, 3),
            s4("4", 4, 3), s5("5", 8, 3), s6("6", 3, 4),
            s7("7", 6, 4), s8("8", 2, 5), s9("9", 3, 5),
            s10("10", 4, 5), s11("11", 2, 6), s12("12", 3, 7),
            s13("13", 7, 7), s14("14", 5, 8);
    Stations stations;
    stations.insert(stations.end(),{s1, s2, s3, s4, s5, s6,
        s7, s8, s9, s10, s11, s12, s13, s14});

    // Test 1
    // Main station is s4; number of nearest stations is 1
    //
    size_t num_stations = 1;
    vector<size_t> results = stations.getNearestStationsId(
            s4.getX(), s4.getY(), num_stations);
    CPPUNIT_ASSERT(results.size() == num_stations);
    CPPUNIT_ASSERT(results[0] == s4.getID());

    // Test 2
    // Main station is s13; number of nearest stations is 2
    //
    num_stations = 3;
    results = stations.getNearestStationsId(
            s13.getX(), s13.getY(), num_stations);
    CPPUNIT_ASSERT(results.size() == num_stations);
    CPPUNIT_ASSERT(find(results.begin(), results.end(),
            s7.getID()) != results.end());
    CPPUNIT_ASSERT(find(results.begin(), results.end(),
            s14.getID()) != results.end());
    CPPUNIT_ASSERT(find(results.begin(), results.end(),
            s13.getID()) != results.end());

    // Test 3
    // Main station is s4; number of nearest stations is 4;
    // threshold is 2.1
    //
    num_stations = 4;
    double threshold = 2.1;
    results = stations.getNearestStationsId(
            s4.getX(), s4.getY(), num_stations, threshold);
    CPPUNIT_ASSERT(results.size() == 3);
    CPPUNIT_ASSERT(find(results.begin(), results.end(),
            s6.getID()) != results.end());
    CPPUNIT_ASSERT(find(results.begin(), results.end(),
            s4.getID()) != results.end());
    CPPUNIT_ASSERT(find(results.begin(), results.end(),
            s10.getID()) != results.end());
    CPPUNIT_ASSERT(find(results.begin(), results.end(),
            s9.getID()) == results.end());
}

void testStations::testGetStationsIndex() {

    /**
     * Test the function getStationsIndex()
     */
    Station s1("1", 1, 1), s2("2", 6, 1), s3("3", 1, 3),
            s4("4", 4, 3), s5("5", 8, 3), s6("6", 3, 4),
            s7("7", 6, 4), s8("8", 2, 5), s9("9", 3, 5),
            s10("10", 4, 5), s11("11", 2, 6), s12("12", 3, 7),
            s13("13", 7, 7), s14("14", 5, 8);
    vector<Station> vec_stations{s1, s2, s3, s4, s5, s6, s7, s8, s9, s10,
        s11, s12, s13, s14};

    vector<size_t> stations_ID;
    for (const auto & s : vec_stations) {
        stations_ID.push_back(s.getID());
    }

    Stations stations;
    stations.insert(stations.end(), vec_stations.begin(), vec_stations.end());

    vector<size_t> stations_index = stations.getStationsIndex(stations_ID);
    
    size_t compare = 0;
    for (const auto & i : stations_index) {
        CPPUNIT_ASSERT(i == compare);
        compare++;
    }
}
