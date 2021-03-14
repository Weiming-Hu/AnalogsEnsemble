/*
 * File:   testTxt.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on Mar 14, 2021, 17:11
 */

#include <vector>
#include <numeric>
#include <iostream>

#include "testTxt.h"
#include "Functions.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION(testTxt);

testTxt::testTxt() {
}

testTxt::~testTxt() {
}

void
testTxt::testReadLines_() {

    /*
     * Test reading the current file
     */

    cout << "Reading " << __FILE__ << endl;

    vector<string> content;
    Txt::readLines(__FILE__, content);

    cout << Functions::format(content, "-------------- [end of entry] ||\n", 100) << endl;

    CPPUNIT_ASSERT(content.at(0) == "/*");
    CPPUNIT_ASSERT(content.at(6) == "using namespace std;");
}

