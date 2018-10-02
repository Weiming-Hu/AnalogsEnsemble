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
    
    using TimeMapMatrix = boost::numeric::ublas::matrix<size_t>;
    using SearchStationMatrix = boost::numeric::ublas::matrix<double>;

    /**
     * Specifies the method that AnEn::computeSimilarity function uses:
     * - ONE_TO_ONE Stations of search and test forecasts are aligned; each
     * test station only search its own station;
     * - ONE_TO_MANY Stations of search are extended; search extension is 
     * defined by a search station list.
     * - ONE_TO_ALL Each of test station searches all search stations.
     * 
     * The ONE, MANY, ALL are concepts for stations, like one station,
     * many stations, and all stations.
     */
    enum simMethod {
        /// 1
        ONE_TO_ONE = 1, 
        /// 2
        ONE_TO_MANY = 2, 
        /// 3
        ONE_TO_ALL = 3 
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
     * @return An AnEn::errorType.
     */
    errorType computeObservationsTimeIndices(
            const anenTime::Times & times_forecasts,
            const anenTime::Times & flts_forecasts,
            const anenTime::Times & times_observations,
            TimeMapMatrix & mapping) const;

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
            size_t max_num_search_stations,
            double distance = 0, size_t num_nearest_stations = 0,
            bool return_index = true);

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
     * 
     * @return An AnEn::errorType.
     */
    errorType computeSimilarity(
            const Forecasts_array & search_forecasts,
            const StandardDeviation & sds,
            SimilarityMatrices & sims,
            const Observations_array& search_observations,
            const boost::numeric::ublas::matrix<size_t> & mapping,
            boost::numeric::ublas::matrix<double> & i_search_stations,
            size_t i_observation_parameter = 0) const;
    
    errorType computeSimilarity(
            const Forecasts_array & search_forecasts,
            const StandardDeviation & sds,
            SimilarityMatrices & sims,
            const Observations_array& search_observations,
            const boost::numeric::ublas::matrix<size_t> & mapping,
            size_t i_observation_parameter = 0) const;

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
     * @param preserve_real_time Whether to replace the observation time index
     * with the actual observation time.
     * @return An AnEn::errorType.
     */
    errorType selectAnalogs(
            Analogs & analogs,
            SimilarityMatrices & sims,
            const Observations_array& search_observations,
            boost::numeric::ublas::matrix<size_t> mapping,
            size_t i_parameter, size_t num_members,
            bool quick = true, bool preserve_real_time = false) const;

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
     */
    double mean(const std::vector<double> & values) const;

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
     * This variable specifies the way similarity matrices are computed.
     * It is used by function AnEn::computeSimilarity. The ONE, MANY, ALL are
     * concepts for stations, like one station, many stations, and all stations.
     */
    simMethod method_ = ONE_TO_ALL;

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
            boost::numeric::ublas::matrix<size_t> mapping,
            size_t i_observation_parameter) const;
};

#endif /* ANEN_H */
