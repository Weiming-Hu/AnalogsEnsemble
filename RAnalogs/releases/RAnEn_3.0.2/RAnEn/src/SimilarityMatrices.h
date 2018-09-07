/* 
 * File:   SimilarityMatrices.h
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on August 2, 2018, 8:32 PM
 */

#ifndef SIMILARITYMATRIX_H
#define SIMILARITYMATRIX_H

#include "Forecasts.h"
#include "boost/multi_array.hpp"

/**
 * \class SimilarityMatrices
 * 
 * \brief SimilarityMatrices class stores similarity metrics. Default dimensions
 *  of SimilarityMatrices are:
 *  [test stations][test times][test FLTs][Number of entries for comparison][3]
 * 
 * The second dimension stores in sequence similarity metric value, time
 * indicator, and station indication.
 */
class SimilarityMatrices : public boost::multi_array <double, 5> {
public:
    SimilarityMatrices(size_t max_entries = 10);
    SimilarityMatrices(const SimilarityMatrices& orig) = delete;
    SimilarityMatrices(const Forecasts & forecasts, size_t max_entries = 10);
    SimilarityMatrices(const size_t & num_stations, const size_t & num_times,
            const size_t & num_flts, const size_t & max_entries = 10);

    ~SimilarityMatrices();

    enum COL_TAG {
        VALUE = 0,
        STATION = 1,
        TIME = 2
    };

    struct matrixSort {
        COL_TAG order_tag;

        template < typename T, size_t dims > using sub_array =
        boost::detail::multi_array::sub_array < T, dims >;

        template < typename T, size_t dims >
        bool operator()(sub_array < T, dims > const &lhs, sub_array < T,
                dims > const &rhs) const {
            if (std::isnan(lhs[order_tag])) return false;
            if (std::isnan(rhs[order_tag])) return true;
            return (lhs[order_tag] < rhs[order_tag]);
        };

        template < typename T, size_t dims >
        bool operator()(boost::multi_array < T, dims > const &lhs,
                sub_array < T, dims > const &rhs) const {
            if (std::isnan(lhs[order_tag])) return false;
            if (std::isnan(rhs[order_tag])) return true;
            return (lhs[order_tag] < rhs[order_tag]);
        };

        template < typename T, size_t dims >
        bool operator()(sub_array < T, dims > const &lhs,
                boost::multi_array < T, dims > const &rhs) const {
            if (std::isnan(lhs[order_tag])) return false;
            if (std::isnan(rhs[order_tag])) return true;
            return (lhs[order_tag] < rhs[order_tag]);
        };

        template < typename T > bool operator()(T lhs, T rhs) const {
            return std::less < T > () (lhs, rhs);
        };
    };

    void resize();
    void resize(size_t dim0, size_t dim1, size_t dim2);

    /**
     * Set the target forecast.
     * 
     * @param target A Forecasts with one station, one time, one FLT,
     * and all parameters.
     */
    void setTargets(const Forecasts & targets);

    /**
     * Order all SimilarityMatrix using a specific column.
     * 
     * @return Whether the order succeeded.
     */
    bool sortRows(bool quick = true, size_t length = 0,
            COL_TAG col_tag = VALUE);

    /**
     * Get the target forecast values. Since forecasts only have parameters
     * for one station, one FLT, and one time. Values would a vector of
     * all the parameters.
     * 
     * @return A vector of double.
     */
    const Forecasts_array & getTargets() const;

    void setOrderTag(COL_TAG order_tag);
    void setMaxEntries(size_t max_entries);

    int getNumCols();
    size_t getMaxEntries();
    COL_TAG getOrderTag() const;

    void print(std::ostream &) const;
    void printSize(std::ostream &) const;
    friend std::ostream & operator<<(std::ostream &,
            const SimilarityMatrices&);

private:

    /**
     * The corresponding forecasts for each SimilarityMatrix.
     */
    Forecasts_array targets_;

    COL_TAG order_tag_ = VALUE;
    size_t max_entries_;

    const static int _NUM_COLS_ = 3;
};


#endif /* SIMILARITYMATRIX_H */

