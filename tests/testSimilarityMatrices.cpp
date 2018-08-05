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

    for (size_t i = 0; i < mat.nrows(); ++i)
        for (size_t j = 0; j < mat.ncols() - 1; ++j)
            mat[i][j] = 3 * i + j;

    cout << mat;
}

void testSimilarityMatrices::testMatrixSort() {

    /**
     * Test the function sort().
     */
    SimilarityMatrix mat(10);

    double val = 0;
    size_t pos = 0;
    vector<double> input{7, 49, 73, 58, 30, 72, 44, 78, 23, 9, 40, 65,
        92, 42, 87, 3, 27, 29, 40, 12, 3, 69, 9, 57, 60, 33, 99, 78, 16, 35};
    for (size_t i = 0; i < mat.nrows(); ++i)
        for (size_t j = 0; j < mat.ncols(); ++j, val++, pos++) {
            mat[i][j] = input[pos];
        }
    cout << mat;

    cout << "Quick sort: (Please visually check the correctness)" << endl;
    mat.sortRows(true, 5);
    cout << mat;

    cout << "Globle sort" << endl;
    mat.sortRows(false);
    cout << mat;
    
    pos = 0;
    vector<double> result_global{40, 12, 3, 44, 78, 23, 3, 27, 29, 78, 16,
        35, 69, 9, 57, 9, 40, 65, 58, 30, 72, 7, 49, 73, 92, 42, 87, 60, 33, 99};
    for (size_t i = 0; i < mat.nrows(); ++i)
        for (size_t j = 0; j < mat.ncols(); ++j, val++, pos++) {
            CPPUNIT_ASSERT(mat[i][j] = result_global[pos]);
        }
    
}

void testSimilarityMatrices::testMatrixResize() {

    /**
     * Test the Matrix resize function.
     */

    SimilarityMatrix test1;
    CPPUNIT_ASSERT(test1.size() == 0);

    SimilarityMatrix test2(4);
    CPPUNIT_ASSERT(test2.size() == 4);
    for (const auto & vec : test2)
        CPPUNIT_ASSERT(vec.size() == 3);
}