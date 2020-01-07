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

#include "boost/multi_array.hpp"

/**
 * \class SimilarityMatrices
 * 
 * \brief SimilarityMatrices class stores similarity metrics. Default dimensions
 *  of SimilarityMatrices are:
 *  [test stations][test times][test FLTs][Number of entries for comparison][3]
 * 
 * The last dimension stores in sequence similarity metric value, station
 * indicator, and time indication.
 * 
 * SimilarityMatrices data order is row-major. This is not implemented as column-
 * major because it needs to have the ability to be ordered by rows.
 */
class SimilarityMatrices : public boost::multi_array <double, 5> {
public:
    SimilarityMatrices(size_t max_entries = 10);
    SimilarityMatrices(const SimilarityMatrices& orig) = delete;
    SimilarityMatrices(const size_t & num_stations, const size_t & num_times,
            const size_t & num_flts, const size_t & max_entries = 10);

    ~SimilarityMatrices();

    enum COL_TAG {
        VALUE = 0, STATION = 1, TIME = 2
    };

    void resize(size_t dim0, size_t dim1, size_t dim2);

    bool sortRows(bool quick = true, size_t length = 0,
            COL_TAG col_tag = VALUE);

    void setOrderTag(COL_TAG order_tag);
    void setMaxEntries(size_t max_entries);

    int getNumCols();
    size_t getMaxEntries();
    COL_TAG getOrderTag() const;

    void print(std::ostream &) const;
    void printShape(std::ostream &) const;
    
    friend std::ostream & operator<<(std::ostream &,
            const SimilarityMatrices&);
    
private:
    COL_TAG order_tag_;
    size_t max_entries_;
};

#endif /* SIMILARITYMATRIX_H */

