/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   testStandardDeviation.h
 * Author: wuh20
 *
 * Created on Aug 3, 2018, 3:56:05 PM
 */

#ifndef TESTSTANDARDDEVIATION_H
#define TESTSTANDARDDEVIATION_H

#include "../StandardDeviation.h"

#include <cppunit/extensions/HelperMacros.h>

class testStandardDeviation : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(testStandardDeviation);

    CPPUNIT_TEST(testPrint);
    
    CPPUNIT_TEST_SUITE_END();

public:
    testStandardDeviation();
    virtual ~testStandardDeviation();

private:
    void testPrint();
};

#endif /* TESTSTANDARDDEVIATION_H */

