/* 
 * File:   SimilarityMatrices.h
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on August 2, 2018, 8:32 PM
 */

#ifndef SIMILARITYMATRIX_H
#define SIMILARITYMATRIX_H

#ifndef BOOST_NO_AUTO_PTR
#define BOOST_NO_AUTO_PTR
#endif

#include "Forecasts.h"
#include "boost/multi_array.hpp"

/**
 * \class SimilarityMatrices
 * 
 * \brief SimilarityMatrices class stores similarity metrics. Default dimensions
 *  of SimilarityMatrices are:
 *  [3][Number of entries for comparison][test FLTs][test times][test stations]
 * 
 * The second dimension stores in sequence similarity metric value, station
 * indicator, and time indication.
 * 
 * SimilarityMatrices data order is row-major.
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

    void resize(size_t dim0, size_t dim1, size_t dim2);
    
    /**
     * Check whether the similarity metrics are computed using search space extension.
     * @return A boolean for whether search space extension is used.
     */
    bool checkSearchSpaceExtension() const;

    /**
     * Order all SimilarityMatrix using a specific column.
     * 
     * @return Whether the order succeeded.
     */
    bool sortRows(bool quick = true, size_t length = 0,
            COL_TAG col_tag = VALUE);

    void setOrderTag(COL_TAG order_tag);
    void setMaxEntries(size_t max_entries);

    int getNumCols();
    size_t getMaxEntries();
    COL_TAG getOrderTag() const;

    void print(std::ostream &) const;
    void printSize(std::ostream &) const;
    friend std::ostream & operator<<(std::ostream &,
            const SimilarityMatrices&);
    
    const static int _NUM_COLS = 3;

private:

    COL_TAG order_tag_ = VALUE;
    size_t max_entries_;
};


#endif /* SIMILARITYMATRIX_H */

