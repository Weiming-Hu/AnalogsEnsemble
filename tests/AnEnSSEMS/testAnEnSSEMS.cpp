/*
 * File:   testAnEnSSEMS.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on Mar. 10, 2021, 17:13
 */
#include <numeric>

#include "AnEnSSEMS.h"
#include "Forecasts.h"
#include "Observations.h"
#include "testAnEnSSEMS.h"
#include "ForecastsPointer.h"
#include "ObservationsPointer.h"

#include "boost/bimap/bimap.hpp"
#include "boost/assign/list_of.hpp"
#include "boost/assign/list_inserter.hpp"
#include "boost/numeric/ublas/matrix_proxy.hpp"

using namespace std;
using namespace boost;
using namespace boost::bimaps;

CPPUNIT_TEST_SUITE_REGISTRATION(testAnEnSSEMS);

testAnEnSSEMS::testAnEnSSEMS() {
}

testAnEnSSEMS::~testAnEnSSEMS() {
}

void
testAnEnSSEMS::testMatchStations_() {

    /*
     * Test finding the closest stations
     */
    Stations obs_stations;
    {
        Station s0(1, 0), s1(0, 0.2), s2(0.6, 0.9);
        assign::push_back(obs_stations.left)(0, s0)(1, s1)(2, s2);
    }

    Stations fcst_stations;
    {
        Station s0(0, 0), s1(1, 0), s2(0.5, NAN), s3(0, 1), s4(1, 1);
        assign::push_back(fcst_stations.left)(0, s0)(1, s1)(2, s2)(3, s3)(4, s4);
    }

    matchStations_(obs_stations, fcst_stations);
    vector<size_t> results = {1, 0, 4};

    CPPUNIT_ASSERT(equal(results.begin(), results.end(), match_obs_stations_with_.begin()));
}
