/*
 * File:   testAnEnIOMPI.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on Mar 4, 2020, 4:09:20 PM
 */

#ifndef TESTAnEnIOMPI_H
#define TESTAnEnIOMPI_H

#include "AnEnReadGribMPI.h"
#include "ParameterGrib.h"
#include "Forecasts.h"
#include "Observations.h"

#include <cppunit/extensions/HelperMacros.h>

class testAnEnIOMPI : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(testAnEnIOMPI);

    CPPUNIT_TEST(testReadGrib_);

    CPPUNIT_TEST_SUITE_END();

public:
    testAnEnIOMPI();
    virtual ~testAnEnIOMPI();

    void setUp();
    void tearDown();


    // Public members to control the file I/O process.
    // These variables are set up in the setUp function.
    //
    std::string data_folder;
    std::vector<std::string> data_files;
    std::vector<ParameterGrib> grib_parameters;

    int world_rank;
    int num_proc;

    bool delimited;
    std::string regex_str;
    size_t unit_in_seconds;
    std::vector<int> stations_index;


private:
    void testReadGrib_();

};

#endif /* TESTAnEnIOMPI_H */
