/* 
 * File:   Functions.h
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on February 6, 2019, 2:58 PM
 */


#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "Times.h"
#include "AnEnDefaults.h"
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

    static const double _DEG2RAD = M_PI / 180;
    static const double _RAD2DEG = 180 / M_PI;

    /**
     * Computes a lookup table which maps from forecast time and lead time
     * indices to observation time indices. If the observation time index is
     * not found, the cell value in the table will stay untouched.
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
     * Computes the averaged decomposition of angles in degree, sine and cosine.
     * 
     * @param degs A vector of angles in degree.
     * @param s The averaged sine components of the input angles.
     * @param c The averaged cosine components of the input angles.
     */
    void meanCircularDecomp(const std::vector<double> & degs,
            double & s, double & c);

    /**
     * The estimator for standard deviation using Yamartino's algorithm.
     * 
     * @param s The averaged sine components of the input angles.
     * @param c The averaged cosine components of the input angles.
     * @return The standard deviation value in degree
     */
    double sdYamartino(const double & s, const double & c);

    /**
     * Computes the standard deviation for angles in degree.
     * 
     * @param values A vector of values.
     */
    double sdCircular(const std::vector<double> & degs);
    
    /**
     * Computes the mean of a vector.
     * @param values A vector of values.
     * @param max_nan_allowed The number of NAN values allowed in the
     * vector. Set it to NAN to allow any number of NAN values.
     */
    double mean(const std::vector<double> & values,
            size_t max_nan_allowed = AnEnDefaults::_MAX_SIZE_T);

    /**
     * Computes the variance of a vector.
     * @param values A vector of values.
     * @param average The average of input values.
     */
    double variance(const std::vector<double> & values);
    double variance(const std::vector<double> & values,
            const double & average);
    
    /**
     * Computes the difference of two circular numbers
     * 
     * @param i A double.
     * @param j A double.
     * @return  A double.
     */
    double diffCircular(double i, double j);

    /**************************************************************************
     *                          Template Functions                            *
     **************************************************************************/
    
    /**
     * Print multi_array and multi_array_view.
     * 
     * @param os output stream
     * @param arr multi_array obejct to be printed.
     */
    template <typename T>
    void print(std::ostream & os, const boost::multi_array<T, 2> & arr);
    template <typename T>
    void print(std::ostream & os, const boost::multi_array<T, 3> & arr);
    template <typename T>
    void print(std::ostream & os, const boost::multi_array<T, 4> & arr);
    template <typename T>
    void print(std::ostream & os, const boost::multi_array<T, 5> & arr);
    template <typename T, size_t NDims>
    void print(std::ostream & os, const array_view<T, NDims> & view);
    
    /**
     * Format a vector as a string for printing.
     * 
     * @param vec A vector
     * @param delim A string deliminator
     * @return A formatted string
     */
    template <typename T>
    std::string format(const std::vector<T> & vec,
            const std::string & delim = ",");
    
    /**
     * Format a pointer as a string for printing
     * @param ptr A pointer
     * @param len Length of the pointed object
     * @param delim A string deliminator
     * @return A formatted string
     */
    template <typename T>
    std::string format(const T* ptr, size_t len,
            const std::string & delim = ",");

    /**
     * Calculate the indices for each query object from the pool objects.
     * 
     * @param index A vector to store indices.
     * @param query Objects to query. It can be Parameters, Times, Stations.
     * @param pool Objects from which indices are generated. It can be
     * Parameters, Times, Stations.
     */
    template <class T>
    void toIndex(std::vector<size_t> & index, const T & query, const T & pool);
}

// Definition of template functions
#include "Functions.tpp"

#endif /* FUNCTIONS_H */
