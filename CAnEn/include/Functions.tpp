/* 
 * File:   Functions.tpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on January 7, 2020, 10:45 AM
 */

template <typename T>
void Functions::print(std::ostream & os, const boost::multi_array<T, 3> & arr) {
    
    using namespace std;
    const auto & sizes = arr.shape();

    for (size_t m = 0; m < sizes[0]; m++) {
        cout << "[" << m << ",,]" << endl;
        for (size_t p = 0; p < sizes[2]; p++) os << "\t[,," << p << "]";
        os << endl;

        for (size_t o = 0; o < sizes[1]; o++) {
            os << "[," << o << ",]\t";
            for (size_t p = 0; p < sizes[2]; p++) os << arr[m][o][p] << "\t";
            os << endl;
        }
        os << endl;
    }
    return;
}

template <typename T>
void Functions::print(std::ostream & os, const boost::multi_array<T, 4> & arr) {

    using namespace std;
    const auto & sizes = arr.shape();

    for (size_t m = 0; m < sizes[0]; m++) {
        for (size_t n = 0; n < sizes[1]; n++) {
            cout << "[" << m << "," << n << ",,]" << endl;
            for (size_t p = 0; p < sizes[3]; p++) os << "\t[,,," << p << "]";
            os << endl;

            for (size_t o = 0; o < sizes[2]; o++) {
                os << "[,," << o << ",]\t";
                for (size_t p = 0; p < sizes[3]; p++) os << arr[m][n][o][p] << "\t";
                os << endl;
            }
            os << endl;
        }
    }

    return;
}

template <typename T>
void Functions::print(std::ostream & os, const boost::multi_array<T, 5> & arr) {
    
    using namespace std;

    size_t dim1 = arr.shape()[0];
    size_t dim2 = arr.shape()[1];
    size_t dim3 = arr.shape()[2];
    size_t dim4 = arr.shape()[3];
    size_t dim5 = arr.shape()[4];

    for (size_t i_dim1 = 0; i_dim1 < dim1; i_dim1++) {
        for (size_t i_dim2 = 0; i_dim2 < dim2; i_dim2++) {
            for (size_t i_dim3 = 0; i_dim3 < dim3; i_dim3++) {

                os << "[" << i_dim1 << "][" << i_dim2 << "]["
                        << i_dim3 << "][][]" << endl;
                os << "\t\t [, 0] \t\t [, 1] \t\t [, 2]" << endl;
                for (size_t i_dim4 = 0; i_dim4 < dim4; i_dim4++) {

                    os << "[" << i_dim4 << ", ] ";
                    for (size_t i_dim5 = 0; i_dim5 < dim5; i_dim5++) {
                        os << " \t\t " << arr[i_dim1][i_dim2]
                                [i_dim3][i_dim4][i_dim5];
                    }
                    os << endl;
                }
            }
        }
    }

    return;
}
