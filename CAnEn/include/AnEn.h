/* 
 * File:   AnEn.h
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on July 23, 2018, 10:56 PM
 */

#ifndef ANEN_H
#define ANEN_H

#include "Functions.h"

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

    using TimeMapMatrix = Functions::TimeMapMatrix;

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
     * - OneToOne: Each test station is assigned with the closest station in the
     * search stations;
     * - OneToMany: Each test station is assigned with a set of nearby stations
     * in the search stations (Search space extension);
     * 
     */
    enum simMethod {
        /// 1
        OneToOne = 1, 
        /// 2
        OneToMany = 2
    };
    
    /**
     * Computes the search stations of each test stations. Search stations are 
     * represented using the their index.
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
     * @return An errorType;
     */
    errorType computeSearchStations(
            const anenSta::Stations & test_stations,
            const anenSta::Stations & search_stations,
            SearchStationMatrix & i_search_stations,
            size_t max_num_search_stations = 1,
            double distance = 0, size_t num_nearest_stations = 0) const;

    /**
     * Generates the search times and the corresponding index with an 
     * operational mode.
     * 
     * @param test_times The test anenTime::Times;
     * @param search_times The complete search anenTime::Times;
     * @param search_times_operational The search anenTime::Times for operational mode.
     * @param i_search_times_operational The index for search anenTime::Times for
     * operational mode.
     * @param max_flt The maximum FLT.
     * @return An errorType.
     */
    errorType generateOperationalSearchTimes(
            const anenTime::Times & test_times,
            const anenTime::Times & search_times,
            std::vector< anenTime::Times > & search_times_operational,
            std::vector< std::vector<size_t> > & i_search_times_operational,
            double max_flt) const;

    /**
     * Computes the similarity matrices.
     * 
     * If a particular value is NAN, you can inspect the reason for this value
     * being NAN by looking at the time index column. The value can
     * be any one from the following list:
     *     - -1: Cannot find the corresponding observation time. Mapping entry is NAN.
     *     - -2: Search forecast excluded because it overlaps with the current test
     *     foreacst.
     *     - NAN: Noun of the above. This can be caused by a NAN search station or 
     *     too many NAN parameters for the particular test or search forecasts.
     *
     * @param test_forecasts Forecasts to test.
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
     * @param operational Whether to use operational search.
     * 
     * @return An errorType.
     */
    errorType computeSimilarity(
            const Forecasts_array & test_forecasts,
            const Forecasts_array & search_forecasts,
            const StandardDeviation & sds,
            SimilarityMatrices & sims,
            const Observations_array& search_observations,
            const TimeMapMatrix & mapping,
            const SearchStationMatrix & i_search_stations,
            size_t i_observation_parameter = 0,
            bool extend_observations = false,
            double max_par_nan = 0, double max_flt_nan = 0,
            anenTime::Times test_times = {},
            anenTime::Times search_times = {},
            bool operational = false) const;

    /**
     * Select analogs based on the similarity matrices.
     *
     * If a particular value is NAN, you can inspect the reason for this value
     * being NAN by looking at the time index column. The value can
     * be any one from the following list:
     *     - -1: Cannot find the corresponding observation time. Mapping entry is NAN.
     *     - -2: Similarity is NAN.
     *     - -3: Number of members is larger than number of similarity entries.
     *     - NAN: Unknown reason.
     *
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
     * @return An errorType.
     */
    errorType selectAnalogs(
            Analogs & analogs,
            SimilarityMatrices & sims,
            const anenSta::Stations & test_stations,
            const Observations_array& search_observations,
            const TimeMapMatrix & mapping,
            size_t i_parameter, size_t num_members,
            bool quick = true, bool extend_observations = false) const;

    int getVerbose() const;
    simMethod getMethod() const;

    void setMethod(simMethod method);
    void setVerbose(int verbose);

    /**
     * The default fill value for analogs.
     */
    static const double _FILL_VALUE;
    
    /**
     * The upper limit for running program serially. If the length is larger
     * than the limit, it will be parallelized.
     */
    static const size_t _SERIAL_LENGTH_LIMIT;

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

    /*
     * This variable specifies how similarity matrices are computed across stations.
     */
    simMethod method_ = OneToOne;

    /**
     * Check input.
     * @param test_forecasts Test forecasts.
     * @param search_forecasts Search forecasts.
     * @param sds StandardDeviation.
     * @param search_observations Search observations.
     * @param mapping Time and FLT mapping matrix.
     * @param i_observation_parameter The index of observation parameter used.
     * @return An errorType.
     */
    errorType check_input_(
            const Forecasts_array& test_forecasts,
            const Forecasts_array& search_forecasts,
            const StandardDeviation& sds,
            const Observations_array& search_observations,
            const TimeMapMatrix & mapping,
            const AnEn::SearchStationMatrix & i_search_stations,
            size_t i_observation_parameter,
            double max_par_nan, double max_flt_nan,
            bool operational) const;

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
     * @return An errorType.
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
