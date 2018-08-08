/* 
 * File:   SimilarityMatrices.h
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on August 2, 2018, 8:32 PM
 */

#ifndef SIMILARITYMATRIX_H
#define SIMILARITYMATRIX_H

#include "Forecasts.h"
#include "colorTexts.h"
#include "boost/multi_array.hpp"

#include <iostream>

/**
 * \class SimilarityMatrix
 * 
 * \brief SimilarityMatrix class is designed to store the similarity metric
 * for one test station at one FLT and one test time. Default dimensions are:
 *                     [Number of entries for comparison][3]
 * 
 * The second dimension stores in sequence station id, time id, and similarity
 * value.
 */
class SimilarityMatrix : public std::vector< std::vector< double > > {
public:
    SimilarityMatrix();
    SimilarityMatrix(size_t nrows);
    SimilarityMatrix(const SimilarityMatrix& orig);

    virtual ~SimilarityMatrix();

    void resize(size_t nrows);
    size_t nrows() const;
    size_t ncols() const;

    /**
     * Specifies the meaning of each column. The first column stores
     * station index, the second column stores time index, and the third
     * column stores value index.
     */
    enum COL_TAG {
        UNKNOWN = -999,
        STATION = 0,
        TIME = 1,
        VALUE = 2
    };

    bool sortRows(bool quick = true,
            size_t length = 0, COL_TAG col_tag = VALUE);

    /**
     * Check whether the matrix is ordered.
     * 
     * @return A boolean.
     */
    bool isOrdered();
    
    /**
     * Set values from vector input.
     * @param values
     */
    void setValues(std::vector<double> values);

    /**
     * Get the order tag.
     * 
     * @return SimilarityMatrix::TAG
     */
    COL_TAG getOrderTag();

    void print(std::ostream &) const;
    void printSize(std::ostream &) const;
    friend std::ostream & operator<<(std::ostream &,
            const SimilarityMatrix&);

    SimilarityMatrix& operator=(const SimilarityMatrix & right);
    
    const static int NUM_COLS = 3;

private:

    /**
     * Whether the matrix has been ordered.
     */
    bool ordered_ = false;

    /**
     * Which column is the order based on.
     */
    COL_TAG order_tag_ = UNKNOWN;
};

/**
 * \class SimilarityMatrices
 * 
 * \brief SimilarityMatrices class stores multiple SimilarityMatrix. Default 
 * dimensions of SimilarityMatrices are:
 *              [test stations][test times][test FLTs]
 */
class SimilarityMatrices : public boost::multi_array<SimilarityMatrix, 3> {
public:
    SimilarityMatrices();
    SimilarityMatrices(const SimilarityMatrices& orig) = delete;
    SimilarityMatrices(const Forecasts & forecasts);

    ~SimilarityMatrices();

    /**
     * Set the target forecast.
     * 
     * @param target A Forecasts with one station, one time, one FLT,
     * and all parameters.
     */
    void setTargets(const Forecasts & targets);
    
    /**
     * Order all SimilarityMatrix using a specific column.
     * 
     * @return Whether the order succeeded.
     */
    bool sortRows(bool quick = true, size_t length = 0,
            SimilarityMatrix::COL_TAG col_tag = SimilarityMatrix::COL_TAG::VALUE);


    /**
     * Get the target forecast values. Since forecasts only have parameters
     * for one station, one FLT, and one time. Values would a vector of
     * all the parameters.
     * 
     * @return A vector of double.
     */
    const Forecasts_array & getTargets() const;

    void print(std::ostream &) const;
    void printSize(std::ostream &) const;
    friend std::ostream & operator<<(std::ostream &,
            const SimilarityMatrices&);

private:

    /**
     * The corresponding forecasts for each SimilarityMatrix.
     */
    Forecasts_array targets_;
};


#endif /* SIMILARITYMATRIX_H */

