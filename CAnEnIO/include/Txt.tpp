/* 
 * File:   ReadTxt.tpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on January 1, 2020, 6:11 PM
 */

#include <fstream>

// Boost does not provide the correct functions to read nan values 
// for double, the operator is overloaded.
//
namespace boost {
    namespace numeric {
        namespace ublas {

            template<class E, class T, class MF, class MA>
            std::basic_istream<E, T> &operator>>(std::basic_istream<E, T> &is,
                    matrix<double, MF, MA> &m) {
                typedef typename matrix<double, MF, MA>::size_type size_type;
                char preview;
                E ch;
                size_type size1, size2;
                if (is >> ch && ch != '[') {
                    is.putback(ch);
                    is.setstate(std::ios_base::failbit);
                } else if (is >> size1 >> ch && ch != ',') {
                    is.putback(ch);
                    is.setstate(std::ios_base::failbit);
                } else if (is >> size2 >> ch && ch != ']') {
                    is.putback(ch);
                    is.setstate(std::ios_base::failbit);
                } else if (!is.fail()) {
                    matrix<double, MF, MA> s(size1, size2);
                    if (is >> ch && ch != '(') {
                        is.putback(ch);
                        is.setstate(std::ios_base::failbit);
                    } else if (!is.fail()) {
                        for (size_type i = 0; i < size1; i++) {
                            if (is >> ch && ch != '(') {
                                is.putback(ch);
                                is.setstate(std::ios_base::failbit);
                                break;
                            }
                            for (size_type j = 0; j < size2; j++) {

                                // Add codes to read 'nan' values
                                //
                                // If the input starts with 'n' I assume it
                                // is going to be an nan vaue.
                                //
                                if (is >> preview && preview == 'n') {
                                    if (is >> preview && preview == 'a' &&
                                            is >> preview && preview == 'n') {
                                        s(i, j) = NAN;

                                        if (is >> preview && preview != ',') {
                                            is.putback(preview);
                                            if (j < size2 - 1) {
                                                is.setstate(std::ios_base::failbit);
                                                break;
                                            }
                                        }
                                    } else {
                                        is.setstate(std::ios_base::failbit);
                                        break;
                                    }
                                } else {
                                    is.putback(preview);
                                    if (is >> s(i, j) >> ch && ch != ',') {
                                        is.putback(ch);
                                        if (j < size2 - 1) {
                                            is.setstate(std::ios_base::failbit);
                                            break;
                                        }
                                    }
                                }
                            }
                            if (is >> ch && ch != ')') {
                                is.putback(ch);
                                is.setstate(std::ios_base::failbit);
                                break;
                            }
                            if (is >> ch && ch != ',') {
                                is.putback(ch);
                                if (i < size1 - 1) {
                                    is.setstate(std::ios_base::failbit);
                                    break;
                                }
                            }
                        }
                        if (is >> ch && ch != ')') {
                            is.putback(ch);
                            is.setstate(std::ios_base::failbit);
                        }
                    }
                    if (!is.fail())
                        m.swap(s);
                }
                return is;
            }
        }
    }
}

template <typename T>
bool
Txt::readMatrix(const std::string & file_path, 
        boost::numeric::ublas::matrix<T> & mat) {

    using namespace std;

    Txt::checkPath(file_path, Mode::Read, "");
    
    ifstream in;

    in.open(file_path);
    in >> mat;
    in.close();

    if ((in.rdstate() & std::ifstream::failbit) != 0) {
        ostringstream msg;
        msg << "Failed to read matrix file " << file_path;
        throw runtime_error(msg.str());
    }

    return true;
}

template <typename T>
bool
Txt::writeMatrix(
        const std::string & file_path,
        const boost::numeric::ublas::matrix<T> & mat) {

    using namespace std;

    Txt::checkPath(file_path, Mode::Write, "");

    ofstream out;
    out.open(file_path);
    out << mat;
    out.close();

    if ((out.rdstate() & std::ifstream::failbit) != 0) {
        ostringstream msg;
        msg << "Failed to write matrix file " << file_path;
        throw runtime_error(msg.str());
    }

    return true;
}
