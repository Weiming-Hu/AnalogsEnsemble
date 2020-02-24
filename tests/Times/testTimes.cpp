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
#include "Functions.h"

using namespace std;
using namespace boost::bimaps;
using namespace boost;

CPPUNIT_TEST_SUITE_REGISTRATION(testTimes);

testTimes::testTimes() {
}

testTimes::~testTimes() {
}

void
testTimes::testGetTimeIndex_() {

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

void
testTimes::testSlice_() {

    /**
     * Test the slicing function
     */

    Times times;
    assign::push_back(times.left)
            (0, Time(100))(1, Time(200))(2, Time(300))
            (3, Time(400))(4, Time(500))(5, Time(150))
            (6, Time(12))(7, Time(18))(8, Time(900))
            (9, Time(98))(10, Time(101))(11, Time(1000))
            (12, Time(98))(13, Time(100))(14, Time(500));

    // Confirm that times will not be ordered
    CPPUNIT_ASSERT(times.getTime(0).timestamp == 100);
    CPPUNIT_ASSERT(times.getTime(2).timestamp == 300);
    CPPUNIT_ASSERT(times.getTime(4).timestamp == 500);
    CPPUNIT_ASSERT(times.getTime(6).timestamp == 12);
    CPPUNIT_ASSERT(times.getTime(8).timestamp == 900);
    CPPUNIT_ASSERT(times.getTime(10).timestamp == 101);

    // Confirm that only unique times are kept
    CPPUNIT_ASSERT(times.size() == 12);

    /*************************************************************************
     *                            Test 1: Empty slicing                      *
     *************************************************************************/
    size_t time_start = 51, time_end = 54;
    Times sliced_times;
    times(time_start, time_end, sliced_times);

    CPPUNIT_ASSERT(sliced_times.size() == 0);


    /*************************************************************************
     *                           Test 2: size_t slicing                      *
     *************************************************************************/
    time_start = 50;
    time_end = 100;
    times(time_start, time_end, sliced_times);

    CPPUNIT_ASSERT(sliced_times.size() == 2);
    CPPUNIT_ASSERT(sliced_times.getTime(0).timestamp == 100);
    CPPUNIT_ASSERT(sliced_times.getTime(1).timestamp == 98);


    /*************************************************************************
     *            Test 3: String slicing with standard format                *
     *************************************************************************/
    string time_start_str = "1970-01-01 00:00:50";
    string time_end_str = "1970-01-01 00:01:41";
    bool iso_string = false;
    times(time_start_str, time_end_str, sliced_times, iso_string);

    CPPUNIT_ASSERT(sliced_times.size() == 3);
    CPPUNIT_ASSERT(sliced_times.getTime(0).timestamp == 100);
    CPPUNIT_ASSERT(sliced_times.getTime(1).timestamp == 98);
    CPPUNIT_ASSERT(sliced_times.getTime(2).timestamp == 101);

    // Test 1: string slicing with standard format

    /*************************************************************************
     *                 Test 4: String slicing with ISO format                *
     *************************************************************************/
    time_start_str = "19700101T000050";
    time_end_str = "19700101T000141";
    iso_string = true;
    times(time_start_str, time_end_str, sliced_times, iso_string);

    CPPUNIT_ASSERT(sliced_times.size() == 3);
    CPPUNIT_ASSERT(sliced_times.getTime(0).timestamp == 100);
    CPPUNIT_ASSERT(sliced_times.getTime(1).timestamp == 98);
    CPPUNIT_ASSERT(sliced_times.getTime(2).timestamp == 101);
}

void
testTimes::testSubset_() {

    /*
     * Test the index subset functionality.
     */

    Times all, subset;
    assign::push_back(all.left)
            (0, Time(100))(1, Time(200))(2, Time(300))
            (3, Time(400))(4, Time(500))(5, Time(150))
            (6, Time(12))(7, Time(18))(8, Time(900))
            (9, Time(98))(10, Time(101))(11, Time(1000))
            (12, Time(98))(13, Time(100))(14, Time(500));

    assign::push_back(subset.left)
            (0, Time(400))(1, Time(500))(2, Time(98))(3, Time(500));

    vector<size_t> indices;
    all.getIndices(subset, indices);
    
    cout << "indices: " << Functions::format(indices) << endl;

    CPPUNIT_ASSERT(indices.size() == subset.size());
    CPPUNIT_ASSERT(indices[0] == 3);
    CPPUNIT_ASSERT(indices[1] == 4);
    CPPUNIT_ASSERT(indices[2] == 9);
}
