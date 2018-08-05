/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   testStandardDeviation.cpp
 * Author: wuh20
 *
 * Created on Aug 3, 2018, 3:56:05 PM
 */

#include "testStandardDeviation.h"

#include <numeric>

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION(testStandardDeviation);

testStandardDeviation::testStandardDeviation() {
}

testStandardDeviation::~testStandardDeviation() {
}

void testStandardDeviation::testPrint() {
    
    StandardDeviation arr(3, 4, 5);
    iota(arr.data(), arr.data() + arr.num_elements(), 0);

    cout << arr;
}