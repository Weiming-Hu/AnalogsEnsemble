/* 
 * File:   Functions.tpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on January 7, 2020, 10:45 AM
 */

#include "Parameters.h"
#include "Stations.h"
#include "Times.h"

template <typename T>
std::string
Functions::format(const std::vector<T> & vec, const std::string & delim) {
    
    using namespace std;
    
    stringstream msg;
    size_t len = vec.size();
    for (size_t i = 0; i < len; ++i) {
        msg << vec[i];
        if (i != len - 1) msg << delim;
    }
    
    return (msg.str());
}

template <typename T>
std::string
Functions::format(const T* ptr, size_t len, const std::string & delim) {
    
    using namespace std;
    
    stringstream msg;
    for (size_t i = 0; i < len; ++i) {
        msg << ptr[i];
        if (i != len - 1) msg << delim;
    }
    
    return (msg.str());
}

template <class T>
void
Functions::toIndex(std::vector<std::size_t> & index,
        const T & query, const T & pool) {
    
    // Check for accepted instantiation
    static const bool accepted_types =
            (boost::is_base_of<Parameters, T>::value) ||
            (boost::is_base_of<Stations, T>::value) ||
            (boost::is_base_of<Times, T>::value);
    BOOST_STATIC_ASSERT(accepted_types);

    // Resize the index vector
    index.resize(query.size());

    // Calculate indices
    for (std::size_t i = 0; i < index.size(); ++i) {
        index[i] = pool.getIndex(query.left[i].second);
    }

    return;
}
