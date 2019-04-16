/*
 * File:   testAnEn.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on Aug 4, 2018, 4:09:20 PM
 */

#ifndef TESTANEN_H
#define TESTANEN_H

#include <cppunit/extensions/HelperMacros.h>

#include "AnEn.h"

class testAnEn : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(testAnEn);

    CPPUNIT_TEST(testAutomaticDelteOverlappingTimes);
    CPPUNIT_TEST(testGenerateOperationalSearchTimes);
    CPPUNIT_TEST(testComputeSearchStations);
    CPPUNIT_TEST(testComputeSimilarity);
    CPPUNIT_TEST(testOperationalSearch);
    CPPUNIT_TEST(testSelectAnalogs);
    CPPUNIT_TEST(testLeaveOneOut);
    CPPUNIT_TEST(testOpenMP);

    CPPUNIT_TEST_SUITE_END();

public:
    testAnEn();
    virtual ~testAnEn();

private:
    void testAutomaticDelteOverlappingTimes();
    void testGenerateOperationalSearchTimes();
    void testComputeSearchStations();
    void testComputeSimilarity();
    void testOperationalSearch();
    void testSelectAnalogs();
    void testLeaveOneOut();
    void testOpenMP();
};

#endif /* TESTANEN_H */

