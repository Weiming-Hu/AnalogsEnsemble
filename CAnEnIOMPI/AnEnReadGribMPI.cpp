/* 
 * File:   AnEnReadGribMPI.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on March, 2020, 11:10 AM
 */

#include "AnEnReadGribMPI.h"

#include <stdexcept>
#include <exception>

using namespace std;

AnEnReadGribMPI::AnEnReadGribMPI() {
    Config config;
    verbose_ = config.verbose;
}

AnEnReadGribMPI::AnEnReadGribMPI(const AnEnReadGribMPI& orig) : AnEnReadGrib(orig) {
}

AnEnReadGribMPI::AnEnReadGribMPI(Verbose verbose) : AnEnReadGrib(verbose) {
}

AnEnReadGribMPI::~AnEnReadGribMPI() {
}

void
AnEnReadGribMPI::readForecasts(Forecasts & forecasts,
        const std::vector<ParameterGrib> & grib_parameters,
        const std::vector<std::string> & files,
        const std::string & regex_str,
        size_t unit_in_seconds,
        bool delimited,
        std::vector<int> stations_index) const {

    int world_rank, num_children;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_children);

    MPI_Finalize();
    exit(0);
}

