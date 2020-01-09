/*
 * File:   testForecastsArray.h
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on Jul 1, 2018, 10:47:47 AM
 */

#ifndef TESTFORECASTSARRAY_H
#define TESTFORECASTSARRAY_H

#include <cppunit/extensions/HelperMacros.h>

class testForecastsArray : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(testForecastsArray);
    
    CPPUNIT_TEST(testForecastSetVectorValues);
    CPPUNIT_TEST(testForecastParameters);
    
    CPPUNIT_TEST_SUITE_END();

public:
    testForecastsArray();
    virtual ~testForecastsArray();
    
    void testForecastSetVectorValues();
    void testForecastParameters();

private:
    
};

#endif /* TESTFORECASTSARRAY_H */
