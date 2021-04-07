/*
 * File:   testGrid.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on April 3, 2021, 13:23
 */

#include <iostream>
#include <boost/numeric/ublas/io.hpp>

#include "testGrid.h"
#include "GridQuery.h"
#include "AnEnReadNcdf.h"
#include "ForecastsPointer.h"

CPPUNIT_TEST_SUITE_REGISTRATION(testGrid);

using namespace std;

testGrid::testGrid() {
}

testGrid::~testGrid() {
}

void
testGrid::testReadPrint_() {

    /**
     * Test Grid to read from a text file and print information to the console
     */

    Grid grid;
    grid.setup(file_grid1);

    cout << "Summary: " << grid.summary() << endl
        << "Details: " << endl << grid << endl;

    cout << "Location (2, 3): " << grid(2, 3) << endl;
    cout << "Key 5: " << grid[5] << endl;

    CPPUNIT_ASSERT(grid(2, 3) == 9);

    RowCol row_col = {0, 1};
    CPPUNIT_ASSERT(grid[5] == row_col);

    try {
        cout << "key 12: " << grid[12] << endl;
    } catch (exception & e) {
        cout << "Caught the following error: " << e.what() << endl;
        return;
    }

    // The function should not have gone this far. The error detection has gone wrong!
    CPPUNIT_ASSERT(false);
}

void
testGrid::testRectangleMask_() {

    /**
     * Test Grid to generate a rectangle mask
     */

    Grid grid(file_grid1);

    Matrix mask;

    cout << "Without padding ..." << endl;

    grid.getRectangle(15, mask, 3, 3);
    cout << mask << endl;

    CPPUNIT_ASSERT(mask(0, 0) == 0);
    CPPUNIT_ASSERT(mask(0, 1) == 5);
    CPPUNIT_ASSERT(mask(0, 2) == 2);
    CPPUNIT_ASSERT(mask(1, 0) == 4);
    CPPUNIT_ASSERT(mask(1, 1) == 15);
    CPPUNIT_ASSERT(mask(1, 2) == 6);
    CPPUNIT_ASSERT(mask(2, 0) == 20);
    CPPUNIT_ASSERT(mask(2, 1) == 7);
    CPPUNIT_ASSERT(mask(2, 2) == 13);

    grid.getRectangle(20, mask, 3, 3);
    cout << mask << endl;

    CPPUNIT_ASSERT(std::isnan(mask(0, 0)));
    CPPUNIT_ASSERT(mask(0, 1) == 4);
    CPPUNIT_ASSERT(mask(0, 2) == 15);
    CPPUNIT_ASSERT(std::isnan(mask(1, 0)));
    CPPUNIT_ASSERT(mask(1, 1) == 20);
    CPPUNIT_ASSERT(mask(1, 2) == 7);
    CPPUNIT_ASSERT(std::isnan(mask(2, 0)));
    CPPUNIT_ASSERT(std::isnan(mask(2, 1)));
    CPPUNIT_ASSERT(std::isnan(mask(2, 2)));

    grid.getRectangle(0, mask, 1, 3);
    cout << mask << endl;

    CPPUNIT_ASSERT(std::isnan(mask(0, 0)));
    CPPUNIT_ASSERT(mask(1, 0) == 0);
    CPPUNIT_ASSERT(mask(2, 0) == 4);

    grid.getRectangle(0, mask, 5, 3);
    cout << mask << endl;

    CPPUNIT_ASSERT(std::isnan(mask(0, 0)));
    CPPUNIT_ASSERT(std::isnan(mask(0, 1)));
    CPPUNIT_ASSERT(std::isnan(mask(0, 2)));
    CPPUNIT_ASSERT(std::isnan(mask(0, 3)));
    CPPUNIT_ASSERT(std::isnan(mask(0, 4)));
    CPPUNIT_ASSERT(std::isnan(mask(1, 0)));
    CPPUNIT_ASSERT(std::isnan(mask(1, 1)));
    CPPUNIT_ASSERT(mask(1, 2) == 0);
    CPPUNIT_ASSERT(mask(1, 3) == 5);
    CPPUNIT_ASSERT(mask(1, 4) == 2);
    CPPUNIT_ASSERT(std::isnan(mask(2, 0)));
    CPPUNIT_ASSERT(std::isnan(mask(2, 1)));
    CPPUNIT_ASSERT(mask(2, 2) == 4);
    CPPUNIT_ASSERT(mask(2, 3) == 15);
    CPPUNIT_ASSERT(mask(2, 4) == 6);

    grid.getRectangle(16, mask, 3, 1);
    cout << mask << endl;

    CPPUNIT_ASSERT(mask(0, 0) == 6);
    CPPUNIT_ASSERT(mask(0, 1) == 16);
    CPPUNIT_ASSERT(std::isnan(mask(0, 2)));

    grid.getRectangle(16, mask, 3, 3);
    cout << mask << endl;

    CPPUNIT_ASSERT(mask(0, 0) == 2);
    CPPUNIT_ASSERT(std::isnan(mask(0, 1)));
    CPPUNIT_ASSERT(std::isnan(mask(0, 2)));
    CPPUNIT_ASSERT(mask(1, 0) == 6);
    CPPUNIT_ASSERT(mask(1, 1) == 16);
    CPPUNIT_ASSERT(std::isnan(mask(1, 2)));
    CPPUNIT_ASSERT(mask(2, 0) == 13);
    CPPUNIT_ASSERT(mask(2, 1) == 9);
    CPPUNIT_ASSERT(std::isnan(mask(2, 2)));

    cout << "With same padding ..." << endl;

    grid.getRectangle(15, mask, 3, 3, true);
    cout << mask << endl;

    CPPUNIT_ASSERT(mask(0, 0) == 0);
    CPPUNIT_ASSERT(mask(0, 1) == 5);
    CPPUNIT_ASSERT(mask(0, 2) == 2);
    CPPUNIT_ASSERT(mask(1, 0) == 4);
    CPPUNIT_ASSERT(mask(1, 1) == 15);
    CPPUNIT_ASSERT(mask(1, 2) == 6);
    CPPUNIT_ASSERT(mask(2, 0) == 20);
    CPPUNIT_ASSERT(mask(2, 1) == 7);
    CPPUNIT_ASSERT(mask(2, 2) == 13);

    grid.getRectangle(20, mask, 3, 3, true);
    cout << mask << endl;

    CPPUNIT_ASSERT(mask(0, 0) == 4);
    CPPUNIT_ASSERT(mask(0, 1) == 4);
    CPPUNIT_ASSERT(mask(0, 2) == 15);
    CPPUNIT_ASSERT(mask(1, 0) == 20);
    CPPUNIT_ASSERT(mask(1, 1) == 20);
    CPPUNIT_ASSERT(mask(1, 2) == 7);
    CPPUNIT_ASSERT(mask(2, 0) == 20);
    CPPUNIT_ASSERT(mask(2, 1) == 20);
    CPPUNIT_ASSERT(mask(2, 2) == 7);

    grid.getRectangle(0, mask, 1, 3, true);
    cout << mask << endl;

    CPPUNIT_ASSERT(mask(0, 0) == 0);
    CPPUNIT_ASSERT(mask(1, 0) == 0);
    CPPUNIT_ASSERT(mask(2, 0) == 4);

    grid.getRectangle(0, mask, 5, 3, true);
    cout << mask << endl;

    CPPUNIT_ASSERT(mask(0, 0) == 0);
    CPPUNIT_ASSERT(mask(0, 1) == 0);
    CPPUNIT_ASSERT(mask(0, 2) == 0);
    CPPUNIT_ASSERT(mask(0, 3) == 5);
    CPPUNIT_ASSERT(mask(0, 4) == 2);
    CPPUNIT_ASSERT(mask(1, 0) == 0);
    CPPUNIT_ASSERT(mask(1, 1) == 0);
    CPPUNIT_ASSERT(mask(1, 2) == 0);
    CPPUNIT_ASSERT(mask(1, 3) == 5);
    CPPUNIT_ASSERT(mask(1, 4) == 2);
    CPPUNIT_ASSERT(mask(2, 0) == 4);
    CPPUNIT_ASSERT(mask(2, 1) == 4);
    CPPUNIT_ASSERT(mask(2, 2) == 4);
    CPPUNIT_ASSERT(mask(2, 3) == 15);
    CPPUNIT_ASSERT(mask(2, 4) == 6);

    grid.getRectangle(16, mask, 3, 1, true);
    cout << mask << endl;

    CPPUNIT_ASSERT(mask(0, 0) == 6);
    CPPUNIT_ASSERT(mask(0, 1) == 16);
    CPPUNIT_ASSERT(mask(0, 2) == 16);

    try {
        grid.getRectangle(16, mask, 3, 3, true);
    } catch (exception & e) {
        CPPUNIT_ASSERT(e.what() == string("NAN is not allowed when using same padding"));
        return;
    }

    // The function should not have gone this far. The error detection has gone wrong!
    CPPUNIT_ASSERT(false);
}

void
testGrid::testQuery_() {

    /*
     * Test querying a forecasts with a mask
     */

    Grid grid(file_grid2);
    cout << "Grid: " << endl << grid << endl;

    AnEnReadNcdf anen_read;
    ForecastsPointer forecasts;
    anen_read.readForecasts(file_forecasts, forecasts);

    Matrix mask;
    grid.getRectangle(7, mask, 3, 3);
    cout << mask << endl;

    Array4DPointer arr;
    GridQuery::rectangle(mask, 3, 0, 1, forecasts, arr);
    cout << arr << endl;

    vector<size_t> lead_times {0, 1};

    for (size_t i = 0; i < lead_times.size(); ++i) {
        for (size_t j = 0; j < forecasts.getParameters().size(); ++j) {
            for (size_t k = 0; k < 3; ++k) {
                for (size_t m = 0; m < 3; ++m) {
                    double lhs = arr.getValue(i, j, k, m);
                    double rhs = forecasts.getValue(j, mask(k, m), 3, lead_times.at(i));
                    CPPUNIT_ASSERT(lhs == rhs);
                }
            }
        }
    }
}
