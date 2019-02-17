#include <mpi.h>
#include <iostream>
#include <string>

#include "AnEnIO.h"

using namespace std;
using namespace netCDF;

//int main (int argc, char** argv) {
//    MPI_Init(&argc, &argv);
//    int num_children = 3;
//
//    
//    
////    MPI_Intercomm_merge(children, 0, &intra_children);
//    
//    int universe_size, *universe_sizep, flag; 
//    MPI_Comm_get_attr(MPI_COMM_WORLD, MPI_UNIVERSE_SIZE,
//            &universe_sizep, &flag);
//    if (!flag) {
//        cout << "This MPI does not support UNIVERSE_SIZE." << endl;
//        MPI_Finalize();
//        return 1;
//    } else {
//        universe_size = *universe_sizep;
//    }
//    
//    if (universe_size == 1) cout << "No room to start workers" << endl;
//    
//    MPI_Comm children, intra_children;
//    if (MPI_Comm_spawn("mpiAnEnIO", MPI_ARGV_NULL, num_children, MPI_INFO_NULL,
//            0, MPI_COMM_SELF, &children, MPI_ERRCODES_IGNORE) != MPI_SUCCESS) {
//        throw runtime_error("Spawning children failed.");
//    }
//            
//    
//    char p_vals[15] = {'0','0','0','0','0','0','0','0','0','0','0','0','0','0','0'};
//    
//    int recvcounts[3] = {5, 5, 5}, displs[3] = {0, 5, 10};
//    
//    void *sendbuf = nullptr;
//    MPI_Gatherv(sendbuf, 0, MPI_CHAR, p_vals, recvcounts, displs, MPI_CHAR, MPI_ROOT, children);
//    
//    cout << "received: ";
//    for (int i = 0; i < 15; i++) {
//        cout << p_vals[i] << " ";
//    }
//    cout << endl;
//    
//    MPI_Finalize();
//    return 0;
//}

int main (int argc, char** argv) {
 
    MPI_Init(&argc, &argv);

    string file_path;

    if (argc == 2) {
        file_path = string(argv[argc - 1]);
    } else {
        cout << "Usage: Only one parameter is accepted as the file path." << endl;
        return 1;
    }

    cout << "The file to read is " << file_path << endl;
    int verbose = 6;

    NcFile nc(file_path, NcFile::FileMode::read);
    auto var = nc.getVar("StationNames");

    auto var_name = var.getName();
    const auto & var_dims = var.getDims();
    
    size_t total = 1;
    for (const auto & dim : var_dims) {
        total *= dim.getSize();
    }

    char *p_vals = new char[total];
    vector<size_t> start, count;
    int world_rank, num_children;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    char *num_procs_str = getenv("MPI_UNIVERSE_SIZE");
    if (num_procs_str) {
        sscanf(num_procs_str, "%d", &num_children);
#if defined(_OPENMP)
    } else {
        throw runtime_error("Please set MPI_UNIVERSE_SIZE to limit the number of spawned children.");
#endif
    }
    
    num_children -= 1;
    if (num_children == 0) {
        throw runtime_error("Please enlarge the MPI_UNIVERSE_SIZE to be at least 2.");
    }

    if (world_rank == 0) {

        // Spawn child processes
        if (verbose >= 4) cout << "Spawning " << num_children
                << " processes to read " << var_name << " ..." << endl;

        MPI_Comm children;
        if (MPI_Comm_spawn("mpiAnEnIO", MPI_ARGV_NULL, num_children, MPI_INFO_NULL,
                0, MPI_COMM_SELF, &children, MPI_ERRCODES_IGNORE) != MPI_SUCCESS) {
            throw runtime_error("Spawning children failed.");
        }

        // Since MPI does not allow send/receive size_t, I create a copy
        // of the start and count indices.
        //
        int num_indices = (int) var_dims.size();
        int *p_start = new int[num_indices], *p_count = new int[num_indices];

        if (start.size() == (size_t) num_indices ||
                count.size() == (size_t) num_indices) {
            for (int i = 0; i < num_indices; i++) {
                p_start[i] = (int) start[i];
                p_count[i] = (int) count[i];
            }
        } else {
            for (int i = 0; i < num_indices; i++) {
                p_start[i] = 0;
                p_count[i] = (int) var_dims[i].getSize();
            }
        }
        
        char *p_file_path = new char[file_path.length() + 1];
        char *p_var_name = new char[var_name.length() + 1];
        strcpy(p_file_path, file_path.c_str());
        strcpy(p_var_name, var_name.c_str());

        // Broadcast some variables to children
        if (verbose >= 4) cout << "Broadcasting variables ..." << endl;
        MPI_Bcast(p_file_path, file_path.length() + 1, MPI_CHAR, MPI_ROOT, children);
        MPI_Bcast(p_var_name, var_name.length() + 1, MPI_CHAR, MPI_ROOT, children);
        MPI_Bcast(&num_indices, 1, MPI_INT, MPI_ROOT, children);
//        MPI_Bcast(p_file_path, file_path.length() + 1, MPI_CHAR, 0, intra_children);
//        MPI_Bcast(p_var_name, var_name.length() + 1, MPI_CHAR, 0, intra_children);
//        MPI_Bcast(&num_indices, 1, MPI_INT, 0, intra_children);

        MPI_Bcast(&verbose, 1, MPI_INT, MPI_ROOT, children);
        MPI_Bcast(p_start, num_indices, MPI_INT, MPI_ROOT, children);
        MPI_Bcast(p_count, num_indices, MPI_INT, MPI_ROOT, children);
//        MPI_Bcast(&verbose, 1, MPI_INT, 0, intra_children);
//        MPI_Bcast(p_start, num_indices, MPI_INT, 0, intra_children);
//        MPI_Bcast(p_count, num_indices, MPI_INT, 0, intra_children);

        // Collect data from children
        vector<int> recvcounts(num_children), displs(num_children);
        int multiply = accumulate(p_count + 1, p_count + num_indices, 1, std::multiplies<int>());

        for (int i = 0; i < num_children; i++) {
            displs[i] = (i * (int)(p_count[0] / num_children) + p_start[0]) * multiply;

            if (i == num_children - 1)
                recvcounts[i] = (p_count[0] - i * (int)(p_count[0] / num_children) + p_start[0]) * multiply;
            else
                recvcounts[i] = (p_count[0] / num_children) * multiply;
        }
        
        cout << "Master process recv counts/displs: " << endl;
        for (size_t i = 0; i <recvcounts.size(); i ++)
        {
            cout << recvcounts[i] << "," << displs[i] << endl;
        }
        cout << endl;

        // CAUTIOUS: Please leave this barrier here to ensure the execution of
        // parent and children, otherwise the execution is not predictable.
        //
        MPI_Barrier(children);

        if (verbose >= 4) cout << "Parent waiting to gather data from processes ..." << endl;
        
        void *sendbuf = nullptr;
        
        MPI_Gatherv(sendbuf, 0, MPI_CHAR, p_vals, recvcounts.data(), displs.data(), MPI_CHAR, MPI_ROOT, children);
        //MPI_Gatherv(NULL, 0, MPI_DATATYPE_NULL, p_vals, recvcounts.data(),
        //
        //        displs.data(), datatype, 0, intra_children);

        delete [] p_file_path;
        delete [] p_var_name;
        delete [] p_start;
        delete [] p_count;

    } else {
        cout << RED << "Warning: Please only run with 1 processor because child processes are automatically spawned."
                << RESET << endl;
        nc.close();
        MPI_Finalize();
        throw runtime_error("Terminate non-root processes.");
    }

    cout << "Parent process finished collecting data" << endl;
   
    nc.close();

    MPI_Finalize();
    return (0);
}
