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

template <class T>
void
Functions::guess_arguments(const std::vector< std::basic_string<T> >& unregistered_keys,
        const std::vector<std::string> & available_options,
        std::ostream & os) {
    
    using namespace std;
    
    vector<int> distances;
    os << "Found unknown arguments: " << endl;
    for (auto arg : unregistered_keys) {

        distances.clear();
        os << arg.c_str();

        // compute the Leveshtein distances
        for (auto arg_available : available_options) {

            // the weights are suggested by Github
            distances.push_back(levenshtein(arg, arg_available, 0, 2, 1, 3));
        }

        // find the most similar ones and print them out
        os << ": did you mean ";
        int min = *min_element(distances.begin(), distances.end());
        auto iter = distances.begin();
        while (iter != distances.end()) {
            if (*iter == min) {
                os << available_options.at(iter - distances.begin()) << " ";
            }
            iter++;
        }
        os << endl;
    }
    os << "Please refer to the full list of parameters using -h or --help" << endl;
    
    return;
}