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

class MathFunctions {
public:
    MathFunctions();
    MathFunctions(const MathFunctions& orig);
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
     * AnEn::SearchStationMatrix is a lookup table for search stations of each
     * test station. The number of rows is the number of test stations, and
     * the number of column is the maximum number of search stations for
     * each test stations. NA values can exist in the table because the 
     * numbers of search stations can vary for test stations.
     */
    using SearchStationMatrix = boost::numeric::ublas::matrix<double>;

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
            StandardDeviation & sds);

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
     * Select analogs based on the similarity matrices.
     * @param analogs Analogs object to write the analogs
     * @param sims SimilarityMatrices on which the selection is based
     * @param test_stations anenSta::Stations for the test.
     * @param search_observations Observations_array where the analog values
     * come from.
     * @param mapping A Boost Matrix for the mapping of times between
     * forecasts and observations. This is computed from the function
     * AnEn::computeObservationsTimeIndices.
     * @param i_parameter The index of the parameter to select in Observations.
     * @param num_members How many members each analog should have.
     * @param quick Whether to use quick sort mechanism.
     * @param extend_observations Whether to extend observation stations to
     * search stations. This only works when search space extension is used.
     * @return An AnEn::errorType.
     */
    errorType selectAnalogs(
            Analogs & analogs,
            SimilarityMatrices & sims,
            const anenSta::Stations & test_stations,
            const Observations_array& search_observations,
            const TimeMapMatrix & mapping,
            size_t i_parameter, size_t num_members,
            bool quick = true, bool extend_observations = false) const;

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

