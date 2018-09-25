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

/**
 * Reads the matrix of type boost::numeric::ublas::matrix<T>.
 * Currently AnEn::TimeMapMatrix and AnEn::SearchStationMatrix are using
 * this template.
 * 
 * @param matrix A boost matrix to store the data.
 * @return An AnEnIO::errorType.
 */
template <typename T>
AnEnIO::errorType
AnEnIO::readTextMatrix(boost::numeric::ublas::matrix<T> & mat) {
    
    using namespace std;
    
    if (verbose_ >= 3) cout << "Reading text matrix ..." << endl;
    
    if (mode_ != "Read") {
        if (verbose_ >= 1) cout << BOLDRED << "Error: Mode should be 'Read'."
                << RESET << endl;
        return (WRONG_MODE);
    }
    
    handleError(checkFilePath());
    handleError(checkFileType());
    
    ifstream in;
    
    in.open(file_path_);
    in >> mat;
    in.close();

    return (SUCCESS);
}

/**
 * Writes the matrix of type boost::numeric::ublas::matrix<T> into a file.
 * Currently AnEn::TimeMapMatrix and AnEn::SearchStationMatrix are using
 * this template.
 * 
 * @param mapping A boost matrix to write.
 * @return An AnEnIO::errorType.
 */
template <typename T>
AnEnIO::errorType
AnEnIO::writeTextMatrix(
        const boost::numeric::ublas::matrix<T> & mat) const {
    
    using namespace std;
    
    if (verbose_ >= 3) cout << "Writing text matrix ..." << endl;
    
    if (mode_ != "Write") {
        if (verbose_ >= 1) cout << BOLDRED << "Error: Mode should be 'Write'."
                << RESET << endl;
        return (WRONG_MODE);
    }
    
    handleError(checkFilePath());
    
    ifstream in;
    ofstream out;
    out.open(file_path_);
    out << mat;
    out.close();
    
    return (SUCCESS);
}


/************************************************************************
 *                      Private Template Functions                      *
 ************************************************************************/

template<typename T>
AnEnIO::errorType
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

    auto var_dims = var.getDims();
    T *p_vals = nullptr;

    size_t len = 1;
    for (const auto & dim : var_dims) {
        len *= dim.getSize();
    }

    try {
        p_vals = new T[len];
    } catch (bad_alloc & e) {
        nc.close();
        if (verbose_ >= 1) cout << BOLDRED
                << "Error: Insufficient memory when reading variable ("
                << var_name << ")!" << RESET << endl;
        nc.close();
        return (INSUFFICIENT_MEMORY);
    }

    var.getVar(p_vals);
    results.assign(p_vals, p_vals + len);

    nc.close();
    delete [] p_vals;
    return (SUCCESS);
};

template<typename T>
AnEnIO::errorType
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
        p_vals = new T[total];
    } catch (bad_alloc & e) {
        nc.close();
        if (verbose_ >= 1) cout << BOLDRED
                << "Error: Insufficient memory when reading variable ("
                << var_name << ")!" << RESET << endl;
        nc.close();
        return (INSUFFICIENT_MEMORY);
    }

    // Reverse the order because of the storage style of NetCDF
    reverse(start.begin(), start.end());
    reverse(count.begin(), count.end());
    reverse(stride.begin(), stride.end());

    var.getVar(start, count, stride, p_vals);
    results.assign(p_vals, p_vals + total);

    nc.close();
    delete [] p_vals;
    return (SUCCESS);
};

template<typename T>
AnEnIO::errorType
AnEnIO::read_vector_(std::string var_name, std::vector<T> & results,
        size_t start, size_t count, ptrdiff_t stride) const {
    std::vector<size_t> vec_start{start}, vec_count{count};
    std::vector<ptrdiff_t> vec_stride{stride};
    return (read_vector_(var_name, results,
            vec_start, vec_count, vec_stride));
};