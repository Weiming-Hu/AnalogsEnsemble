/*
 * File:   testTxt.h
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on Mar 14, 2021, 17:11
 */

#ifndef TESTTXT_H
#define TESTTXT_H

#include <cppunit/extensions/HelperMacros.h>
#include "Txt.h"

class testTxt : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(testTxt);

    CPPUNIT_TEST(testReadLines_);

    CPPUNIT_TEST_SUITE_END();

public:
    testTxt();
    virtual ~testTxt();

private:
    void testReadLines_();
};

#endif /* TESTTXT_H */

