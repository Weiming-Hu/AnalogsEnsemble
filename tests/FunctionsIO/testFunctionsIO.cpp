/*
 * File:   testFunctionsIO.cpp
 * Author: wuh20
 *
 * Created on Feb 10, 2020, 12:19:11 PM
 */

#include <boost/bimap/bimap.hpp>

#include "FunctionsIO.h"
#include "ForecastsPointer.h"
#include "ObservationsPointer.h"
#include "testFunctionsIO.h"
#include "boost/assign/list_of.hpp"
#include "boost/assign/list_inserter.hpp"

CPPUNIT_TEST_SUITE_REGISTRATION(testFunctionsIO);

using namespace std;
using namespace boost;
using namespace boost::bimaps;

testFunctionsIO::testFunctionsIO() {
}

testFunctionsIO::~testFunctionsIO() {
}

void
testFunctionsIO::testParseFilename() {

    /**
     * This function tests given a file name, can the function correctly
     * extract time and lead time information
     */
    string file;
    bool delimited;
    Time time, flt;
    double unit_in_seconds = 60 * 60;
    boost::gregorian::date start_day = boost::gregorian::from_string("1970/01/01");

    regex regex_day(".*nam_218_(.+?)_\\d{4}_\\d{3}\\.grb2$");
    regex regex_flt(".*nam_218_.+?_\\d{4}_(\\d{3})\\.grb2$");
    regex regex_cycle(".*nam_218_.+?_(\\d{2})\\d{2}_\\d{3}\\.grb2$");

    // Case 1: Filename without separators and cycle time matching
    file = "Desktop/nam_218_19700102_1100_002.grb2";
    delimited = false;

    FunctionsIO::parseFilename(time, flt, file, start_day,
            regex_day, regex_flt, unit_in_seconds, delimited);

    // check results
    CPPUNIT_ASSERT(time.timestamp == 24 * 3600);
    CPPUNIT_ASSERT(flt.timestamp == 2 * 3600);

    // Case 2: Filename without separators but with cycle time matching
    FunctionsIO::parseFilename(time, flt, file, start_day,
            regex_day, regex_flt, regex_cycle, unit_in_seconds, delimited);

    // check results
    CPPUNIT_ASSERT(time.timestamp == 24 * 3600 + 11 * 3600);
    CPPUNIT_ASSERT(flt.timestamp == 2 * 3600);
    
    // Case 3: Filename with separators and cycle time matching
    file = "Desktop/nam_218_1970-01-02_1100_002.grb2";
    delimited = true;
    
    FunctionsIO::parseFilename(time, flt, file, start_day,
            regex_day, regex_flt, regex_cycle, unit_in_seconds, delimited);

    // check results
    CPPUNIT_ASSERT(time.timestamp == 24 * 3600 + 11 * 3600);
    CPPUNIT_ASSERT(flt.timestamp == 2 * 3600);
}
