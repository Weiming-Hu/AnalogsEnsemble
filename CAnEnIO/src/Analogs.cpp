/* 
 * File:   Analogs.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 * 
 * Created on August 5, 2018, 5:00 PM
 */

#include "Analogs.h"
#include "Functions.h"

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

Analogs::~Analogs() {
}

void
Analogs::printShape(std::ostream & os) const {
    os << "Analogs Shape = ";
    for (size_t i = 0; i < 5; i++) os << "[" << shape()[i] << "]";
    os << endl;
    return;
}

std::ostream &
operator<<(std::ostream & os, const Analogs & obj) {
    obj.printShape(os);
    Functions::print(os, obj);
    return os;
}
