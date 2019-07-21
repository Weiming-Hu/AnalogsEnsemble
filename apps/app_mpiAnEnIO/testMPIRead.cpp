#include <mpi.h>
#include <iostream>
#include <string>

#include "AnEnIO.h"

using namespace std;
using namespace netCDF;

//int main(int argc, char** argv) {
//
//    AnEnIO::handle_MPI_Init();
//
//    double wtime_start = MPI_Wtime();
//
//    string file_path;
//
//    if (argc == 2) {
//        file_path = string(argv[argc - 1]);
//    } else {
//        cout << "Usage: Only one parameter is accepted as the file path." << endl;
//        return 1;
//    }
//
//    AnEnIO io("Read", file_path, "Observations", 6);
//    Observations_array observations;
//    io.readObservations(observations);
//
//    AnEnIO::handle_MPI_Finalize();
//
//    double wtime_end = MPI_Wtime();
//
//    cout << endl << "********************************" << endl
//            << "Total execution time: " << wtime_end - wtime_start << " s;" << endl
//            << "********************************" << endl;
//
//
//    return (0);
//}

int main (int argc, char** argv) {
 
    MPI_Init(&argc, &argv);
    
    double wtime_start = MPI_Wtime();
    double wtime_barrier;

    string file_path;

    if (argc >= 2) {
        file_path = string(argv[1]);
    } else {
        cout << "Usage: testMPIRead <file path> [start index 0] [start index 1] [start index 2] [start index 3] [count index 0] [count index 1] [count index 2] [count index 3]." << endl;
        return 1;
    }

    vector<size_t> start, count;
    if (argc == 10) {
        start.resize(4);
        start[0] = atoi(argv[2]);
        start[1] = atoi(argv[3]);
        start[2] = atoi(argv[4]);
        start[3] = atoi(argv[5]);
        count.resize(4);
        count[0] = atoi(argv[6]);
        count[1] = atoi(argv[7]);
        count[2] = atoi(argv[8]);
        count[3] = atoi(argv[9]);
    }

    cout << "The file to read is " << file_path << endl;
    int verbose = 6;

    NcFile nc(file_path, NcFile::FileMode::read);
    auto var = nc.getVar("Data");

    auto var_name = var.getName();
    const auto & var_dims = var.getDims();
    
    size_t total = 1;
    for (const auto & dim : var_dims) {
        total *= dim.getSize();
    }

    double *p_vals = new double[total];
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

        MPI_Bcast(&verbose, 1, MPI_INT, MPI_ROOT, children);
        MPI_Bcast(p_start, num_indices, MPI_INT, MPI_ROOT, children);
        MPI_Bcast(p_count, num_indices, MPI_INT, MPI_ROOT, children);

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
        
//        cout << "Master process recv counts/displs: " << endl;
//        for (size_t i = 0; i <recvcounts.size(); i ++)
//        {
//            cout << recvcounts[i] << "," << displs[i] << endl;
//        }
//        cout << endl;

        // CAUTIOUS: Please leave this barrier here to ensure the execution of
        // parent and children, otherwise the execution is not predictable.
        //
        MPI_Barrier(children);
        wtime_barrier = MPI_Wtime();

        if (verbose >= 4) cout << "Parent waiting to gather data from processes ..." << endl;
        
        void *sendbuf = nullptr;
        
        MPI_Gatherv(sendbuf, 0, MPI_DOUBLE, p_vals, recvcounts.data(), displs.data(), MPI_DOUBLE, MPI_ROOT, children);

        delete [] p_file_path;
        delete [] p_var_name;
        delete [] p_start;
        delete [] p_count;

    } else {
        cerr << RED << "Warning: Please only run with 1 processor because child processes are automatically spawned."
                << RESET << endl;
        nc.close();
        MPI_Finalize();
        throw runtime_error("Terminate non-root processes.");
    }

    cout << "Parent process finished collecting data" << endl;
   
    nc.close();

    MPI_Finalize();
    
    double wtime_end = MPI_Wtime();
    
    cout << endl << "********************************" << endl
            << "Total execution time: " << wtime_end - wtime_start << " s;" << endl
            << "Time until the barrier: " << wtime_barrier - wtime_start << " s;" << endl
            << "Time for MPI_Gatherv: " << wtime_end - wtime_barrier << " s;" << endl
            << "********************************" << endl;
    
    
    return (0);
}
