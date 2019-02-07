/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   MathFunctions.h
 * Author: guido
 *
 * Created on February 6, 2019, 2:58 PM
 */

#ifndef MATHFUNCTIONS_H
#define MATHFUNCTIONS_H

#include "Analogs.h"
#include "errorType.h"
#include "Observations.h"
#include "SimilarityMatrices.h"
#include "StandardDeviation.h"
#include "boost/numeric/ublas/matrix.hpp"

class MathFunctions {
public:
    MathFunctions(int verbose);
    MathFunctions(const MathFunctions& orig) = delete;
    virtual ~MathFunctions();
    
    /**
     * AnEn::TimeMapMatrix is a lookup table to map time and FLT from
     * forecasts to time of observations. The number of rows is the number
     * of times in forecasts, and the number of columns is the number
     * of FLTs in forecasts. The values are the times of the corresponding
     * observation to the forecast at a specific time and FLT.
     */
    using TimeMapMatrix = boost::numeric::ublas::matrix<double>;

    /**
     * Computes the standard deviation of times of forecasts for each 
     * parameter, each station, and each FLT. This function is designed to 
     * generate NAN values only when all values for a parameter, a station, and
     * a FLT are NAN. Otherwise, NAN values will be ignored.
     * 
     * See StandardDeviation for more information about storage.
     * 
     * @param forecasts Forecasts for which standard deviation is generated for.
     * @param sds StandardDeviation to store the values.
     * @return An AnEn::errorType.
     */
    errorType computeStandardDeviation(
            const Forecasts_array & forecasts,
            StandardDeviation & sds) const;

    /**
     * Computes the search window for each FLT. the ranges are stored in a 
     * matrix with the following shape:
     *                   [num_flts][2]
     * 
     * The second dimension in sequence stores the start and the end FLT.
     * 
     * @param matrix A matrix to store the range of FLT windows.
     * @param num_flts Number of FLTs.
     * @param window_half_size Half size of the window.
     * @return AnEn::errorType.
     */
    errorType computeSearchWindows(
            boost::numeric::ublas::matrix<size_t> & matrix,
            size_t num_flts, size_t window_half_size) const;

    /**
     * Computes the corresponding indices for observation times from 
     * forecast times and FLTs.
     * 
     * @param times_forecasts Forecast anenTimes::Time.
     * @param flts_forecasts Forecast anenTimes::FLTs.
     * @param times_observations Observation anenTimes::Time.
     * @param mapping A matrix stores the indices.
     * @param time_match_mode An integer specifying how to deal with missing observation
     * times. 0 stands for strict search. It will throw an error when a forecast time
     * cannot be found in observation times. 1 stands for loose search. It will insert
     * NA into the matrix when a observation time cannot be found for a foreacst time.
     * @return An AnEn::errorType.
     */
    errorType computeObservationsTimeIndices(
            const anenTime::Times & times_forecasts,
            const anenTime::Times & flts_forecasts,
            const anenTime::Times & times_observations,
            TimeMapMatrix & mapping, int time_match_mode = 1) const;

    /**
     * Computes the standard deviation for linear numbers.
     * 
     * @param values A vector of values.
     */
    double sdLinear(const std::vector<double> & values) const;

    /**
     * Computes the standard deviation for circular numbers.
     * 
     * @param values A vector of values.
     */
    double sdCircular(const std::vector<double> & values) const;

    /**
     * Computes the mean of a vector.
     * @param values A vector of values.
     * @param max_nan_allowed The number of NAN values allowed in the
     * vector. Set it to NAN to allow any number of NAN values.
     */
    double mean(const std::vector<double> & values,
            const double max_nan_allowed = NAN) const;

    /**
     * Computes the variance of a vector.
     * @param values A vector of values.
     */
    double variance(const std::vector<double> & values) const;

    /**
     * Computes the difference of two circular numbers
     * 
     * @param i A double.
     * @param j A double.
     * @return  A double.
     */
    double diffCircular(double i, double j) const;
    
private:
    
    /**
     * Specifies the verbose level.
     * - 0: Quiet.
     * - 1: Errors.
     * - 2: The above plus Warnings.
     * - 3: The above plus program progress information.
     * - 4: The above plus check information.
     * - 5: The above plus session information.
     */
    int verbose_ = 2;
    
};

#endif /* MATHFUNCTIONS_H */

