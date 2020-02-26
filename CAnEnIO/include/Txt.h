/* 
 * File:   ReadTxt.h
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on January 1, 2020, 6:06 PM
 */

#ifndef TXT_H
#define TXT_H

// This include file is needed because there is a bug in older versions of the ulas library
//
// Reference: https://stackoverflow.com/questions/44534516/error-make-array-is-not-a-member-of-boostserialization
//
// #include <boost/serialization/array_wrapper.hpp>
//
// And then it seems like for some Boost versions, even the above file is missing, so 
// I'm using the following files as a workaround.
//
// Reference: https://github.com/bitzy/DeepSort/issues/5#issuecomment-502518256
//
#include <boost/serialization/array.hpp>
#include <boost/serialization/wrapper.hpp>
#include <boost/numeric/ublas/matrix.hpp>

#include <string>
#include <sstream>
#include <stdexcept>

/**
 * Namespace ReadTxt provides functionality for txt file I/O.
 */
namespace Txt {

    enum class Mode {
        Read, Write
    };

    void checkPath(const std::string & file_path, Mode mode,
            const std::string & extension = ".txt");

    /************************************************************************
     *                         Template Functions                           *
     ************************************************************************/

    template <typename T>
    void
    readMatrix(const std::string & file_path,
            boost::numeric::ublas::matrix<T> & mat);

    template <typename T>
    void
    writeMatrix(const std::string & file_path,
            const boost::numeric::ublas::matrix<T> & mat);
}

// Template functions are defined in the following file
#include "Txt.tpp"

#endif /* TXT_H */
