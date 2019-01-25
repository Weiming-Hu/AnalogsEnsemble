/*
 * File: mpiIO.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on Jan 25, 2019, 10:35 AM
 */

/** @file */

#include "CommonExeFunctions.h"
#include "colorTexts.h"

#include "boost/program_options.hpp"
#include "boost/filesystem.hpp"
#include "boost/exception/all.hpp"

#include "mpi.h"

#include <iostream>
using namespace std;

/**
 * This is the child utility supporting the MPI I/O process
 * of NetCDF files. This utility should not be called 
 * directly by the user. Rather, this utility is automatically
 * called by the CAnEnIO library.
 *
 * This utility contains the child functino to read and write
 * NetCDF files with MPI processes using the spawn function
 * from MPI.
 *
 * ********************************************************
 *        USERS, PLEASE DO NOT USE THIS PROGRAM
 *        DIRECTLY IF YOU DO NOT KNOW WHAT IT IS.
 * ********************************************************
 */
int main(int argc, char** argv) {
    cout << BOLDGREEN << "Parallel Ensemble Forecasts --- MPI IO (Child)"
        << _APPVERSION << RESET << endl << GREEN << _COPYRIGHT_MSG << RESET << endl;
}
