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

#include "testSimilarityMatrices.h"

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

    auto vec_mat = mats.getTargets().getValues();
    for (size_t i = 0; i < mats.getTargets().getDataLength(); i++) {
        CPPUNIT_ASSERT(vec_mat[i] == values[i]);
    }
    
    cout << mats;
}

void testSimilarityMatrices::testMatrixPrint() {
    
    /**
     * Test the print functionality of SimilarityMatrix
     */

    SimilarityMatrix mat(10);
    mat.clear();

    for (size_t i = 0; i < mat.size1(); ++ i)
        for (size_t j = 0; j < mat.size2() - 1; ++ j)
            mat(i, j) = 3 * i + j;

    cout << mat;
}