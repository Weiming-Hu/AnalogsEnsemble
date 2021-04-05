/*
 * File:   Grid.h
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on April 3, 2021, 10:54 AM
 */

#ifndef GRID_H

#define GRID_H
#include <utility>
#include <unordered_map>

#ifdef _BOOST_VERSION_1_58_
#include <boost/serialization/array.hpp>
#include <boost/serialization/wrapper.hpp>
#else
#include <boost/serialization/array_wrapper.hpp>
#endif

#include <boost/numeric/ublas/matrix.hpp>

#include "Txt.h"
#include "Forecasts.h"

// The data structure to store grid locations
//
//          IMPORTANT NOTE
//
// Count columns from left to right
// Count rows from top to bottom
//
using Matrix = boost::numeric::ublas::matrix<double>;

// The data sctructure to store a pair of the row and the column numbers
using RowCol = std::pair<std::size_t, std::size_t>;

// The data structure to map from the station index to the row/column numbers
using RowCols = std::unordered_map<std::size_t, RowCol>;

class Grid {
public:
    Grid();
    Grid(const Grid & orig);
    Grid(const std::string & file);
    virtual ~Grid();

    virtual void setup(const std::string & file);

    virtual void getRectangle(std::size_t station_key, Matrix & mask,
            std::size_t width, std::size_t height, bool same_padding = false) const;

    virtual std::size_t nrows() const;
    virtual std::size_t ncols() const;
    virtual std::size_t nkeys() const;
    virtual std::string summary() const;

    friend std::ostream & operator<<(std::ostream &, const Grid &);

    Grid & operator=(const Grid & rhs);
    RowCol operator[](std::size_t station_key) const;
    double operator()(std::size_t row, std::size_t col) const;
    
protected:
    size_t nan_count_;
    Matrix grid_;
    RowCols row_cols_;
};

namespace GridQuery {
    void rectangle(const Matrix & mask, std::size_t time_index,
            long lead_time_index, long lead_time_radius,
            const Forecasts & forecasts, Array4D & arr_to);
}

#endif /* GRID_H */
