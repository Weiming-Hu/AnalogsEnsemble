/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   testSimilarityMatrix.h
 * Author: wuh20
 *
 * Created on Aug 3, 2018, 12:53:03 PM
 */

#ifndef TESTSIMILARITYMATRIX_H
#define TESTSIMILARITYMATRIX_H

#include <cppunit/extensions/HelperMacros.h>

#include "Forecasts.h"
#include "SimilarityMatrices.h"

class testSimilarityMatrices : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(testSimilarityMatrices);
    
    CPPUNIT_TEST(testMatricesConstructor);
    CPPUNIT_TEST(testMatricesSort);
    CPPUNIT_TEST(testHasTargets);

    CPPUNIT_TEST_SUITE_END();

public:
    testSimilarityMatrices();
    virtual ~testSimilarityMatrices();

private:
    void testMatricesConstructor();
    void testMatricesSort();
    void testHasTargets();
};

#endif /* TESTSIMILARITYMATRIX_H */

