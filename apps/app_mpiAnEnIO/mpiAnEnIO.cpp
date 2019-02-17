/*
 * File: mpiIO.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on Jan 25, 2019, 10:35 AM
 */

/** @file */

//#include "CommonExeFunctions.h"
#include "mpi.h"
#include "colorTexts.h"
#include "functions_netcdf.h"

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

// For linux system, get process id
//
//#include <sys/types.h>
//#include <unistd.h>

using namespace std;
using namespace netCDF;

const static int _MAX_FILE_NAME = 1000;
const static int _MAX_VAR_NAME = 100;

#define ERR {if(res!=NC_NOERR) {cout << "Error at line=" << __LINE__ << ": (" << res << ") "<< nc_strerror(res) << endl;MPI_Finalize();return res;}}

int my_MPI_Gatherv(
        const void* sendbuf, int sendcount, 
        int root, MPI_Comm comm, const nc_type &c) {

    void* recvbuf;
    int *recvcounts, *displs;
    
    switch (c) {
        case NC_CHAR:
            return (MPI_Gatherv(sendbuf, sendcount, MPI_CHAR, recvbuf, recvcounts, displs, MPI_CHAR, root, comm));
        case NC_BYTE:
            return (MPI_Gatherv(sendbuf, sendcount, MPI_BYTE, recvbuf, recvcounts, displs, MPI_BYTE, root, comm));
        case NC_SHORT:
            return (MPI_Gatherv(sendbuf, sendcount, MPI_SHORT, recvbuf, recvcounts, displs, MPI_SHORT, root, comm));
        case NC_INT:
            return (MPI_Gatherv(sendbuf, sendcount, MPI_INT, recvbuf, recvcounts, displs, MPI_INT, root, comm));
        case NC_FLOAT:
            // I do not want to pass float values because they are handled as double in AnEnIO.
            // I will convert them explicitly.
            //
            // return MPI_FLOAT;
            // break;
        case NC_DOUBLE:
            //        case NC_UBYTE:
            //        case NC_USHORT:
            //        case NC_UINT:
            //        case NC_UINT64:
            //            return MPI_UNSIGNED;
            //            break;
            return (MPI_Gatherv(sendbuf, sendcount, MPI_DOUBLE, recvbuf, recvcounts, displs, MPI_DOUBLE, root, comm));
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
 * This utility contains the child functions to read and write
 * NetCDF files with MPI processes using the spawn function
 * from MPI.
 *
 * ********************************************************
 *        USERS, PLEASE DO NOT USE THIS PROGRAM
 *        DIRECTLY IF YOU DO NOT KNOW WHAT IT IS.
 * ********************************************************
 */

//int main(int argc, char** argv) {
//    MPI_Init(&argc, &argv);
//    int world_size = -1, world_rank = -1;
//
//    // Get the parent communicator
//    MPI_Comm inter_parent, parent;
//    MPI_Comm_get_parent(&inter_parent);
//
//    if (inter_parent == MPI_COMM_NULL) {
//        cout << BOLDRED << "Child cannot get parent communicator. Something is wrong!"
//                << RESET << endl;
//        MPI_Finalize();
//        return 1;
//    }
//
//    //int parent_size = 0;
//    //MPI_Comm_remote_size(parent, &parent_size);
//    //if (parent_size != 1) {
//    //    cout << BOLDRED << "Child rank #" << world_rank
//    //            << " should be only 1 parent. Multiple found." << RESET << endl;
//    //    MPI_Finalize();
//    //    return 1;
//    //}
//
//    // Get the size and rank of the current communicator
////    MPI_Intercomm_merge(inter_parent, 1, &parent);
//    MPI_Comm_size(inter_parent, &world_size);
//    MPI_Comm_rank(inter_parent, &world_rank);
//
//    // For linux system, get process id
//    //
//    pid_t pid = getpid();
//    cout << "***************** pid " << pid << " is world rank: "
//        << world_rank << " *****************" << endl;
//    
//    char sendbuf[5] = {'a', 'b', 'd', 'e', 'f'};
//    void* recvbuf;
//    int *recvcounts, *displs;
//    MPI_Gatherv(sendbuf, 5, MPI_CHAR, recvbuf, recvcounts, displs, MPI_CHAR, 0, inter_parent);
//            
//    MPI_Finalize();
//    return 0;
//}

int main(int argc, char** argv) {

    MPI_Init(&argc, &argv);
    int world_size = -1, world_rank = -1;

    // Get the parent communicator
    MPI_Comm parent;
    MPI_Comm_get_parent(&parent);

    if (parent == MPI_COMM_NULL) {
        cout << BOLDRED << "Child cannot get parent communicator. Something is wrong!"
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
//    MPI_Intercomm_merge(inter_parent, 1, &parent);
    MPI_Comm_size(parent, &world_size);
    MPI_Comm_rank(parent, &world_rank);

    // For linux system, get process id
    //
    //pid_t pid = getpid();
    //cout << "***************** pid " << pid << " is world rank: "
    //    << world_rank << " *****************" << endl;

    // Get variables broadcasted from parent
    char *p_file_name = new char[_MAX_FILE_NAME],
            *p_var_name = new char[_MAX_VAR_NAME];
    int num_indices = 0, verbose = 0;

    MPI_Bcast(p_file_name, _MAX_FILE_NAME, MPI_CHAR, 0, parent);
    MPI_Bcast(p_var_name, _MAX_VAR_NAME, MPI_CHAR, 0, parent);
    MPI_Bcast(&num_indices, 1, MPI_INT, 0, parent);
    MPI_Bcast(&verbose, 1, MPI_INT, 0, parent);

    if (verbose >= 4) cout << GREEN << "Child rank #" << world_rank
            << " received from the parent's broadcast ..." << RESET << endl;

    int *p_start = new int[num_indices](),
            *p_count = new int[num_indices]();

    MPI_Bcast(p_start, num_indices, MPI_INT, 0, parent);
    MPI_Bcast(p_count, num_indices, MPI_INT, 0, parent);

    // Define the start and count indices
    // Distribution only happens to the first dimension for simplicity.
    //
    p_start[0] += (world_rank) * (int)(p_count[0] / (world_size));

    if (world_rank == world_size - 1)
        p_count[0] = p_count[0] - p_start[0];
    else
        p_count[0] = p_count[0] / (world_size);

    if (verbose >= 4) {
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

    // Check whether the file supported parallel access
    if (world_rank == 1) {
        res = nc_open(p_file_name, NC_NOWRITE, &ncid);
        ERR;

        int format = -1;
        res = nc_inq_format(ncid, &format);
        ERR;

        if (format == NC_FORMAT_NETCDF4 || format == NC_FORMAT_NETCDF4_CLASSIC) {
            // Correct! Expected NetCDF 4 format which can be accessed by MPI
        } else {
            if (verbose >= 1) cout << BOLDRED << "Error: The NetCDF file ("
                << format << ") is not NETCDF4 format ("
                    << NC_FORMAT_NETCDF4 << "," << NC_FORMAT_NETCDF4_CLASSIC
                    << "). Please turn off MPI and reinstall the program. "
                    "(This might be a signal of something is wrong. Please submit a ticket.)" << RESET << endl;
            MPI_Finalize();
            return 1;
        }

        nc_close(ncid);
    }

    //MPI_Barrier(MPI_COMM_WORLD);

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
    
    if (verbose >= 4) cout << GREEN << "Rank #" << world_rank
            << " sending data (" << len << ") back to the parent ..."
            << RESET << endl;

    //cout << "#" << world_rank << ": ";
    //char *pd = (char*)p_vals;
    //for (size_t i = 0; i < len; i++) {
    //    cout << pd[i] << " ";
    //}
    //cout << endl;

    if (my_MPI_Gatherv(p_vals, len, 0, parent, var_type) != MPI_SUCCESS) {
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
    if (verbose >= 4) cout << GREEN << "Rank #" << world_rank
            << " terminated successfully." << RESET << endl;

    return 0;
}
