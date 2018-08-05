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
#include "boost/numeric/ublas/matrix.hpp"
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
class SimilarityMatrix : public boost::numeric::ublas::matrix<double> {
public:
    SimilarityMatrix();
    SimilarityMatrix(size_t nrows);
    SimilarityMatrix(size_t nrows, size_t ncols);
    SimilarityMatrix(const SimilarityMatrix& orig);
    
    virtual ~SimilarityMatrix();

    /**
     * Specifies the meaning of each column. The first column stores
     * station index, the second column stores time index, and the third
     * column stores value index.
     */
    enum TAG {
        UNKNOWN = -999,
        STATION = 0,
        TIME = 1,
        VALUE = 2
    };

    /**
     * Set the order tag SimilarityMatrix::TAG.
     * 
     * @param tag
     */
    void setOrderTag(TAG tag);

    /**
     * Check whether the matrix is ordered.
     * 
     * @return A boolean.
     */
    bool isOrdered();

    /**
     * Get the order tag.
     * 
     * @return SimilarityMatrix::TAG
     */
    TAG getOrderTag();
    
    void print(std::ostream &) const;
    void printSize(std::ostream &) const;
    friend std::ostream & operator<<(std::ostream &,
            const SimilarityMatrix&);
    
    SimilarityMatrix& operator=(const SimilarityMatrix& right);


private:

    /**
     * Whether the matrix has been ordered.
     */
    bool ordered_ = false;

    /**
     * Which column is the order based on.
     */
    TAG order_tag_ = UNKNOWN;
};

/**
 * \class SimilarityMatrices
 * 
 * \brief SimilarityMatrices class stores multiple SimilarityMatrix. Default 
 * dimensions are:
 *              [test stations][test times][test FLTs]
 */
class SimilarityMatrices : public boost::multi_array<SimilarityMatrix, 3> {
public:
    SimilarityMatrices() = delete;
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
     * One Forecast entry for one station, one time, one FLT,
     * and all parameters.
     */
    Forecasts_array targets_;
};


#endif /* SIMILARITYMATRIX_H */

