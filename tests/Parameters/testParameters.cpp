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

void testParameters::testCreation_() {
    Parameter p1, p2("temperature"), p3("Direction"),
            p4("wind direction", true);
    Parameters parameters;
    assign::push_back(parameters.left)(0, p1)(1, p2)(2, p3)(3, p4);
    cout << parameters;

    // Test circular
    CPPUNIT_ASSERT(!parameters.left[0].second.getCircular());
    CPPUNIT_ASSERT(!parameters.left[1].second.getCircular());
    CPPUNIT_ASSERT(!parameters.left[2].second.getCircular());
    CPPUNIT_ASSERT(parameters.left[3].second.getCircular());
}

void testParameters::testUnique_() {
    Parameter p1, p2, p3("Direction"), p4("Direction", true), p5;
    p5.setCircular(true);
    Parameters parameters;
    assign::push_back(parameters.left)(0, p1)(1, p2)(2, p3)(3, p4)(4, p5);
    cout << parameters;
    
    CPPUNIT_ASSERT(parameters.size() == 2);
    CPPUNIT_ASSERT(parameters.left[0].second == p1);
    CPPUNIT_ASSERT(parameters.left[1].second == p3);
}

void testParameters::testSubset_() {

    /*
     * Test the index subset functionality.
     */

    // Unique values
    Parameter p1, p2("temperature"), p3("Direction"),
            p4("wind direction", true);
    
    Parameters all, subset;
    assign::push_back(all.left)(0, p1)(1, p2)(2, p3)(3, p4);
    assign::push_back(subset.left)(0, p2)(1, p4);

    vector<size_t> indices;
    all.getIndices(subset, indices);
    
    CPPUNIT_ASSERT(indices.size() == subset.size());
    CPPUNIT_ASSERT(indices[0] == 1);
    CPPUNIT_ASSERT(indices[1] == 3);
}