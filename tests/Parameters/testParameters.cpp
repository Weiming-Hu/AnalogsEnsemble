/*
 * File:   testParameters.cpp
 * Author: wuh20
 *
 * Created on Jan 10, 2020, 4:00:12 PM
 */

#include "testParameters.h"
#include "Parameters.h"

#include "boost/assign/list_of.hpp"
#include "boost/assign/list_inserter.hpp"

using namespace std;
using namespace boost::bimaps;
using namespace boost;

CPPUNIT_TEST_SUITE_REGISTRATION(testParameters);

testParameters::testParameters() {
}

testParameters::~testParameters() {
}

void testParameters::testCreation() {
    Parameter p1, p2("temperature"), p3("Direction", 0.6),
            p4("wind direction", 0.05, true);
    Parameters parameters;
    assign::push_back(parameters.left)(0, p1)(1, p2)(2, p3)(3, p4);
    cout << parameters;

    // Test circular
    CPPUNIT_ASSERT(!parameters.left[0].second.getCircular());
    CPPUNIT_ASSERT(!parameters.left[1].second.getCircular());
    CPPUNIT_ASSERT(!parameters.left[2].second.getCircular());
    CPPUNIT_ASSERT(parameters.left[3].second.getCircular());

    // Test weight
    CPPUNIT_ASSERT(parameters.left[0].second.getWeight() == 1);
    CPPUNIT_ASSERT(parameters.left[1].second.getWeight() == 1);
    CPPUNIT_ASSERT(parameters.left[2].second.getWeight() == 0.6);
    CPPUNIT_ASSERT(parameters.left[3].second.getWeight() == 0.05);
}

void testParameters::testUnique() {
    Parameter p1, p2, p3("Direction", 0.6), p4("Direction", 0.1, true), p5;
    p5.setCircular(true);
    Parameters parameters;
    assign::push_back(parameters.left)(0, p1)(1, p2)(2, p3)(3, p4)(4, p5);
    cout << parameters;
    
    CPPUNIT_ASSERT(parameters.size() == 2);
    CPPUNIT_ASSERT(parameters.left[0].second == p1);
    CPPUNIT_ASSERT(parameters.left[1].second == p3);
}
