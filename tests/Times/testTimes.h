/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   testTimes.h
 * Author: wuh20
 *
 * Created on Aug 5, 2018, 12:57:17 PM
 */

#ifndef TESTTIMES_H
#define TESTTIMES_H

#include <cppunit/extensions/HelperMacros.h>
#include "Times.h"

class testTimes : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(testTimes);
    
    CPPUNIT_TEST(testGetTimeIndex);

    CPPUNIT_TEST_SUITE_END();

public:
    testTimes();
    virtual ~testTimes();

private:
    void testGetTimeIndex();
};

#endif /* TESTTIMES_H */

