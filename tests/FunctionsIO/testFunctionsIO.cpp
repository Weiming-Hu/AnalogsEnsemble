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
testFunctionsIO::testCollapseLeadTimes() {
    
    /**
     * This function tests the conversion from forecasts to observations
     */
    // Parameters
    Parameters parameters;
    Parameter p1, p2("temperature"), p3("Direction"), p4("wind direction", true);
    assign::push_back(parameters.left)(0, p1)(1, p2)(2, p3)(3, p4);
    
    // Stations
    Stations stations;
    Station s0, s1(5, 3), s2(10, 20, "station3"), s3(10, 20, "station2");
    assign::push_back(stations.left)(0, s0)(1, s1)(2, s2)(3, s3);
    
    // Times and FLTs
    Times times, flts;
    assign::push_back(times.left)(0, 0)(0, 6)(0, 12);
    assign::push_back(flts.left)(0, 0)(0, 3)(0, 6)(0, 9)(0, 12);
    
    // Forecasts
    ForecastsPointer forecasts(parameters, stations, times, flts);
    
    double* p_data = forecasts.getValuesPtr();
    for (size_t value_i = 0; value_i < forecasts.num_elements(); ++value_i) {
        p_data[value_i] = value_i;
    }
    
    cout << "Forecasts initialized" << endl;
    
    // Call the method
    ObservationsPointer observations;
    FunctionsIO::collapseLeadTimes(observations, forecasts);
    
    cout << "Forecasts collapsed to observations" << endl;
    
    // Check results
    double obs, fcst;
    CPPUNIT_ASSERT(observations.num_elements() == 4 * 4 * 9);
    
    for (size_t parameter_i = 0; parameter_i < parameters.size(); ++parameter_i) {
        for (size_t station_i = 0; station_i < stations.size(); ++station_i) {
            for (size_t time_i = 0; time_i < observations.getTimes().size(); ++time_i) {
                obs = observations.getValue(parameter_i, station_i, time_i);
                
                /*
                 * If you fail this test, you need to check whether your algorithm
                 * prefers values that are from an earlier forecast lead time
                 */
                if (time_i == 0) fcst = forecasts.getValue(parameter_i, station_i, 0, 0);
                else if (time_i == 1) fcst = forecasts.getValue(parameter_i, station_i, 0, 1);
                else if (time_i == 2) fcst = forecasts.getValue(parameter_i, station_i, 1, 0);
                else if (time_i == 3) fcst = forecasts.getValue(parameter_i, station_i, 1, 1);
                else if (time_i == 4) fcst = forecasts.getValue(parameter_i, station_i, 2, 0);
                else if (time_i == 5) fcst = forecasts.getValue(parameter_i, station_i, 2, 1);
                else if (time_i == 6) fcst = forecasts.getValue(parameter_i, station_i, 2, 2);
                else if (time_i == 7) fcst = forecasts.getValue(parameter_i, station_i, 2, 3);
                else if (time_i == 8) fcst = forecasts.getValue(parameter_i, station_i, 2, 4);
                else throw runtime_error("Wrong time index");
                
                CPPUNIT_ASSERT(obs == fcst);

            }
        }
    }
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
