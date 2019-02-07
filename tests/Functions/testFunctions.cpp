/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   testFunctions.cpp
 * Author: wuh20
 *
 * Created on Feb 7, 2019, 2:28:24 PM
 */

#include "testFunctions.h"


CPPUNIT_TEST_SUITE_REGISTRATION(testFunctions);

testFunctions::testFunctions() {
}

testFunctions::~testFunctions() {
}

void testFunctions::setUp() {
}

void testFunctions::tearDown() {
}

void testFunctions::testMethod() {
    CPPUNIT_ASSERT(true);
}

void testFunctions::testFailedMethod() {
    CPPUNIT_ASSERT(false);
}

