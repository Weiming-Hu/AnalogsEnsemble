/*
 * File:   testTimes.h
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on Aug 5, 2018, 12:57:17 PM
 */

#ifndef TESTTIMES_H
#define TESTTIMES_H

#include <cppunit/extensions/HelperMacros.h>

class testTimes : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(testTimes);
    
    CPPUNIT_TEST(testGetTimeIndex_);
    CPPUNIT_TEST(testSlice_);

    CPPUNIT_TEST_SUITE_END();

public:
    testTimes();
    virtual ~testTimes();

private:
    void testGetTimeIndex_();
    void testSlice_();
};

#endif /* TESTTIMES_H */

