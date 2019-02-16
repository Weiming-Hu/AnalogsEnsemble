/* 
 * File:   AnEnIO.tpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on September 25, 2018, 3:28 PM
 */

/************************************************************************
 *                      Public Template Functions                       *
 ************************************************************************/
#include "boost/numeric/ublas/io.hpp"

#include <iostream>
#include <fstream>
#include <cstdlib>

// Boost does not provide the correct functions to read nan values 
// for double, the operator is overloaded.
//
namespace boost { namespace numeric { namespace ublas {
    template<class E, class T, class MF, class MA>
        std::basic_istream<E, T> &operator >> (std::basic_istream<E, T> &is,
                matrix<double, MF, MA> &m) {
            typedef typename matrix<double, MF, MA>::size_type size_type;
            char preview;
            E ch;
            size_type size1, size2;
            if (is >> ch && ch != '[') {
                is.putback (ch);
                is.setstate (std::ios_base::failbit);
            } else if (is >> size1 >> ch && ch != ',') {
                is.putback (ch);
                is.setstate (std::ios_base::failbit);
            } else if (is >> size2 >> ch && ch != ']') {
                is.putback (ch);
                is.setstate (std::ios_base::failbit);
            } else if (! is.fail ()) {
                matrix<double, MF, MA> s (size1, size2);
                if (is >> ch && ch != '(') {
                    is.putback (ch);
                    is.setstate (std::ios_base::failbit);
                } else if (! is.fail ()) {
                    for (size_type i = 0; i < size1; i ++) {
                        if (is >> ch && ch != '(') {
                            is.putback (ch);
                            is.setstate (std::ios_base::failbit);
                            break;
                        }
                        for (size_type j = 0; j < size2; j ++) {

                            // Add codes to read 'nan' values
                            //
                            // If the input starts with 'n' I assume it
                            // is going to be an nan vaue.
                            //
                            if (is >> preview && preview == 'n') {
                                if (is >> preview && preview == 'a' && 
                                        is >> preview && preview == 'n') {
                                    s (i,j) = NAN;

                                    if (is >> preview && preview != ',') {
                                        is.putback (preview);
                                        if (j < size2 - 1) {
                                            is.setstate (std::ios_base::failbit);
                                            break;
                                        }
                                    }
                                } else {
                                    is.setstate (std::ios_base::failbit);
                                    break;
                                }
                            } else {
                                is.putback(preview);
                                if (is >> s (i, j) >> ch && ch != ',') {
                                    is.putback (ch);
                                    if (j < size2 - 1) {
                                        is.setstate (std::ios_base::failbit);
                                        break;
                                    }
                                }
                            }
                        }
                        if (is >> ch && ch != ')') {
                            is.putback (ch);
                            is.setstate (std::ios_base::failbit);
                            break;
                        }
                        if (is >> ch && ch != ',') {
                            is.putback (ch);
                            if (i < size1 - 1) {
                                is.setstate (std::ios_base::failbit);
                                break;
                            }
                        }
                    }
                    if (is >> ch && ch != ')') {
                        is.putback (ch);
                        is.setstate (std::ios_base::failbit);
                    }
                }
                if (! is.fail ())
                    m.swap (s);
            }
            return is;
        }
} } } 

/**
 * Reads the matrix of type boost::numeric::ublas::matrix<T>.
 * Currently AnEn::TimeMapMatrix and AnEn::SearchStationMatrix are using
 * this template.
 * 
 * @param matrix A boost matrix to store the data.
 * @return An errorType.
 */
template <typename T>
errorType
AnEnIO::readTextMatrix(boost::numeric::ublas::matrix<T> & mat) {
    
    using namespace std;
    
    if (verbose_ >= 3) cout << "Reading text matrix ..." << endl;
    
    if (mode_ != "Read") {
        if (verbose_ >= 1) cout << BOLDRED << "Error: Mode should be 'Read'."
                << RESET << endl;
        return (WRONG_MODE);
    }
    
    ifstream in;
    
    in.open(file_path_);
    in >> mat;
    in.close();

    if ( (in.rdstate() & std::ifstream::failbit ) != 0 ) {
        if (verbose_ >= 1) {
            cout << BOLDRED << "Error occurred during reading matrix file "
                << getFilePath() << RESET << endl;
        }
        return (FILEIO_ERROR);
    }

    return (SUCCESS);
}

/**
 * Writes the matrix of type boost::numeric::ublas::matrix<T> into a file.
 * Currently AnEn::TimeMapMatrix and AnEn::SearchStationMatrix are using
 * this template.
 * 
 * @param mat_in A boost matrix to write.
 * @return An errorType.
 */
template <typename T>
errorType
AnEnIO::writeTextMatrix(
        const boost::numeric::ublas::matrix<T> & mat) const {
    
    using namespace std;
    
    if (verbose_ >= 3) cout << "Writing text matrix ..." << endl;
    
    if (mode_ != "Write") {
        if (verbose_ >= 1) cout << BOLDRED << "Error: Mode should be 'Write'."
                << RESET << endl;
        return (WRONG_MODE);
    }
    
    ifstream in;
    ofstream out;
    out.open(file_path_);
    out << mat;
    out.close();

    if ( (in.rdstate() & std::ifstream::failbit ) != 0 ) {
        if (verbose_ >= 1) {
            cout << BOLDRED << "Error occurred during reading matrix file "
                << getFilePath() << RESET << endl;
        }
        return (FILEIO_ERROR);
    }
    
    return (SUCCESS);
}


/************************************************************************
 *                      Private Template Functions                      *
 ************************************************************************/

template<typename T>
errorType
AnEnIO::read_vector_(std::string var_name, std::vector<T> & results) const {

    using namespace netCDF;
    using namespace std;

    if (mode_ != "Read") {
        if (verbose_ >= 1) cout << "Error: Mode should be 'Read'." << endl;
        return (WRONG_MODE);
    }

    NcFile nc(file_path_, NcFile::FileMode::read);

    NcVar var = nc.getVar(var_name);
    if (var.isNull()) {
        if (verbose_ >= 1) cout << BOLDRED << "Error: Could not"
                << " find variable " << var_name << "!" << RESET << endl;
        return (WRONG_INDEX_SHAPE);
    }

    const auto & var_dims = var.getDims();
    T *p_vals = nullptr;

    size_t total = 1;
    for (const auto & dim : var_dims) {
        total *= dim.getSize();
    }

    try {
        results.resize(total);
    } catch (bad_alloc & e) {
        nc.close();
        if (verbose_ >= 1) cout << BOLDRED
                << "Error: Insufficient memory when reading variable ("
                << var_name << ")!" << RESET << endl;
        nc.close();
        return (INSUFFICIENT_MEMORY);
    }

    p_vals = results.data();
    read_vector_(var, p_vals, total);

    nc.close();

    // Don't delete pointer because it is managed by the vector object
    // delete [] p_vals;
    
    return (SUCCESS);
};

template<typename T>
void
AnEnIO::read_vector_(const netCDF::NcVar & var, T *p_vals,
        const size_t & total) const {

#if defined(_ENABLE_MPI)
    if (NC4_ && total > _SERIAL_LENGTH_LIMIT) {
        std::vector<size_t> start(0), count(0);
        MPI_read_vector_(var, p_vals, start, count);
    } else {
#endif
        var.getVar(p_vals);
#if defined(_ENABLE_MPI)
    }
#endif

}

template<typename T>
errorType
AnEnIO::read_vector_(std::string var_name, std::vector<T> & results,
        std::vector<size_t> start, std::vector<size_t> count,
        std::vector<ptrdiff_t> stride) const {

    using namespace netCDF;
    using namespace std;

    if (mode_ != "Read") {
        if (verbose_ >= 1) cout << "Error: Mode should be 'Read'." << endl;
        return (WRONG_MODE);
    }

    NcFile nc(file_path_, NcFile::FileMode::read);
    NcVar var = nc.getVar(var_name);
    T *p_vals = nullptr;

    size_t total = 1;
    for (auto i : count) {
        total *= i;
    }

    try {
        results.resize(total);
    } catch (bad_alloc & e) {
        nc.close();
        if (verbose_ >= 1) cout << BOLDRED
                << "Error: Insufficient memory when reading variable ("
                << var_name << ")!" << RESET << endl;
        nc.close();
        return (INSUFFICIENT_MEMORY);
    }

    p_vals = results.data();

    // Reverse the order because of the storage style of NetCDF
    reverse(start.begin(), start.end());
    reverse(count.begin(), count.end());
    reverse(stride.begin(), stride.end());

    read_vector_(var, p_vals, start, count, stride, total);
    
    nc.close();
    // Don't delete pointer because it is managed by the vector object
    // delete [] p_vals;
    
    return (SUCCESS);
};

template<typename T>
errorType
AnEnIO::read_vector_(std::string var_name, std::vector<T> & results,
        size_t start, size_t count, ptrdiff_t stride) const {
    std::vector<size_t> vec_start{start}, vec_count{count};
    std::vector<ptrdiff_t> vec_stride{stride};
    return (read_vector_(var_name, results,
            vec_start, vec_count, vec_stride));
};

template<typename T>
void
AnEnIO::read_vector_(const netCDF::NcVar & var, T *p_vals,
        const std::vector<size_t> & start,
        const std::vector<size_t> & count,
        const std::vector<ptrdiff_t> & stride,
        const size_t & total) const {

    // using namespace std;

#if defined(_ENABLE_MPI)
    // Check whether stride is used.
    bool use_MPI = all_of(stride.begin(), stride.end(), [](ptrdiff_t i) {
            return (i == 1);
            });

    // Check whether there are enough value to write
    use_MPI &= total > _SERIAL_LENGTH_LIMIT;

    if (NC4_ && use_MPI) {
        // cout << "************  using MPI " << var.getName() << "  ************" << endl;
        MPI_read_vector_(var, p_vals, start, count);
    } else {
#endif
        // cout << "************  NOT using MPI " << var.getName() << "************" << endl;
        var.getVar(start, count, stride, p_vals);
#if defined(_ENABLE_MPI)
    }
#endif

}

#if defined(_ENABLE_MPI)
template<typename T>
int
AnEnIO::my_MPI_Gatherv(
        const void* sendbuf, int sendcount, 
        void* recvbuf, const int* recvcounts, const int* displs,
        int root, MPI_Comm comm) const {

    char name = typeid (T).name()[0];

    switch (name) {
        case 'i':
            return (MPI_Gatherv(sendbuf, sendcount, MPI_INT, recvbuf, recvcounts, displs, MPI_INT, root, comm));
        case 'f':
        case 'd':
            return (MPI_Gatherv(sendbuf, sendcount, MPI_DOUBLE, recvbuf, recvcounts, displs, MPI_DOUBLE, root, comm));
        case 'c':
            return (MPI_Gatherv(sendbuf, sendcount, MPI_CHAR, recvbuf, recvcounts, displs, MPI_CHAR, root, comm));
        case 's':
            return (MPI_Gatherv(sendbuf, sendcount, MPI_SHORT, recvbuf, recvcounts, displs, MPI_SHORT, root, comm));
        case 'b':
            return (MPI_Gatherv(sendbuf, sendcount, MPI_BYTE, recvbuf, recvcounts, displs, MPI_BYTE, root, comm));
    }

    throw std::runtime_error("Cannot deduce the MPI Datatype.");
}

template<typename T>
MPI_Datatype
AnEnIO::get_mpi_type() const {
    
    char name = typeid (T).name()[0];
    
    switch (name) {
        case 'i':
            return MPI_INT;
            break;
//        case 'j':
//            return MPI_UNSIGNED;
//            break;
        case 'f':
            // I convert float values to double
            // return MPI_FLOAT;
            // break;
        case 'd':
            return MPI_DOUBLE;
            break;
        case 'c':
            return MPI_CHAR;
            break;
        case 's':
            return MPI_SHORT;
            break;
//        case 'l':
//            return MPI_LONG;
//            break;
//        case 'm':
//            return MPI_UNSIGNED_LONG;
//            break;
        case 'b':
            return MPI_BYTE;
            break;
    }

    throw std::runtime_error("Cannot deduce the MPI Datatype.");
}

template<typename T>
void
AnEnIO::MPI_read_vector_(const netCDF::NcVar & var, T* & p_vals,
        const std::vector<size_t> & start, const std::vector<size_t> & count) const {

    using namespace netCDF;
    using namespace std;
    
    auto var_name = var.getName();
    const auto & var_dims = var.getDims();
    
    handle_MPI_Init();
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    int num_children = var_dims[0].getSize();
    int num_procs_sys = num_children;

    char *num_procs_str = getenv("OMP_NUM_THREADS");
    if (num_procs_str) {
        sscanf(num_procs_str, "%d", &num_procs_sys);
#if defined(_OPENMP)
    } else {
        num_procs_sys = omp_get_num_procs();
#endif
    }

    if (num_children > num_procs_sys) {
        num_children = num_procs_sys;
    }

    if (world_rank == 0) {

        // Spawn child processes
        if (verbose_ >= 4) cout << "Spawning " << num_children
                << " processes to read " << var_name << " ..." << endl;

        MPI_Comm children, intra_children;
        if (MPI_Comm_spawn(mpiAnEnIO_.c_str(), MPI_ARGV_NULL, num_children, MPI_INFO_NULL,
                0, MPI_COMM_SELF, &children, MPI_ERRCODES_IGNORE) != MPI_SUCCESS) {
            throw runtime_error("Spawning children failed.");
        }
        MPI_Intercomm_merge(children, 0, &intra_children);

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
        
        char *p_file_path = new char[file_path_.length() + 1];
        char *p_var_name = new char[var_name.length() + 1];
        strcpy(p_file_path, file_path_.c_str());
        strcpy(p_var_name, var_name.c_str());

        // Broadcast some variables to children
        if (verbose_ >= 4) cout << "Broadcasting variables ..." << endl;
        //MPI_Bcast(p_file_path, file_path_.length() + 1, MPI_CHAR, MPI_ROOT, children);
        //MPI_Bcast(p_var_name, var_name.length() + 1, MPI_CHAR, MPI_ROOT, children);
        //MPI_Bcast(&num_indices, 1, MPI_INT, MPI_ROOT, children);
        MPI_Bcast(p_file_path, file_path_.length() + 1, MPI_CHAR, 0, intra_children);
        MPI_Bcast(p_var_name, var_name.length() + 1, MPI_CHAR, 0, intra_children);
        MPI_Bcast(&num_indices, 1, MPI_INT, 0, intra_children);

        int verbose = verbose_;
        //MPI_Bcast(&verbose, 1, MPI_INT, MPI_ROOT, children);
        //MPI_Bcast(p_start, num_indices, MPI_INT, MPI_ROOT, children);
        //MPI_Bcast(p_count, num_indices, MPI_INT, MPI_ROOT, children);
        MPI_Bcast(&verbose, 1, MPI_INT, 0, intra_children);
        MPI_Bcast(p_start, num_indices, MPI_INT, 0, intra_children);
        MPI_Bcast(p_count, num_indices, MPI_INT, 0, intra_children);

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

        // CAUTIOUS: Please leave this barrier here to ensure the execution of
        // parent and children, otherwise the execution is not predictable.
        //
        MPI_Barrier(intra_children);

        if (verbose_ >= 4) cout << "Parent waiting to gather data from processes ..." << endl;
        
        my_MPI_Gatherv<T>(NULL, 0, p_vals, recvcounts.data(), displs.data(), 0, intra_children);
        //MPI_Gatherv(NULL, 0, MPI_DATATYPE_NULL, p_vals, recvcounts.data(),
        //        displs.data(), datatype, 0, intra_children);

        delete [] p_file_path;
        delete [] p_var_name;
        delete [] p_start;
        delete [] p_count;

        handle_MPI_Finalize();
    } else {
        cout << RED << "Warning: Please only run with 1 processor because child processes are automatically spawned."
                << RESET << endl;
        handle_MPI_Finalize();
        throw runtime_error("Terminate non-root processes.");
    }
}
#endif
