/* 
 * File:   AnEn.h
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on July 23, 2018, 10:56 PM
 */

#ifndef ANEN_H
#define ANEN_H

#include "AnEnIO.h"
#include "string"
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
    };

    errorType computeStandardDeviation(
            const Forecasts_array & forecasts,
            StandardDeviation & sds);

    errorType computeSearchWindows(
            boost::numeric::ublas::matrix<size_t> & matrix,
            size_t num_flts, size_t window_half_size) const;

    errorType computeSimilarity(
            const Forecasts_array & search_forecasts,
            const StandardDeviation & sds,
            SimilarityMatrices & sims,
            const Observations_array& search_observations) const;

    // writeMetric function to AnEnIO
    // readMetric function to AnEnIO

    errorType selectAnalogs(const SimilarityMatrix & matrix,
            std::set<size_t> & selected_index);

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

    /**
     * Computes the standard deviation for linear numbers.
     * 
     * @param values A vector of values.
     */
    double _sdLinear(const std::vector<double> & values) const;

    /**
     * Computes the standard deviation for circular numbers.
     * 
     * @param values A vector of values.
     */
    double _sdCircular(const std::vector<double> & values) const;

    /**
     * Computes the mean of a vector.
     * @param values A vector of values.
     */
    double _mean(const std::vector<double> & values) const;

    /**
     * Computes the variance of a vector.
     * @param values A vector of values.
     */
    double _variance(const std::vector<double> & values) const;

    double _diffCircular(double i, double j) const;

};

#endif /* ANEN_H */

