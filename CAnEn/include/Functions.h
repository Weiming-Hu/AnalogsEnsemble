/* 
 * File:   Functions.h
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on February 6, 2019, 2:58 PM
 */


#ifndef FUNCTIONS_H
#define FUNCTIONS_H

// This include file is needed because there is a bug in older versions of the ulas library
//
// Reference: https://stackoverflow.com/questions/44534516/error-make-array-is-not-a-member-of-boostserialization
//
#include <boost/serialization/array_wrapper.hpp>

#include "Times.h"
#include "Config.h"
#include "Forecasts.h"
#include "Observations.h"
#include "boost/numeric/ublas/matrix.hpp"
#include "boost/numeric/ublas/io.hpp"

#include <vector>
#include <string>
#include <sstream>
#include <iostream>

namespace Functions {

    static const std::size_t _MAX_SIZE_T = std::numeric_limits<std::size_t>::max();

    /**
     * The Matrix type is from boost uBLAS matrix.
     * 
     * It is a double matrix because it needs to be ablt to hold NAN.
     * 
     * Its internal storage is in column major because it is easier to be
     * converted to an R structure and for file I/O with file formats like NetCDF. 
     * 
     * The storage type is std::vector because the default storage type,
     * unbounded_array, does not model sequence operators. Using vectors will
     * make it easier for value initialization. Some basic profiling has shown
     * that the creation will be slightly slower, but the indexing will be 
     * slightly faster when std::vector is used.
     * 
     */
    using Matrix = boost::numeric::ublas::matrix<
            double, boost::numeric::ublas::column_major, std::vector<double> >;

    void setSearchStations(const Stations &, Matrix & table, double);

    /**
     * Convert an integer to Verbose and vice versa
     * 
     * @param An integer
     * @return A Verbose
     */
    Verbose itov(int);
    int vtoi(Verbose);
    std::string vtos(Verbose);

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
            const std::vector<std::size_t> & fcst_times_index,
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
     * Computes the standard deviation for angles in degree.
     * 
     * @param values A vector of values.
     */
    double sdCircular(const std::vector<double> & degs);


    double sum(const std::vector<double> & values,
            std::size_t max_nan_allowed = _MAX_SIZE_T);

    /**
     * Computes the mean of a vector.
     * @param values A vector of values.
     * @param max_nan_allowed The number of NAN values allowed in the
     * vector. Set it to NAN to allow any number of NAN values.
     */
    double mean(const std::vector<double> & values,
            std::size_t max_nan_allowed = _MAX_SIZE_T);

    /**
     * Computes the sum of a vector.
     * @param values A vector of values.
     * @param max_nan_allowed The number of NAN values allowed in the
     * vector. Set it to NAN to allow any number of NAN values.
     */
    double sum(const std::vector<double> & values,
            const double max_nan_allowed = NAN);

    /**
     * Computes the variance of a vector.
     * @param values A vector of values.
     * @param average The average of input values.
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

    /**
     * Computes the Levenshtein distance of two strings. The function can be
     * used to guess the intended argument of an unknown one. This function is
     * used by the function Functions::guess_arguments.
     * 
     * The implementation is referenced from Github
     * https://github.com/git/git/blob/master/levenshtein.h
     *
     * Thanks to the pointer of Vlad Lazarenko
     * http://lazarenko.me/smart-getopt/
     * 
     * @param str1 A string.
     * @param str2 A string.
     * @param w Weight suggested by GitHub.
     * @param s Weight suggested by GitHub.
     * @param a Weight suggested by GitHub.
     * @param d Weight suggested by GitHub.
     * @return A distance measure
     */
    size_t levenshtein(const std::string & str1, const std::string & str2,
            size_t w = 0, size_t s = 2, size_t a = 1, size_t d = 3);
    
    /**
     * Convert a date time string to the number of seconds since origin time.
     * 
     * The format should follow the section 'Construct from String' from here
     * https://www.boost.org/doc/libs/1_72_0/doc/html/date_time/posix_time.html
     * 
     * @param datetime_str A date time string for start time
     * @param origin_str A date time string for the original time
     * @param iso_string Whether the string is in ISO format
     * @return The number of seconds since the origin;
     */
    long toSeconds(const std::string & datetime_str,
            const std::string & origin_str, bool iso_string);

    /**
     * Collapse the time and lead time dimensions of a forecasts and convert
     * them to observations;
     * @param  Observations
     * @param  Forecasts
     */
    void collapseLeadTimes(Observations &, const Forecasts &);

    /**************************************************************************
     *                          Template Functions                            *
     **************************************************************************/

    /**
     * Format a vector as a string for printing.
     * 
     * @param vec A vector
     * @param len Length of the pointed object
     * @param ptr A pointer
     * @param delim A string deliminator
     * @return A formatted string
     */
    template <typename T>
    std::string format(const std::vector<T> & vec, const std::string & delim = ",");
    template <typename T>
    std::string format(const T* ptr, std::size_t len, const std::string & delim = ",");

    /**
     * Calculate the indices for each query object from the pool objects.
     * 
     * @param index A vector to store indices.
     * @param query Objects to query. It can be Parameters, Times, Stations.
     * @param pool Objects from which indices are generated. It can be
     * Parameters, Times, Stations.
     */
    template <class T>
    void toIndex(std::vector<std::size_t> & index, const T & query, const T & pool);

    /**
     * Guess the unregistered parameters.
     * 
     * @param unregistered_keys The unregistered keys returned by 
     * boost::program_options::collect_unrecognized.
     * @param available_options A vector of string for available options.
     * @param os An output stream.
     */
    template <class T>
    void guess_arguments(
            const std::vector< std::basic_string<T> > & unregistered_keys,
            const std::vector<std::string> & available_options,
            std::ostream & os);
}

// Definition of template functions
#include "Functions.tpp"

#endif /* FUNCTIONS_H */
