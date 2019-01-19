/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   testSimilarityMatrix.cpp
 * Author: wuh20
 *
 * Created on Aug 3, 2018, 12:53:04 PM
 */

#include <cppunit/TestAssert.h>

#include "testSimilarityMatrices.h"

#include <algorithm>

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION(testSimilarityMatrices);

testSimilarityMatrices::testSimilarityMatrices() {
}

testSimilarityMatrices::~testSimilarityMatrices() {
}

void testSimilarityMatrices::testMatricesConstructor() {

    /**
     * Test SimilarityMatrices constructor and print functionality
     */

    anenSta::Station s1("Beijing", 30, 30), s2;
    anenSta::Stations stations;
    stations.push_back(s1);

    anenPar::Parameter p1, p2("temperature", 0.6),
        p3("humidity", 0.3), p4("wind direction", 0.05, true);
    p1.setWeight(0.05);
    anenPar::Parameters parameters;
    parameters.insert(parameters.end(),{p1, p2, p3, p4});

    anenTime::Times times;
    times.push_back(12.34);
    times.push_back(1.34);
    times.push_back(12.3);

    anenTime::FLTs flts;
    flts.push_back(100);
    flts.push_back(200);

    vector<double> values(parameters.size() * stations.size() *
            times.size() * flts.size());
    iota(values.begin(), values.end(), 0);

    Forecasts_array forecasts(parameters, stations, times, flts, values);

    SimilarityMatrices mats(forecasts);
    CPPUNIT_ASSERT(mats.num_elements() == 
            forecasts.getStationsSize() * forecasts.getTimesSize() *
            forecasts.getFLTsSize() * mats.getMaxEntries() * mats.getNumCols());
}

void testSimilarityMatrices::testMatricesSort() {

    /**
     * Test the function sortRows().
     */
    SimilarityMatrices sims(1, 3, 2, 6);

    vector<double> initials = {
        0, 1, 5, 0, 2, 8, 0, 3, 9, 4, 2, 1, 4, 3, 5, NAN, NAN, NAN,
        9, 1, 5, 1, 2, 3, NAN, NAN, NAN, 2, 3, 1, 4, 6, 3, 7, 8, 10,
        3, 2, 5, 4, 6, 5, 4, 3, 2, 7, 6, NAN, NAN, NAN, 6, 9, 8, 1,
        1, 2, 3, NAN, NAN, NAN, 2, 3, 1, 2, 4, 3, 5, 9, 1, 6, 8, 4,
        NAN, NAN, NAN, 9, 8, 7, 8, 7, 6, 7, 8, 9, 5, 7, 6, 6, 8, 7,
        3, 6, 5, 7, 6, 4, 8, 7, 3, 9, 9, 9, 7, 9, 1, NAN, NAN, NAN
    };

    if (initials.size() != sims.num_elements())
        throw length_error("Number of elements don't match!");
    sims.assign(initials.begin(), initials.end());

    sims.sortRows(false, 0, SimilarityMatrices::COL_TAG::TIME);
    vector<double> results1 = {
        4, 2, 1, 0, 1, 5, 4, 3, 5, 0, 2, 8, 0, 3, 9, NAN, NAN, NAN,
        2, 3, 1, 1, 2, 3, 4, 6, 3, 9, 1, 5, 7, 8, 10, NAN, NAN, NAN,
        9, 8, 1, 4, 3, 2, 3, 2, 5, 4, 6, 5, NAN, NAN, 6, 7, 6, NAN,
        2, 3, 1, 5, 9, 1, 1, 2, 3, 2, 4, 3, 6, 8, 4, NAN, NAN, NAN,
        8, 7, 6, 5, 7, 6, 9, 8, 7, 6, 8, 7, 7, 8, 9, NAN, NAN, NAN,
        7, 9, 1, 8, 7, 3, 7, 6, 4, 3, 6, 5, 9, 9, 9, NAN, NAN, NAN
    };
    for (size_t i = 0; i < sims.num_elements(); i++) {
        if (std::isnan(sims.data()[i]) || std::isnan(results1[i])) {
            CPPUNIT_ASSERT(std::isnan(sims.data()[i]));
            CPPUNIT_ASSERT(std::isnan(results1[i]));
        } else {
            CPPUNIT_ASSERT(sims.data()[i] == results1[i]);
        }
    }

    sims.sortRows(false, 0, SimilarityMatrices::COL_TAG::STATION);
    vector<double> results2 = {
        0, 1, 5, 4, 2, 1, 0, 2, 8, 4, 3, 5, 0, 3, 9, NAN, NAN, NAN,
        9, 1, 5, 1, 2, 3, 2, 3, 1, 4, 6, 3, 7, 8, 10, NAN, NAN, NAN,
        3, 2, 5, 4, 3, 2, 4, 6, 5, 7, 6, NAN, 9, 8, 1, NAN, NAN,
        6, 1, 2, 3, 2, 3, 1, 2, 4, 3, 6, 8, 4, 5, 9, 1, NAN, NAN, NAN,
        8, 7, 6, 5, 7, 6, 9, 8, 7, 6, 8, 7, 7, 8, 9, NAN, NAN, NAN,
        7, 6, 4, 3, 6, 5, 8, 7, 3, 7, 9, 1, 9, 9, 9, NAN, NAN, NAN
    };
    for (size_t i = 0; i < sims.num_elements(); i++) {
        if (std::isnan(sims.data()[i]) || std::isnan(results2[i])) {
            CPPUNIT_ASSERT(std::isnan(sims.data()[i]));
            CPPUNIT_ASSERT(std::isnan(results2[i]));
        } else {
            CPPUNIT_ASSERT(sims.data()[i] == results2[i]);
        }
    }

    sims.sortRows(true, 3, SimilarityMatrices::COL_TAG::TIME);
    vector< vector<double> > results3 = {
        {1, 5, 5}, {1, 3, 3}, {1, 2, 5},
        {1, 1, 3}, {6, 6, 7}, {1, 3, 4}
    };

    size_t pos = 0;
    for (size_t i = 0; i < sims.shape()[0]; i++) 
        for (size_t j = 0; j < sims.shape()[1]; j++) 
            for (size_t k = 0; k < sims.shape()[2]; k++, pos++) {
                CPPUNIT_ASSERT( find(results3[pos].begin(), results3[pos].end(),
                            sims[i][j][k][0][SimilarityMatrices::COL_TAG::TIME]) != results3[pos].end());
                CPPUNIT_ASSERT( find(results3[pos].begin(), results3[pos].end(),
                            sims[i][j][k][1][SimilarityMatrices::COL_TAG::TIME]) != results3[pos].end());
                CPPUNIT_ASSERT( find(results3[pos].begin(), results3[pos].end(),
                            sims[i][j][k][2][SimilarityMatrices::COL_TAG::TIME]) != results3[pos].end());
            }
}