/*
 * File:   testGrid.h
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on April 3, 2021, 13:22
 */

#ifndef TESTGRID_H
#define TESTGRID_H

#include <cppunit/extensions/HelperMacros.h>
#include "Grid.h"

class testGrid : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(testGrid);

    CPPUNIT_TEST(testReadPrint_);
    CPPUNIT_TEST(testRectangleMask_);
    //CPPUNIT_TEST(testQuery_);

    CPPUNIT_TEST_SUITE_END();

public:
    testGrid();
    virtual ~testGrid();

    std::string file_grid1 = _PATH_GRID1;
    std::string file_grid2 = _PATH_GRID2;
    std::string file_forecasts = _PATH_FORECASTS;

private:
    void testReadPrint_();
    void testRectangleMask_();
    //void testQuery_();
};

#endif /* TESTGRID_H */

