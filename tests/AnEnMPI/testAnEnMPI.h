/*
 * File:   testAnEnMPI.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on Mar 4, 2020, 4:09:20 PM
 */

#ifndef TESTAnEnMPI_H
#define TESTAnEnMPI_H

#include <cppunit/extensions/HelperMacros.h>

class testAnEnMPI : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(testAnEnMPI);

    CPPUNIT_TEST(testCompute_);

    CPPUNIT_TEST_SUITE_END();

public:
    testAnEnMPI();
    virtual ~testAnEnMPI();

private:
    void testCompute_();

};

#endif /* TESTAnEnMPI_H */
