/*
 * File:   testGribConverter.h
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on Oct 11, 2018, 4:37:58 PM
 */

#ifndef TESTGRIBCONVERTER_H
#define TESTGRIBCONVERTER_H

#include <cppunit/extensions/HelperMacros.h>

class testGribConverter : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(testGribConverter);

    CPPUNIT_TEST(testGetDoubles);

    CPPUNIT_TEST_SUITE_END();

public:
    testGribConverter();
    virtual ~testGribConverter();

    std::string file_grib1 = _PATH_GRIB1;
    
private:
    void testGetDoubles();
};

#endif /* TESTGRIBCONVERTER_H */

