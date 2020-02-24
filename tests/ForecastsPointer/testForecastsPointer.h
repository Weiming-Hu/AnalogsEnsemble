/*
 * File:   testForecastsPointer.h
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on Jul 1, 2018, 10:47:47 AM
 */

#ifndef TESTFORECASTSPOINTER_H
#define TESTFORECASTSPOINTER_H

#include <cppunit/extensions/HelperMacros.h>

class testForecastsPointer : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(testForecastsPointer);
    
    CPPUNIT_TEST(testForecastSetVectorValues_);
    CPPUNIT_TEST(testSubset_);
    
    CPPUNIT_TEST_SUITE_END();

public:
    testForecastsPointer();
    virtual ~testForecastsPointer();
    
    void testForecastSetVectorValues_();
    void testSubset_();

private:
    
};

#endif /* TESTFORECASTSPOINTER_H */
