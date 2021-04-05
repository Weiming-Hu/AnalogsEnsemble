/*
 * File:   GridPy.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on April 5, 2021, 1:04 AM
 */

#include <cmath>
#include <sstream>
#include <iostream>
#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

#include "Grid.h"

namespace py = pybind11;
using namespace pybind11::literals;

py::module_ np = py::module_::import("numpy");

PYBIND11_MODULE(AnEnGrid, m) {
    py::class_<Grid>(m, "AnEnGrid")
        .def(py::init<>())
        .def(py::init<const std::string &>())
        .def("nrows", &Grid::nrows, "Get the number of rows")
        .def("ncols", &Grid::ncols, "Get the number of columns")
        .def("nkeys", &Grid::nkeys, "Get the number of unique keys")
        .def("summary", &Grid::summary, "Get a summary of the grid")
        .def("detail", &Grid::detail, "Get the grid layout")
        .def("__str__", &Grid::detail)
        .def("loc", &Grid::operator(), "Get the station key at row (top-down) and column (left-right)")
        .def("lookup", &Grid::operator[], "Get the row and column of a station key")
        .def("setup", &Grid::setup, "Set up the grid layout from a text file")
        .def("getRectangle", [](const Grid &self, std::size_t station_key,
                    std::size_t width, std::size_t height, bool same_padding) {

                // Call the C++ routine to generate a mask
                Matrix mask;
                self.getRectangle(station_key, mask, width, height, same_padding);

                // Convert Boost Matrix to Numpy
                py::array_t<double> arr = np.attr("full")(
                        std::vector<ptrdiff_t>{mask.size1(), mask.size2()},
                        NAN, "dtype"_a = "double");

                auto data = arr.mutable_unchecked<2>();
                for (size_t i = 0; i < mask.size1(); ++i)
                    for (size_t j = 0; j < mask.size2(); ++j)
                        data(i, j) = mask(i, j);

                return arr;

                }, "Get a rectangular mask");
}

