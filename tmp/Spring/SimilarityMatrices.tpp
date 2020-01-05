/* 
 * File:   SimilarityMatrices.tpp
 * Author: guido
 *
 * Created on January 5, 2020, 10:22 AM
 */

// Ref:
// http://coliru.stacked-crooked.com/a/bfbb5a98995cc2a4
// https://stackoverflow.com/questions/36117142/sorting-boosts-multi-array-using-sort-function-and-a-recursive-comparator
//

template < typename T, size_t dims >
bool
SimilarityMatrices::operator()(
        SimilarityMatrices::sub_array < T, dims > const &lhs,
        SimilarityMatrices::sub_array < T,
        dims > const &rhs) const {
    if (std::isnan(lhs[order_tag_])) return false;
    if (std::isnan(rhs[order_tag_])) return true;
    return (lhs[order_tag_] < rhs[order_tag_]);
}

template < typename T, size_t dims >
bool
SimilarityMatrices::operator()(boost::multi_array < T, dims > const &lhs,
        SimilarityMatrices::sub_array < T, dims > const &rhs) const {
    if (std::isnan(lhs[order_tag_])) return false;
    if (std::isnan(rhs[order_tag_])) return true;
    return (lhs[order_tag_] < rhs[order_tag_]);
}

template < typename T, size_t dims >
bool
SimilarityMatrices::operator()(SimilarityMatrices::sub_array < T, dims > const &lhs,
        boost::multi_array < T, dims > const &rhs) const {
    if (std::isnan(lhs[order_tag_])) return false;
    if (std::isnan(rhs[order_tag_])) return true;
    return (lhs[order_tag_] < rhs[order_tag_]);
}

template < typename T >
bool
SimilarityMatrices::operator()(T lhs, T rhs) const {
    return std::less < T > () (lhs, rhs);
}

template < typename T, size_t dims >
void
SimilarityMatrices::swap(sub_array < T, dims > lhs, sub_array < T, dims > rhs) {
    using std::swap;
    for (auto ai = lhs.begin(), bi = rhs.begin();
            ai != lhs.end() && bi != rhs.end(); ++ai, ++bi) {
        swap(*ai, *bi);
    }
}
