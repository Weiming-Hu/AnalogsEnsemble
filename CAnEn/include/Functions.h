/* 
 * File:   Functions.h
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on February 6, 2019, 2:58 PM
 */


#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "Times.h"
#include "boost/multi_array.hpp"
#include "boost/numeric/ublas/matrix.hpp"

#include <vector>
#include <string>
#include <sstream>
#include <iostream>

namespace Functions {

    using Matrix = boost::numeric::ublas::matrix<double>;

    /**
     * Computes a lookup table which maps from forecast time and lead time
     * indices to observation time indices.
     * 
     * @param fcst_times Forecast Times.
     * @param fcst_flts Forecast FLTs.
     * @param obs_times Observation Times.
     * @param table A matrix storing the indices with forecast times in rows
     * and forecast lead times in columns.
     * @param start_row_i The starting row index for writing in the matrix.
     * This is helpful when you are updating part of the values in the matrix.
     */
    void updateTimeTable(
            const Times & fcst_times,
            const Times & fcst_flts,
            const Times & obs_times,
            Matrix & table,
            size_t start_row_i = 0);

    /**
     * Computes the standard deviation for linear numbers.
     * 
     * @param values A vector of values.
     */
    double sdLinear(const std::vector<double> & values);

    /**
     * Computes the standard deviation for circular numbers.
     * 
     * @param values A vector of values.
     */
    double sdCircular(const std::vector<double> & values);

    /**
     * Computes the mean of a vector.
     * @param values A vector of values.
     * @param max_nan_allowed The number of NAN values allowed in the
     * vector. Set it to NAN to allow any number of NAN values.
     */
    double mean(const std::vector<double> & values,
            double max_nan_allowed = NAN);

    /**
     * Computes the variance of a vector.
     * @param values A vector of values.
     */
    double variance(const std::vector<double> & values);

    /**
     * Computes the difference of two circular numbers
     * 
     * @param i A double.
     * @param j A double.
     * @return  A double.
     */
    double diffCircular(double i, double j);

    void setVerbose(int verbose);
    int getVerbose();

    /**************************************************************************
     *                          Template Functions                            *
     **************************************************************************/
    template <typename T>
    void print(std::ostream & os, const boost::multi_array<T, 3> & arr);
    template <typename T>
    void print(std::ostream & os, const boost::multi_array<T, 4> & arr);
    template <typename T>
    void print(std::ostream & os, const boost::multi_array<T, 5> & arr);
    template <typename T>
    std::string format(const std::vector<T> & vec,
            const std::string & delim = ",");

    template <class T>
    void toIndex(std::vector<size_t> & index, const T & query, const T & pool);
}

//#include "Analogs.h"
////#include "errorType.h"
//#include "Observations.h"
//#include "SimilarityMatrices.h"
//#include "StandardDeviation.h"
//#include "boost/numeric/ublas/matrix.hpp"
//
//#define errorType int
//
//
//class Functions {
//public:
//    Functions(int verbose);
//    Functions(const Functions& orig) = delete;
//    virtual ~Functions();
//    
//    /**
//     * AnEn::TimeMapMatrix is a lookup table to map time and FLT from
//     * forecasts to time of observations. The number of rows is the number
//     * of times in forecasts, and the number of columns is the number
//     * of FLTs in forecasts. The values are the times of the corresponding
//     * observation to the forecast at a specific time and FLT.
//     */
//    using TimeMapMatrix = boost::numeric::ublas::matrix<double>;
//
//    /**
//     * Computes the standard deviation of times of forecasts for each 
//     * parameter, each station, and each FLT. This function is designed to 
//     * generate NAN values only when all values for a parameter, a station, and
//     * a FLT are NAN. Otherwise, NAN values will be ignored.
//     * 
//     * See StandardDeviation for more information about storage.
//     * 
//     * @param forecasts Forecasts for which standard deviation is generated for.
//     * @param sds StandardDeviation to store the values.
//     * @param i_times the index for times that should be included in the 
//     * standard deviation calculation.
//     * @return An errorType.
//     */
//    errorType computeStandardDeviation(
//            const Forecasts_array & forecasts,
//            StandardDeviation & sds,
//            std::vector<size_t> i_times = {}) const;
//
//    /**
//     * Computes the search window for each FLT. the ranges are stored in a 
//     * matrix with the following shape:
//     *                   [num_flts][2]
//     * 
//     * The second dimension in sequence stores the start and the end FLT.
//     * 
//     * @param matrix A matrix to store the range of FLT windows.
//     * @param num_flts Number of FLTs.
//     * @param window_half_size Half size of the window.
//     * @return errorType.
//     */
//    errorType computeSearchWindows(
//            boost::numeric::ublas::matrix<size_t> & matrix,
//            size_t num_flts, size_t window_half_size) const;
//

//
//    /**
//     * Computes the index of each target in the container.
//     * 
//     * @param targets Targets that index will be computed for.
//     * @param container Container that index will be based on.
//     * @param indexes Index for targets.
//     * @return An errorType
//     */
//    errorType convertToIndex(
//            const anenTime::Times & targets,
//            const anenTime::Times & container,
//            std::vector<size_t> & indexes) const;
//

//    
//private:
//    
//    /**
//     * Specifies the verbose level.
//     * - 0: Quiet.
//     * - 1: Errors.
//     * - 2: The above plus Warnings.
//     * - 3: The above plus program progress information.
//     * - 4: The above plus check information.
//     * - 5: The above plus session information.
//     */
//    int verbose_ = 2;
//    
//};

// Definition of template functions
#include "Functions.tpp"

#endif /* FUNCTIONS_H */
