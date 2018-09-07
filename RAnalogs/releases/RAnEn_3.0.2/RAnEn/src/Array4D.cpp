/* 
 * File:   Matrix4D.cpp
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <cervone@psu.edu>
 * 
 * Created on October 1, 2013, 9:00 AM
 */

#include <iterator>
#include <cstdlib>
#include <utility>
#include <algorithm>

#include "Array4D.h"

using namespace std;

Array4D::Array4D() {
}

Array4D::Array4D(const Array4D& rhs) {
    *this = rhs;
}

Array4D::Array4D(size_t d1, size_t d2, size_t d3, size_t d4) {
    //cout << "Array4D Constructor: With Dimensions" << endl;
    try {
        resize(boost::extents[d1][d2][d3][d4]);
    } catch (bad_alloc & e) {
        cerr << "ERROR: insufficient memory while resizing the array4D to hold "
                << d1 * d2 * d3 * d4 << " double values"
                << endl;
        throw;
    }

}

Array4D::Array4D(Array4D::array_view<4>::type & view) : boost::multi_array<double, 4>(view) {

}

//Array4D::Array4D(Array4D const & rhs) {
//    *this = rhs;
//}

Array4D & Array4D::operator=(const Array4D &rhs) {

    if (this != &rhs) {
        try {
            resize(boost::extents
                    [rhs.shape()[0]][rhs.shape()[1]]
                    [rhs.shape()[2]][rhs.shape()[3]]);
        } catch (bad_alloc e) {
            cerr << "ERROR: insufficient memory while resizing the array4D to hold "
                    << rhs.shape()[0] * rhs.shape()[1] * rhs.shape()[2] * rhs.shape()[3] << " double values"
                    << endl;
            throw e;
        }

        // call base class assignment operator explicitly
        //
        boost::multi_array<double, 4>::operator=(rhs);

    }

    return *this;
}

void
Array4D::setDataFromVector(vector<double> const & data,
        size_t M, size_t N, size_t O, size_t P) {
    // fill the array4D using the data from a vector

    // Resize the current array
    try {
        resize(boost::extents[M][N][O][P]);
    } catch (bad_alloc & e) {
        cerr << "ERROR: insufficient memory while resizing the array4D to hold "
                << M * N * O * P << " double values"
                << endl;
        throw e;
    }

    size_t m = 0;
    size_t n = 0;
    size_t o = 0;
    size_t p = 0;
    
    vector<double>::const_iterator citer;

    for (citer = data.begin();
            citer != data.end(); ++citer) {
        if (m == M) {
            m = 0;
            n++;
            if (n == N) {
                n = 0;
                o++;
                if (o == O) {
                    o = 0;
                    p++;
                }
            }
        }
        (*this)[m][n][o][p] = *citer;
        m++;
    }
}

Array4D::Array4D(vector<double> const & data,
        size_t M, size_t N, size_t O, size_t P) {
    //cout << "Array4D Constructor: From vector data" << endl;
    setDataFromVector(data, M, N, O, P);
}

Array4D::~Array4D() {
    //cout << "Array4D Destructor" << endl;
}

bool
Array4D::myresize(index d1, index d2, index d3, index d4) {
    try {
        resize(boost::extents[d1][d2][d3][d4]);
    } catch (bad_alloc e) {
        cerr << "ERROR: insufficient memory while resizing the array4D to hold "
                << d1 * d2 * d3 * d4 << " double values"
                << endl;
        throw e;
    }
    return ( true);
}

void
Array4D::setFirstDimension(vector<double> const & data, size_t pos) {
    size_t m = pos;
    size_t n = 0;
    size_t o = 0;
    size_t p = 0;

    size_t N = shape()[1];
    size_t O = shape()[2];

    vector<double>::const_iterator citer;

    for (citer = data.begin();
            citer != data.end();
            citer++) {
        if (n == N) {
            n = 0;
            o++;
            if (o == O) {
                o = 0;
                p++;
            }
        }

        (*this)[m][n][o][p] = *citer;
        n++;
    }
}

void
Array4D::getFirstDimensionData(vector<double> & data, size_t pos) {
    size_t m = pos;
    size_t n = 0;
    size_t o = 0;
    size_t p = 0;

    size_t N = shape()[1];
    size_t O = shape()[2];
    size_t P = shape()[3];

    data.clear();

    while (n != (N - 1) || o != (O - 1) || p != (P - 1)) {
        if (n == N) {
            n = 0;
            o++;
            if (o == O) {
                o = 0;
                p++;
            }
        }
        data.push_back((*this)[m][n][o][p]);
        n++;
    }
}

void
Array4D::randomize() {

    size_t M = shape()[0];
    size_t N = shape()[1];
    size_t O = shape()[2];
    size_t P = shape()[3];

    for (size_t m = 0; m < M; m++) {
        for (size_t n = 0; n < N; n++) {
            for (size_t o = 0; o < O; o++) {
                for (size_t p = 0; p < P; p++) {
                    {
                        // pseudo-random is good enough
                        // no need to set seeds in each loop
                        //
                        //srand(time(0));
                        (*this)[m][n][o][p] = rand();
                    }
                }
            }
        }
    }
}

void
Array4D::print_size(ostream & os) const {

    auto M = shape()[0];
    auto N = shape()[1];
    auto O = shape()[2];
    auto P = shape()[3];

    os << "[" << M << ", " << N << ", "
            << O << ", " << P << "]" << endl;
}

void
Array4D::print(ostream & os) const {

    os << "Array Shape = ";
    for (int i = 0; i < 4; i++) {
        os << "[ " << shape()[i] << " ]";
    }
    os << endl;

    size_t M = shape()[0];
    size_t N = shape()[1];
    size_t O = shape()[2];
    size_t P = shape()[3];


    for (size_t m = 0; m < M; m++) {
        for (size_t n = 0; n < N; n++) {

            os << "[ " << m << ", " << n << ", , ]" << endl;

            for (size_t p = 0; p < P; p++) {
                os << "\t[,,," << p << "]";
            }
            os << endl;

            for (size_t o = 0; o < O; o++) {
                os << "[,," << o << ",]\t";

                for (size_t p = 0; p < P; p++) {
                    os << (*this)[m][n][o][p] << "\t";
                }
                os << endl;

            }
            os << endl;
        }
        os << endl;
    }
    os << endl;

#ifdef _DEBUG
    os << "Circular Parameters:";

    for (unsigned int i = 0; i < shape()[0]; i++) {
        os << "[" << isCircular(i) << "]";
    }

    os << endl;
#endif
}

ostream &
operator<<(ostream & os, Array4D const & bv) {
    bv.print(os);
    return os;
}

bool
Array4D::set_xs(vector<double> input) {
    if (input.size() == this->getSizeDim1()) {
        // if there are enough xs for stations
        xs_ = input;
        return true;
    } else {
        cerr << "Error: the length of input (" << input.size()
                << ") should equal to the number of stations ("
                << this->getSizeDim1() << ")!" << endl;
        return false;
    }
}

bool
Array4D::set_ys(vector<double> input) {
    if (input.size() == this->getSizeDim1()) {
        // if there are enough ys for stations
        ys_ = input;
        return true;
    } else {
        cerr << "Error: the length of input (" << input.size()
                << ") should equal to the number of stations ("
                << this->getSizeDim1() << ")!" << endl;
        return false;
    }
}

double
Array4D::xs_max() const {
    return ( *(max_element(xs_.begin(), xs_.end())));
}

double
Array4D::xs_min() const {
    return ( *(min_element(xs_.begin(), xs_.end())));
}

double
Array4D::ys_max() const {
    return ( *(max_element(ys_.begin(), ys_.end())));
}

double
Array4D::ys_min() const {
    return ( *(min_element(ys_.begin(), ys_.end())));
}

size_t
Array4D::getSizeDim0() const {
    return ( shape()[0]);
}

size_t
Array4D::getSizeDim1() const {
    return ( shape()[1]);
}

size_t
Array4D::getSizeDim2() const {
    return ( shape()[2]);
}

size_t
Array4D::getSizeDim3() const {
    return ( shape()[3]);
}

size_t
Array4D::get_nx() {
    return nx_;
}

size_t
Array4D::get_ny() {
    return ny_;
}

bool
Array4D::set_nx_ny(size_t nx, size_t ny) {
    if (this->getSizeDim1() != nx * ny) {
        return false;
    }
    nx_ = nx;
    ny_ = ny;
    return true;
}

const vector<double> &
Array4D::get_xs() const {
    return xs_;
}

const vector<double> &
Array4D::get_ys() const {
    return ys_;
}

bool
Array4D::empty_xs() const {
    return xs_.empty();
}

bool
Array4D::empty_ys() const {
    return ys_.empty();
}

size_t
Array4D::xs_size() const {
    return xs_.size();
}

size_t
Array4D::ys_size() const {
    return ys_.size();
}
