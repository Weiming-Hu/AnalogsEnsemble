/*
 * File:   testStations.h
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on May 30, 2018, 2:40:21 PM
 */

#ifndef TESTSTATIONS_H
#define TESTSTATIONS_H

#include <cppunit/extensions/HelperMacros.h>
#include "Stations.h"

class testStations : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(testStations);

    CPPUNIT_TEST(testUnique);
    CPPUNIT_TEST(testMultiIndex);
    CPPUNIT_TEST(testGetIndex);
    CPPUNIT_TEST(testGetStationsInSquare);
    CPPUNIT_TEST(testAddStation);

    CPPUNIT_TEST_SUITE_END();

public:
    testStations();
    virtual ~testStations();
    void setUp();
    void tearDown();

private:
    void testUnique();
    void testMultiIndex();
    void testGetIndex();
    void testGetStationsInSquare();
    void testAddStation();
};

#endif /* TESTSTATIONS_H */

