/* 
 * File:   SimilarityMatrices.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 * 
 * Created on August 2, 2018, 8:32 PM
 */

#include "SimilarityMatrices.h"
#include "Functions.h"
#include "colorTexts.h"

#include <stdexcept>
#include <algorithm>
#include <exception>
#include <iostream>
#include <sstream>

#if defined(_OPENMP)
#include <omp.h>
#endif


// The struct is created for sorting a 2-dimensional multi_array_view
// (subarray) of boost based on a column.
//
// Ref: http://coliru.stacked-crooked.com/a/bfbb5a98995cc2a4
//

struct colSort {
    SimilarityMatrices::COL_TAG order_tag_ = SimilarityMatrices::COL_TAG::VALUE;

    template <typename T, size_t dims> using sub_array =
    boost::detail::multi_array::sub_array<T, dims>;

    template <typename T, size_t dims>
    bool
    operator()(
            sub_array<T, dims> const &lhs,
            sub_array<T, dims> const &rhs) const {
        if (std::isnan(lhs[order_tag_])) return false;
        if (std::isnan(rhs[order_tag_])) return true;
        return (lhs[order_tag_] < rhs[order_tag_]);
    }

    template <typename T, size_t dims>
    bool
    operator()(boost::multi_array<T, dims> const &lhs,
            sub_array<T, dims> const &rhs) const {
        if (std::isnan(lhs[order_tag_])) return false;
        if (std::isnan(rhs[order_tag_])) return true;
        return (lhs[order_tag_] < rhs[order_tag_]);
    }

    template <typename T, size_t dims>
    bool
    operator()(sub_array<T, dims> const &lhs,
            boost::multi_array<T, dims> const &rhs) const {
        if (std::isnan(lhs[order_tag_])) return false;
        if (std::isnan(rhs[order_tag_])) return true;
        return (lhs[order_tag_] < rhs[order_tag_]);
    }

    template <typename T>
    bool
    operator()(T lhs, T rhs) const {
        return std::less<T>()(lhs, rhs);
    }
};

namespace boost {
    namespace detail {
        namespace multi_array {

            template <typename T, size_t dims>
            static void
            swap(sub_array<T, dims> lhs,
                    sub_array<T, dims> rhs) {
                using std::swap;
                for (auto ai = lhs.begin(), bi = rhs.begin();
                        ai != lhs.end() && bi != rhs.end(); ++ai, ++bi) {
                    swap(*ai, *bi);
                }
            }
        }
    }
}

using namespace std;
using boost::detail::multi_array::swap;
const static int _NUM_COLS = 3;

SimilarityMatrices::SimilarityMatrices(size_t max_entries) :
boost::multi_array<double, 5>(boost::extents[0][0][0][max_entries][_NUM_COLS]),
order_tag_(VALUE), max_entries_(max_entries) {
}

SimilarityMatrices::SimilarityMatrices(const size_t& num_stations,
        const size_t& num_times, const size_t& num_flts,
        const size_t& max_entries) :
boost::multi_array<double, 5>(boost::extents[num_stations][num_times]
[num_flts][max_entries][_NUM_COLS]),
order_tag_(VALUE), max_entries_(max_entries) {
}

SimilarityMatrices::~SimilarityMatrices() {
}

void
SimilarityMatrices::resize(size_t dim0, size_t dim1, size_t dim2) {
    try {
        boost::multi_array < double, 5 >::resize(
                boost::extents[dim0][dim1][dim2][max_entries_][_NUM_COLS]);
    } catch (bad_alloc & e) {
        ostringstream msg;
        msg << BOLDRED << "Insufficient memory for SimilarityMatrices["
                << dim0 << "][" << dim1 << "][" << dim2 << "]["
                << max_entries_ << "][" << _NUM_COLS << "]!" << RESET;
        throw runtime_error(msg.str());
    }
}

bool
SimilarityMatrices::sortRows(bool quick, size_t length, COL_TAG col_tag) {

    order_tag_ = col_tag;

    // Define variables for perfectly nested parallel loops with collapse
    auto limit_i = this->shape()[0];
    auto limit_j = this->shape()[1];
    auto limit_k = this->shape()[2];

    if (length == 0) length = this->shape()[3];

#if defined(_OPENMP)
#pragma omp parallel for default(none) schedule(static) \
collapse(3) shared(quick, length, limit_i, limit_j, limit_k)
#endif
    for (size_t i = 0; i < limit_i; i++) {
        for (size_t j = 0; j < limit_j; j++) {
            for (size_t k = 0; k < limit_k; k++) {
                if (quick) {
                    nth_element((*this)[i][j][k].begin(),
                            (*this)[i][j][k].begin() + length,
                            (*this)[i][j][k].end(), colSort{});
                } else {
                    partial_sort((*this)[i][j][k].begin(),
                            (*this)[i][j][k].begin() + length,
                            (*this)[i][j][k].end(), colSort{});
                }
            }
        }
    }

    return true;
}

void
SimilarityMatrices::setOrderTag(COL_TAG order_tag) {
    order_tag_ = order_tag;
}

void
SimilarityMatrices::setMaxEntries(size_t max_entries) {
    max_entries_ = max_entries;
}

int
SimilarityMatrices::getNumCols() {
    return _NUM_COLS;
}

size_t
SimilarityMatrices::getMaxEntries() {
    return max_entries_;
}

SimilarityMatrices::COL_TAG
SimilarityMatrices::getOrderTag() const {
    return order_tag_;
}

void
SimilarityMatrices::printShape(std::ostream & os) const {
    os << "SimilarityMatrices Array Shape = ";
    for (size_t i = 0; i < 5; i++) os << "[" << shape()[i] << "]";
    os << endl;
    return;
}

ostream & operator<<(ostream & os, const SimilarityMatrices& obj) {
    obj.printShape(os);
    Functions::print(os, obj);
    return os;
}
