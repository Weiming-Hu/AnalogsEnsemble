/* 
 * File:   Analogs.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 * 
 * Created on August 5, 2018, 5:00 PM
 */

#include "Analogs.h"

using namespace std;

const static int _NUM_COLS = 3;

Analogs::Analogs() :
boost::multi_array<double, 5>(
boost::extents[0][0][0][0][_NUM_COLS],
boost::fortran_storage_order()) {
}

Analogs::Analogs(size_t num_stations, size_t num_times,
        size_t num_flts, size_t num_members) :
boost::multi_array<double, 5>(
boost::extents[num_stations][num_times][num_flts][num_members][_NUM_COLS],
boost::fortran_storage_order()) {
    Analogs::extent_gen extents;
    search_stations.resize(num_stations);
}

Analogs::Analogs(const Forecasts& forecasts, size_t num_members) :
boost::multi_array<double, 5>(
boost::extents[forecasts.getStations().size()][forecasts.getTimes().size()]
[forecasts.getFLTs().size()][num_members][_NUM_COLS],
boost::fortran_storage_order()) {

    search_stations.resize(forecasts.getStations().size());
}

Analogs::~Analogs() {
}

void
Analogs::print(std::ostream & os) const {
    size_t dim1 = shape()[0];
    size_t dim2 = shape()[1];
    size_t dim3 = shape()[2];
    size_t dim4 = shape()[3];
    size_t dim5 = shape()[4];

    for (size_t i_dim1 = 0; i_dim1 < dim1; i_dim1++) {
        for (size_t i_dim2 = 0; i_dim2 < dim2; i_dim2++) {
            for (size_t i_dim3 = 0; i_dim3 < dim3; i_dim3++) {

                os << "Analogs [" << i_dim1 << "][" << i_dim2 << "]["
                        << i_dim3 << "][][]" << endl;
                os << "\t\t [, 0] \t\t [, 1] \t\t [, 2]" << endl;
                for (size_t i_dim4 = 0; i_dim4 < dim4; i_dim4++) {

                    os << "[" << i_dim4 << ", ] ";
                    for (size_t i_dim5 = 0; i_dim5 < dim5; i_dim5++) {
                        os << " \t\t " << (*this)[i_dim1][i_dim2]
                                [i_dim3][i_dim4][i_dim5];
                    }
                    os << endl;
                }
            }
        }
    }
}

std::ostream &
operator<<(std::ostream & os, const Analogs & obj) {
    obj.print(os);
    return os;
}
