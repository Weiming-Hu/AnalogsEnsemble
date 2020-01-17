/*
 * File:   testObservationsArray.h
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on Jun 29, 2018, 5:37:58 PM
 */

#ifndef TESTOBSERVATIONSARRAY_H
#define TESTOBSERVATIONSARRAY_H

#include <cppunit/extensions/HelperMacros.h>

class testObservationsArray : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(testObservationsArray);

    CPPUNIT_TEST(testObservationValueSequence_);

    CPPUNIT_TEST_SUITE_END();

public:
    testObservationsArray();
    virtual ~testObservationsArray();
    
private:
    void testObservationValueSequence_();
};

#endif /* TESTOBSERVATIONSARRAY_H */

