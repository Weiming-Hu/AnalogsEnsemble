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

    vector<double> vals_temp, vals_U, vals_V;
    long par_id = 0, level = 0;
    string type;

    // The location of the file is hard coded.
    string file = "/sapphire/s0/wuh20/Nextcloud/data/data_grib_test_PEF/nam_218_20180701_0000_000.grb2";

    // Get the U component
    par_id = 131;
    level = 0;
    type = "tropopause";
    cout << "Getting U" <<endl;
    getDoubles(vals_U, file, par_id, level, type);

    // Get the V component
    par_id = 132;
    level = 0;
    type = "tropopause";
    cout << "Getting V" <<endl;
    getDoubles(vals_V, file, par_id, level, type);

    // Get the temperature
    par_id = 130;
    level = 0;
    type = "surface";
    cout << "Getting temp" <<endl;
    getDoubles(vals_temp, file, par_id, level, type);

    cout << "Printing" <<endl;
    getDoubles(vals_temp, file, par_id, level, type);
    cout << "V component: " << vals_V << endl
        << "U component: " << vals_U << endl
        << "Temperature: " << vals_temp << endl;

    return;
}
