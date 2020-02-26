/* 
 * File:   BmDim.h
 * Author: Weiming Hu <cervone@psu.edu>
 *
 * Created on January 8, 2020, 12:38 PM
 */

#ifndef BMDIM_H
#define BMDIM_H

/* 
 * Disable auto pointers. The standard library no longer supports std::auto_ptr.
 * It was deprecated in C++11 and is removed from C++14.
 */
#ifndef BOOST_NO_AUTO_PTR
#define BOOST_NO_AUTO_PTR
#endif

#include <boost/bimap/vector_of.hpp>
#include <boost/bimap/set_of.hpp>
#include <boost/bimap.hpp>

template <class T>
using BmType = boost::bimap<boost::bimaps::vector_of<std::size_t>, T,
        boost::bimaps::left_based>;

#endif /* BMDIM_H */

