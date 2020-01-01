/* 
 * File:   ReadTxt.h
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on January 1, 2020, 6:06 PM
 */

#ifndef READTXT_H
#define READTXT_H

#include <string>
#include <sstream>
#include <stdexcept>

#include "boost/numeric/ublas/matrix.hpp"

/**
 * Namespace ReadTxt provides functionality for txt file I/O.
 */
namespace ReadTxt {
    void checkPath(const std::string & file_path);
    
    /************************************************************************
     *                         Template Functions                           *
     ************************************************************************/
    template <typename T>
    void
    readMatrix(const std::string & file_path,
            boost::numeric::ublas::matrix<T> & mat);
}

// Template functions are defined in the following file
#include "ReadTxt.tpp"

#endif /* READTXT_H */

