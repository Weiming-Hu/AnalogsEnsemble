/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   testFunctionsIO.h
 * Author: wuh20
 *
 * Created on Feb 10, 2020, 12:19:11 PM
 */

#ifndef TESTFUNCTIONSIO_H
#define TESTFUNCTIONSIO_H

#include <cppunit/extensions/HelperMacros.h>

class testFunctionsIO : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(testFunctionsIO);

    CPPUNIT_TEST(testParseFilename);

    CPPUNIT_TEST_SUITE_END();

public:
    testFunctionsIO();
    virtual ~testFunctionsIO();

private:
    void testParseFilename();
};

#endif /* TESTFUNCTIONSIO_H */

