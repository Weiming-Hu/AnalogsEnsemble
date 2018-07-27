/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   testAnEnIO.h
 * Author: wuh20
 *
 * Created on Jun 29, 2018, 6:02:11 PM
 */

#ifndef TESTANENIO_H
#define TESTANENIO_H

#include <cppunit/extensions/HelperMacros.h>

#include "../AnEnIO.h"

class testAnEnIO : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(testAnEnIO);

    CPPUNIT_TEST(testReadObservationFile);
    CPPUNIT_TEST(testReadForecastFile);
    CPPUNIT_TEST(testWriteReadObservationFile);
    CPPUNIT_TEST(testWriteReadForecastFile);
    
    CPPUNIT_TEST_SUITE_END();

public:
    testAnEnIO();
    virtual ~testAnEnIO();
    void setUp();
    void tearDown();

private:
    void testReadObservationFile();
    void testReadForecastFile();
    void testWriteReadObservationFile();
    void testWriteReadForecastFile();
};

#endif /* TESTANENIO_H */

