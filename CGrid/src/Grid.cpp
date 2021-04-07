/*
 * File:   Grid.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on April 3, 2021, 11:15 AM
 */

#include <cmath>
#include <sstream>
#include <boost/numeric/conversion/cast.hpp>

#include "Txt.h"
#include "Grid.h"

using namespace std;

/********
 * Grid *
*********/

Grid::Grid() {
    nan_count_ = 0;
}

Grid::Grid(const Grid & orig) {
    *this = orig;
}

Grid::Grid(const string & file) {
    setup(file);
}

Grid::~Grid() {
}

void
Grid::setup(const string & file) {

    // Read matrix from the file
    Txt::readMatrix(file, grid_);

    // Set up the mapping from station keys to the row/column numbers
    row_cols_.clear();
    nan_count_ = 0;

    size_t num_rows = nrows();
    size_t num_cols = ncols();

    for (size_t r = 0; r < num_rows; ++r) {
        for (size_t c = 0; c < num_cols; ++c) {
            double key = grid_(r, c);

            if (std::isnan(key)) {
                nan_count_++;

            } else {
                double integral, fractional;
                fractional = modf(key, &integral);

                if (fractional != 0) {
                    throw runtime_error("Station keys must be non-negative integer!");
                }

                row_cols_[boost::numeric_cast<size_t>(key)] = {r, c};
            }
        }
    }

    if (row_cols_.size() + nan_count_ != num_rows * num_cols) {
        stringstream ss;
        ss << "Duplicated station keys found. Expect " << num_rows << "x" << num_cols
            << ", but got " << row_cols_.size() << " and " << nan_count_ << " nan";
        throw runtime_error(ss.str());
    }

    return;
}

void
Grid::getRectangle(size_t station_key, Matrix & mask,
        size_t width, size_t height, bool same_padding) const {

    if (width == 0 || height == 0) throw runtime_error("Invalid width or height");
    if (remainder(width - 1, 2) != 0) throw runtime_error("Width needs to be an odd number");
    if (remainder(height - 1, 2) != 0) throw runtime_error("Height needs to be an odd number");

    // Initialization
    mask.resize(height, width);

    // Get the location of the center grid
    RowCol row_col = (*this)[station_key];
    long center_row = boost::numeric_cast<long>(row_col.first);
    long center_col = boost::numeric_cast<long>(row_col.second);

    // Get the spatial extent of the mask
    long left = center_col - (width - 1) / 2;
    long top = center_row - (height - 1) / 2;

    size_t num_rows = nrows();
    size_t num_cols = ncols();

    for (size_t r = 0; r < mask.size1(); ++r) {
        long grid_r = top + r;

        if (same_padding) {
            if (grid_r < 0) grid_r = 0;
            else if (grid_r >= num_rows) grid_r = num_rows - 1;
        }

        if (grid_r < 0 || grid_r >= num_rows) {
            for (size_t c = 0; c < mask.size2(); ++c) {
                mask(r, c) = NAN;
            }

        } else {
            for (size_t c = 0; c < mask.size2(); ++c) {
                long grid_c = left + c;

                if (same_padding) {
                    if (grid_c < 0) grid_c = 0;
                    else if (grid_c >= num_cols) grid_c = num_cols - 1;
                }

                if (grid_c < 0 || grid_c >= num_cols) {
                    mask(r, c) = NAN;
                } else {
                    double v = (*this)(grid_r, grid_c);

                    if (same_padding && std::isnan(v)) {
                        throw runtime_error("NAN is not allowed when using same padding");
                    } else {
                        mask(r, c) = v;
                    }
                }
            }
        }
    }

    return;
}

size_t
Grid::nrows() const {
    return grid_.size1();
}

size_t
Grid::ncols() const {
    return grid_.size2();
}

size_t
Grid::nkeys() const {
    return row_cols_.size();
}

string
Grid::summary() const {
    stringstream ss;
    ss << nrows() << " rows, " << ncols() << " columns, " << nkeys()
        << " station keys, and " << nan_count_ << " unassigned grids (nan)";
    return ss.str();
}

string
Grid::detail() const {
    stringstream ss;
    ss << grid_;
    return ss.str();
}

ostream &
operator<<(ostream & os, const Grid & obj) {
    return os << obj.grid_;
}

Grid &
Grid::operator=(const Grid & rhs) {
    if (this != &rhs) {
        nan_count_ = rhs.nan_count_;
        grid_ = rhs.grid_;
        row_cols_ = rhs.row_cols_;
    }
    return *this;
}

RowCol
Grid::operator[](size_t station_key) const {
    return row_cols_.at(station_key);
}

double
Grid::operator()(size_t row, size_t col) const {
    return grid_(row, col);
}

