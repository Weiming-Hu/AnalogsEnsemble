/*
 * File:   testGribConverter.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on Oct 11, 2018, 4:37:58 PM
 */

#include "testGribConverter.h"

#include "gribConverterFunctions.h"

using namespace gribConverter;

CPPUNIT_TEST_SUITE_REGISTRATION(testGribConverter);

testGribConverter::testGribConverter() {
}

testGribConverter::~testGribConverter() {
}

void testGribConverter::testGetDoubles() {

    vector<double> vals_U, vals_V;
    long par_id = 0, level = 0;
    string type;

    // The location of the file is hard coded.
    string file = file_grib1;

    // Get the V component
    par_id = 132;
    level = 850;
    type = "isobaricInhPa";
    getDoubles(vals_V, file, par_id, level, type);

    // Get the U component
    par_id = 131;
    level = 1000;
    type = "isobaricInhPa";
    getDoubles(vals_U, file, par_id, level, type);

    // Verification
    size_t max = 10;
    vector<double> veri_U = {2.9, 2.9, 2.9, 2.9, 2.9, 2.9, 2.9, 2.9, 2.9, 2.9},
        veri_V = {2.8, 2.8, 2.8, 2.8, 2.8, 2.8, 2.8, 2.8, 2.8, 2.8};

    for (size_t i = 0; i < max; i++) {
        CPPUNIT_ASSERT((int) (veri_U[i] * 100)== (int) (vals_U[i] * 100));
        CPPUNIT_ASSERT((int) (veri_V[i] * 100) == (int) (vals_V[i] * 100));
    }

    return;
}
