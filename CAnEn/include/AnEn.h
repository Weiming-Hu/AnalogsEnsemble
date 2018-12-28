/* 
 * File:   AnEn.h
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on July 23, 2018, 10:56 PM
 */

#ifndef ANEN_H
#define ANEN_H

#include "Analogs.h"
#include "Forecasts.h"
#include "Observations.h"
#include "SimilarityMatrices.h"
#include "StandardDeviation.h"
#include "boost/numeric/ublas/matrix.hpp"

/**
 * \class AnEn
 * 
 * \brief AnEn class provides interfaces for computing Analog Ensembles.
 * 
 */
class AnEn {
public:
    AnEn();
    AnEn(int verbose);
    AnEn(const AnEn& orig) = delete;

    virtual ~AnEn();
    
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
     * Specifies the how similarity is computed across stations.
     * 
     * - ONE_TO_ONE: Each test station is assigned with the closest station in the
     * search stations;
     * - ONE_TO_MANY: Each test station is assigned with a set of nearby stations
     * in the search stations (Search space extension);
     * 
     */
    enum simMethod {
        /// 1
        ONE_TO_ONE = 1, 
        /// 2
        ONE_TO_MANY = 2
    };

    /**
     * Specifies the return error type of a function. Use AnEn::handleError
     * to handle the returned errorType.
     */
    enum errorType {
        /// 0
        SUCCESS = 0, 
        /// -1
        UNKNOWN_METHOD = -1, 
        /// -2
        MISSING_VALUE = -2, 
        /// -10
        WRONG_SHAPE = -10, 
        /// -11
        WRONG_METHOD = -11, 
        /// -20
        OUT_OF_RANGE = -20 
    };

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
     * Computes the search stations of each test stations.
     * 
     * @param test_stations Test anenSta::Stations.
     * @param search_stations Search anenSta::Stations.
     * @param i_search_stations A matrix to store the search stations for each
     * test stations per row.
     * @param max_num_search_stations The maximum number of search stations
     * that will be stored.
     * @param distance The buffer distance from each test station to look
     * for search stations.
     * @param num_nearest_stations The number of KNN search stations to look
     * for for each test stations.
     * @param return_index Whether to return the index of the ID of the search
     * stations.
     * @return An AnEn::errorType;
     */
    errorType computeSearchStations(
            const anenSta::Stations & test_stations,
            const anenSta::Stations & search_stations,
            SearchStationMatrix & i_search_stations,
            size_t max_num_search_stations = 1,
            double distance = 0, size_t num_nearest_stations = 0,
            bool return_index = true) const;

    /**
     * Computes the similarity matrices.
     * 
     * @param search_forecasts Forecasts to search.
     * @param sds Pre-computed standard deviation of the forecasts to search. 
     * This can be computed from the function AnEn::computeStandardDeviation;
     * @param sims A SimilarityMatrices.
     * @param search_observations Observations to search.
     * @param mapping A matrix stores the mapping indices from forecasts times
     * and FLTs to observation times.
     * @param i_search_stations A precomputed matrix with the search stations 
     * for each test station stored per row.
     * @param i_observation_parameter The parameter in observations that will
     * be checked for NAN values. By default it is 0 pointing at the first
     * parameter.
     * @param extend_observations Whether to extend observation stations to
     * search stations. This only works when search space extension is used.
     * @param max_par_nan The number of NAN values allowed when computing
     * similarity across different parameters. Set it to NAN to allow any number
     * of NAN values.
     * @param max_flt_nan The number of NAN values allowed when computing
     * FLT window averages. Set it to NAN to allow any number of NAN values.
     * 
     * @return An AnEn::errorType.
     */
    errorType computeSimilarity(
            const Forecasts_array & search_forecasts,
            const StandardDeviation & sds,
            SimilarityMatrices & sims,
            const Observations_array& search_observations,
            const TimeMapMatrix & mapping,
            const SearchStationMatrix & i_search_stations,
            size_t i_observation_parameter = 0,
            bool extend_observations = false,
            double max_par_nan = 0, double max_flt_nan = 0) const;

    /**
     * Select analogs based on the similarity matrices.
     * @param analogs Analogs object to write the analogs
     * @param sims SimilarityMatrices on which the selection is based
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
     * @param preserve_real_time Whether to replace the observation time index
     * with the actual observation time.
     * @return An AnEn::errorType.
     */
    errorType selectAnalogs(
            Analogs & analogs,
            SimilarityMatrices & sims,
            const Observations_array& search_observations,
            const TimeMapMatrix & mapping,
            size_t i_parameter, size_t num_members,
            bool quick = true, bool extend_observations = false,
            bool preserve_real_time = false) const;

    /**
     * Handles the errorType.
     * 
     * @param indicator An AnEn::errorType.
     */
    void handleError(const errorType & indicator) const;

    int getVerbose() const;
    simMethod getMethod() const;

    void setMethod(simMethod method);
    void setVerbose(int verbose);

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

    static const double _FILL_VALUE;

private:
    /**
     * Specifies the verbose level.
     * - 0: Quiet.
     * - 1: Errors.
     * - 2: The above plus Warnings.
     * - 3: The above plus program progress information.
     * - 4: The above plus session information.
     */
    int verbose_ = 2;

    /*
     * This variable specifies how similarity matrices are computed across stations.
     */
    simMethod method_ = ONE_TO_ONE;

    /**
     * Check input.
     * @param search_forecasts Search forecasts.
     * @param sims Similarity matrices provides the test forecasts.
     * @param search_observations Search observations.
     * @return An AnEn::errorType.
     */
    errorType check_input_(
            const Forecasts_array& search_forecasts,
            const StandardDeviation& sds,
            SimilarityMatrices& sims,
            const Observations_array& search_observations,
            TimeMapMatrix mapping,
            size_t i_observation_parameter) const;

    /**
     * This function is a special case of AnEn::computeSearchStations.
     * It finds the nearest search station to a specific test station.
     * If location information is not provided, test and search stations
     * are assumed to be the same, and they are matched based on their
     * respective index.
     * 
     * @param test_stations Test anenSta::Stations.
     * @param search_stations Search anenSta::Stations.
     * @param test_stations_index_in_search A size_t vector to store the
     * matching search indices for each test station.
     * 
     * @return An AnEn::errorType.
     */
    errorType find_nearest_station_match_(
            const anenSta::Stations & test_stations,
            const anenSta::Stations & search_stations,
            std::vector<size_t> & test_stations_index_in_search) const;
    
    /**
     * This function is the core function for computing a single similarity
     * metric value between two forecasts (usually one test forecast and one
     * historical search forecast).
     * 
     * @param test_forecasts The test forecasts.
     * @param search_forecasts The search forecasts.
     * @param sims A SimilarityMatrices.
     * @param sds A StandardDeviation.
     * @param weights A vector of weights for each parameter in the forecasts.
     * @param flts_window A matrix specifying the time window for each FLT.
     * @param circular_flags A vector of boolean for whether a parameter
     * is circular or not.
     * @param i_test_station The index of the test station in test forecasts.
     * @param i_test_time The index of the test time in test forecasts.
     * @param i_search_station The index of search station in search forecasts.
     * @param i_search_time The index of search time in search forecasts.
     * @param i_flt The index of FLT of both the search and test forecasts.
     * @param max_par_nan The maximum number of NAN allowed in parameters.
     * @param max_flt_nan The maximum number of NAN allowed in the FLT window.
     * @return A similarity metric value.
     */
    double compute_single_similarity_(
            const Forecasts_array & test_forecasts,
            const Forecasts_array & search_forecasts,
            const SimilarityMatrices & sims,
            const StandardDeviation & sds,
            const std::vector<double> & weights,
            const boost::numeric::ublas::matrix<size_t>& flts_window,
            const std::vector<bool> & circular_flags,
            size_t i_test_station,
            size_t i_test_time,
            size_t i_search_station,
            size_t i_search_time,
            size_t i_flt,
            double max_par_nan,
            double max_flt_nan) const;
};

#endif /* ANEN_H */
