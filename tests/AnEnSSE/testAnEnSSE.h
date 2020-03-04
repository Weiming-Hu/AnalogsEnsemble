/*
 * File:   testAnEnSSE.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on Aug 4, 2018, 4:09:20 PM
 */

#ifndef TESTANEN_H
#define TESTANEN_H

#include <cppunit/extensions/HelperMacros.h>

class testAnEnSSE : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(testAnEnSSE);

    CPPUNIT_TEST(testCompute_);
    CPPUNIT_TEST(testMultiAnEn_);

    CPPUNIT_TEST_SUITE_END();

public:
    testAnEnSSE();
    virtual ~testAnEnSSE();

private:
    void testCompute_();
    void testMultiAnEn_();
};

#endif /* TESTANEN_H */
