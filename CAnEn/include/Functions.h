/* 
 * File:   Functions.h
 * Author: Weiming Hu <weiming@psu.edu>
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

class Functions {
public:
    Functions(int verbose);
    Functions(const Functions& orig) = delete;
    virtual ~Functions();
    
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
     * @param i_times the index for times that should be included in the 
     * standard deviation calculation.
     * @return An errorType.
     */
    errorType computeStandardDeviation(
            const Forecasts_array & forecasts,
            StandardDeviation & sds,
            std::vector<size_t> i_times = {}) const;

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
     * @return errorType.
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
     * @return An errorType.
     */
    errorType computeObservationsTimeIndices(
            const anenTime::Times & times_forecasts,
            const anenTime::Times & flts_forecasts,
            const anenTime::Times & times_observations,
            TimeMapMatrix & mapping, int time_match_mode = 1) const;

    /**
     * Computes the index of each target in the container.
     * 
     * @param targets Targets that index will be computed for.
     * @param container Container that index will be based on.
     * @param indexes Index for targets.
     * @return An errorType
     */
    errorType convertToIndex(
            const anenTime::Times & targets,
            const anenTime::Times & container,
            std::vector<size_t> & indexes) const;

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
     * Computes the sum of a vector.
     * @param values A vector of values.
     * @param max_nan_allowed The number of NAN values allowed in the
     * vector. Set it to NAN to allow any number of NAN values.
     */
    double sum(const std::vector<double> & values,
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
    
    void setVerbose(int verbose);
    int getVerbose();
    
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

