/* 
 * File:   Analogs.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 * 
 * Created on August 5, 2018, 5:00 PM
 */

#include "Analogs.h"

using namespace std;

Analogs::Analogs() :
boost::multi_array<double, 5>(
boost::extents[0][0][0][0][3],
boost::fortran_storage_order()) {
}

Analogs::Analogs(size_t num_stations, size_t num_times,
        size_t num_flts, size_t num_members) :
boost::multi_array<double, 5>(
boost::extents[num_stations][num_times][num_flts][num_members][3],
boost::fortran_storage_order()) {
    Analogs::extent_gen extents;
    search_stations.resize(num_stations);
}

Analogs::Analogs(const Forecasts& forecasts, size_t num_members) :
boost::multi_array<double, 5>(
boost::extents[forecasts.getStationsSize()][forecasts.getTimesSize()]
[forecasts.getFLTsSize()][num_members][3],
boost::fortran_storage_order()) {
    stations_ = forecasts.getStations();
    times_ = forecasts.getTimes();
    flts_ = forecasts.getFLTs();

    search_stations.resize(forecasts.getStationsSize());
}

Analogs::~Analogs() {
}

anenTime::FLTs
Analogs::getFLTs() const {
    return flts_;
}

anenSta::Stations
Analogs::getStations() const {
    return stations_;
}

anenTime::Times
Analogs::getTimes() const {
    return times_;
}

void
Analogs::setFLTs(anenTime::FLTs flts) {
    flts_ = flts;
}

void
Analogs::setStations(anenSta::Stations stations) {
    stations_ = stations;
}

void
Analogs::setTimes(anenTime::Times times) {
    times_ = times;
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
