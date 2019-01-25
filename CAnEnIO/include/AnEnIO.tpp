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
        results.resize(len);
    } catch (bad_alloc & e) {
        nc.close();
        if (verbose_ >= 1) cout << BOLDRED
                << "Error: Insufficient memory when reading variable ("
                << var_name << ")!" << RESET << endl;
        nc.close();
        return (INSUFFICIENT_MEMORY);
    }

    p_vals = results.data();
    var.getVar(p_vals);

    nc.close();

    // Don't delete pointer because it is managed by the vector object
    // delete [] p_vals;
    
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

    var.getVar(start, count, stride, p_vals);
    nc.close();
    // Don't delete pointer because it is managed by the vector object
    // delete [] p_vals;
    
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
