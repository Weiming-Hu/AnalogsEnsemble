/* 
 * File:   standardDeviationCalculator.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on November 29, 2018, 17:30
 */

/** @file */

#include "AnEn.h"
#include "AnEnIO.h"
#include "CommonExeFunctions.h"

#include "boost/program_options.hpp"
#include "boost/multi_array.hpp"
#include "boost/filesystem.hpp"
#include "boost/exception/all.hpp"

#include <functional>
#include <iostream>
#include <sstream>
#include <cmath>

#if defined(_ENABLE_MPI)
#include "netcdf_par.h"
#include <mpi.h>
#endif

using namespace std;

int main(int argc, char** argv) {

    int world_size = -1, world_rank = -1;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    cout << "Rank " << world_rank << " out of " << world_size << endl;

    MPI_Finalize();
    return 0;
}
