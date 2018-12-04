/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   testForecastsArray.h
 * Author: wuh20
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
