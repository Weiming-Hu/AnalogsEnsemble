/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   testStations.h
 * Author: wuh20
 *
 * Created on May 30, 2018, 2:40:21 PM
 */

#ifndef TESTSTATIONS_H
#define TESTSTATIONS_H

#include <cppunit/extensions/HelperMacros.h>

#include "../Stations.h"

class testStations : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(testStations);

    CPPUNIT_TEST(testUnique);

    CPPUNIT_TEST_SUITE_END();

public:
    testStations();
    virtual ~testStations();
    void setUp();
    void tearDown();

private:

    void testUnique();
};

#endif /* TESTSTATIONS_H */

