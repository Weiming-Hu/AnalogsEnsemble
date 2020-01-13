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

    template <typename T, size_t NDims>
    using array_view = boost::detail::multi_array::multi_array_view<T, NDims>;
    using Matrix = boost::numeric::ublas::matrix<double>;
    

    /**
     * Computes a lookup table which maps from forecast time and lead time
     * indices to observation time indices.
     * 
     * @param fcst_times Forecast Times.
     * @param fcst_times_index The indices to compute mapping.
     * @param fcst_flts Forecast FLTs.
     * @param obs_times Observation Times.
     * @param table A matrix storing the indices with forecast times in rows
     * and forecast lead times in columns.
     */
    void updateTimeTable(
            const Times & fcst_times,
            const std::vector<size_t> & fcst_times_index,
            const Times & fcst_flts,
            const Times & obs_times,
            Matrix & table);

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
    template <typename T, size_t NDims>
    void print(std::ostream & os, const array_view<T, NDims> & view);
    
    template <typename T>
    std::string format(const std::vector<T> & vec,
            const std::string & delim = ",");
    template <typename T>
    std::string format(const T* ptr, size_t len,
            const std::string & delim = ",");

    template <class T>
    void toIndex(std::vector<size_t> & index, const T & query, const T & pool);
}

// Definition of template functions
#include "Functions.tpp"

#endif /* FUNCTIONS_H */
