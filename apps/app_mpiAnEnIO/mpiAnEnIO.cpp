/*
 * File: mpiIO.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on Jan 25, 2019, 10:35 AM
 */

/** @file */

//#include "CommonExeFunctions.h"
#include "colorTexts.h"
#include "functions_netcdf.h"

#include "mpi.h"

#include <netcdf_par.h>
#include <stdlib.h>

#include "boost/program_options.hpp"
#include "boost/filesystem.hpp"
#include "boost/exception/all.hpp"

#include <functional>
#include <exception>
#include <iostream>
#include <numeric>
#include <string>

#include <set>

using namespace std;
using namespace netCDF;

const static int _MAX_FILE_NAME = 1000;
const static int _MAX_VAR_NAME = 100;

#define ERR {if(res!=NC_NOERR) {cout << "Error at line=" << __LINE__ << ": "<< nc_strerror(res) << endl;MPI_Finalize();return res;}}

MPI_Datatype get_mpi_type(const nc_type & c) {

    switch (c) {
        case NC_CHAR:
            return MPI_CHAR;
            break;
        case NC_BYTE:
            return MPI_BYTE;
            break;
        case NC_SHORT:
            return MPI_SHORT;
            break;
        case NC_INT:
            return MPI_INT;
            break;
        case NC_FLOAT:
            // I do not want to pass float values because they are handled as double in AnEnIO.
            // I will convert them explicitly/
            //
            // return MPI_FLOAT;
            // break;
        case NC_DOUBLE:
            return MPI_DOUBLE;
            break;
//        case NC_UBYTE:
//        case NC_USHORT:
//        case NC_UINT:
//        case NC_UINT64:
//            return MPI_UNSIGNED;
//            break;
        default:
            throw std::runtime_error("Unknown NcType");
    }
}

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

    MPI_Init(&argc, &argv);
    int world_size, world_rank;

    // Get the parent communicator
    MPI_Comm parent;
    MPI_Comm_get_parent(&parent);

    if (parent == MPI_COMM_NULL) {
        cout << BOLDRED << "Child rank #" << world_rank
                << " cannot get parent communicator. Something is wrong!"
                << RESET << endl;
        MPI_Finalize();
        return 1;
    }

    int parent_size = 0;
    MPI_Comm_remote_size(parent, &parent_size);
    if (parent_size != 1) {
        cout << BOLDRED << "Child rank #" << world_rank
                << " should be only 1 parent. Multiple found." << RESET << endl;
        MPI_Finalize();
        return 1;
    }

    // Get the size and rank of the current communicator
    MPI_Comm_size(parent, &world_size);
    MPI_Comm_rank(parent, &world_rank);

    // Get variables broadcasted from parent
    char *p_file_name = new char[_MAX_FILE_NAME],
            *p_var_name = new char[_MAX_VAR_NAME];
    int num_indices = 0, verbose = 0;

    MPI_Bcast(p_file_name, _MAX_FILE_NAME, MPI_CHAR, 0, parent);
    MPI_Bcast(p_var_name, _MAX_VAR_NAME, MPI_CHAR, 0, parent);
    MPI_Bcast(&num_indices, 1, MPI_INT, 0, parent);
    MPI_Bcast(&verbose, 1, MPI_INT, 0, parent);

    if (verbose >= 3) cout << GREEN << "Child rank #" << world_rank
            << " received from the parent's broadcast ..." << RESET << endl;

    int *p_start = new int[num_indices](),
            *p_count = new int[num_indices]();

    MPI_Bcast(p_start, num_indices, MPI_INT, 0, parent);
    MPI_Bcast(p_count, num_indices, MPI_INT, 0, parent);

    // Define the start and count indices
    // Distribution only happens to the first dimension for simplicity.
    //
    p_start[0] += world_rank * (int)(p_count[0] / world_size);

    if (world_rank == world_size - 1)
        p_count[0] = p_count[0] - p_start[0];
    else
        p_count[0] = p_count[0] / world_size;

    if (verbose >= 3) {
        cout << GREEN << "Child rank #" << world_rank
            << " reading " << string(p_var_name) << " with start/count ( ";
        for (int i = 0; i < num_indices; i++) {
            cout << p_start[i] << "," << p_count[i] << " ";
        }
        cout << ") ..." << RESET << endl;
    }

    int len = accumulate(p_count, p_count + num_indices, 1, std::multiplies<int>());

    // Open file
    int ncid = -1, varid = -1, res = -1;
    nc_type var_type;

    res = nc_open_par(p_file_name, NC_MPIIO | NC_NOWRITE, MPI_COMM_SELF,
            MPI_INFO_NULL, &ncid);
    ERR;
    res = nc_inq_varid(ncid, p_var_name, &varid);
    ERR;
    res = nc_inq_vartype(ncid, varid, &var_type);
    ERR;

    // Allocate memory
    void *p_vals = nullptr;
    allocate_memory(p_vals, var_type, len, verbose);

    vector<size_t> start(num_indices), count(num_indices);
    for (int i = 0; i < num_indices; i++) {
        start[i] = (size_t) p_start[i];
        count[i] = (size_t) p_count[i];
    }

    res = nc_get_vara(ncid, varid, start.data(), count.data(), p_vals);
    ERR;
    MPI_Datatype datatype = get_mpi_type(var_type);
    res = nc_close(ncid);
    ERR;

    if (var_type == NC_FLOAT) {
        if (verbose >= 4) cout << GREEN << "Rank #" << world_rank
            << " converting float to double ..." << RESET << endl;
        float *p_float = (float*)p_vals;
        double *p_double = new double[len];

        for (int i = 0; i < len; i++)
            p_double[i] = p_float[i];

        set<float> s1(p_float, p_float + len);
        set<double> s2(p_double, p_double + len);

        // Release the memory for the old float values
        deallocate_memory(p_vals, var_type, verbose);

        // Change the value pointer to the new double values
        p_vals = p_double;
    }

    // Make sure all children have completed reading files.
    MPI_Barrier(parent);
    
    if (verbose >= 3) cout << GREEN << "Rank #" << world_rank << " sending data ("
            << len << ") back to the parent ..." << RESET << endl;

    if (MPI_Gatherv(p_vals, len, datatype, NULL, NULL, NULL,
            MPI_DATATYPE_NULL, 0, parent) != MPI_SUCCESS) {
        if (verbose >= 1) cout << BOLDRED << "Rank #" << world_rank 
                << " failed during sending data." << RESET << endl;
        MPI_Finalize();
        return 1;
    }

    delete [] p_start;
    delete [] p_count;
    delete [] p_file_name;
    delete [] p_var_name;

    if (var_type == NC_FLOAT) {
        // I already converted this to a double pointer. 
        delete [] (double*) p_vals;
    } else {
        deallocate_memory(p_vals, var_type, verbose);
    }

    MPI_Finalize();
    if (verbose >= 3) cout << GREEN << "Rank #" << world_rank
            << " terminated successfully." << RESET << endl;

    return 0;
}
