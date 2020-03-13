/*
 * File:   testStations.h
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on May 30, 2018, 2:40:21 PM
 */

#ifndef TESTSTATIONS_H
#define TESTSTATIONS_H

#include <cppunit/extensions/HelperMacros.h>

class testStations : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(testStations);

    CPPUNIT_TEST(testShift_);
    CPPUNIT_TEST(testUnique_);
    CPPUNIT_TEST(testSubset_);

    CPPUNIT_TEST_SUITE_END();

public:
    testStations();
    virtual ~testStations();

private:
    void testShift_();
    void testUnique_();
    void testSubset_();
};

#endif /* TESTSTATIONS_H */

