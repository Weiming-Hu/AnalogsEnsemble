/* 
 * File:   AnEn.h
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on July 23, 2018, 10:56 PM
 */

#ifndef ANEN_H
#define ANEN_H

#include <string>

#include "AnEnIO.h"
#include "Analogs.h"
#include "Forecasts.h"
#include "Observations.h"
#include "SimilarityMatrices.h"
#include "StandardDeviation.h"
#include "boost/numeric/ublas/matrix.hpp"

#include <set>

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
    AnEn(std::string cache_similarity);
    AnEn(std::string cache_similarity, std::string output_folder);
    AnEn(std::string cache_similarity,
            std::string output_folder, int verbose);
    AnEn(const AnEn& orig) = delete;

    virtual ~AnEn();

    /**
     * Specifies the return error type of a function. Use AnEn::handleError
     * to handle the returned errorType.
     */
    enum errorType {
        SUCCESS = 0,
        WRONG_SHAPE = -10,
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
            boost::numeric::ublas::matrix<size_t> & mapping) const;

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
     * @return An AnEn::errorType.
     */
    errorType computeSimilarity(
            const Forecasts_array & search_forecasts,
            const StandardDeviation & sds,
            SimilarityMatrices & sims,
            const Observations_array& search_observations,
            boost::numeric::ublas::matrix<size_t> mapping) const;

    // writeMetric function to AnEnIO
    // readMetric function to AnEnIO

    errorType selectAnalogs(
            Analogs & analogs,
            SimilarityMatrices & sims,
            const Observations_array& search_observations,
            boost::numeric::ublas::matrix<size_t> mapping,
            size_t i_parameter, size_t num_members, bool quick = true) const;

    /**
     * Handles the errorType.
     * 
     * @param indicator An AnEn::errorType.
     */
    void handleError(const errorType & indicator) const;
    
    std::string getCacheSimilarity() const;
    std::string getOutputFolder() const;
    int getVerbose() const;

    void setCacheSimilarity(std::string cache_similarity);
    void setOutputFolder(std::string output_folder);
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

private:

    /**
     * Specifies the method to store similarity matrices. It can be:
     * - Memory
     * - NetCDF
     */
    std::string cache_similarity_ = "Memory";

    /**
     * Specifies the output folder for file output if "NetCDF"
     * is chosen for AnEn::cache_similarity_;
     */
    std::string output_folder_;

    /**
     * Specifies the verbose level.
     * - 0: Quiet.
     * - 1: Errors.
     * - 2: The above plus Warnings.
     * - 3: The above plus program progress information.
     * - 4: The above plus session information.
     */
    int verbose_ = 2;

};

#endif /* ANEN_H */

