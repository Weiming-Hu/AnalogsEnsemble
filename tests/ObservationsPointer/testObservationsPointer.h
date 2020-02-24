/*
 * File:   testObservationsPointer.h
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on Jun 29, 2018, 5:37:58 PM
 */

#ifndef TESTOBSERVATIONSPOINTER_H
#define TESTOBSERVATIONSPOINTER_H

#include <cppunit/extensions/HelperMacros.h>

class testObservationsPointer : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(testObservationsPointer);

    CPPUNIT_TEST(testObservationValueSequence_);
    CPPUNIT_TEST(testSubset_);

    CPPUNIT_TEST_SUITE_END();

public:
    testObservationsPointer();
    virtual ~testObservationsPointer();
    
private:
    void testObservationValueSequence_();
    void testSubset_();
};

#endif /* TESTOBSERVATIONSPOINTER_H */

