/* 
 * File:   Array4D.h
 * Author: Guido Cervone
 *
 * Created on October 1, 2013, 9:00 AM
 *
 * This is the main container class for the 4D data structure that 
 * contains both the observations and the forecasts
 * 
 * It is implemented starting from the boost multidimensional array
 * library, which is optimized for speed and for compatibility with 
 * the STL containers. 
 *  */

#ifndef ARRAY4D_H
#define ARRAY4D_H

// code snippet for R interface
// Please don't remove
//
// [[Rcpp::depends(BH)]]
//
#include "boost/multi_array.hpp"
#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>

using namespace std;

/** GC:
 This is the basic data structure that is used to store the forecasts and
 * the observations.  Normally the data is stored in the following format:
 * 
 * [Parameter][Station][Days][Forecast Lead Time]
 * 
 * It is assumed that all the data is stored as double.  It has not been
 * templated for optimization purposes, but it should be rather trivial
 * to make this class work with different data types. 
 * 
 * The class extends the functionality of the boost multidimensional array
 * structure, which is used as the underlying container for the data.  Although
 * defining a built 4D array could lead to an increase in speed, it 
 * is not possible to use standard STL paradigm and algorithms.  The boost 
 * library allows for a transparent integration with STL, and provides an
 * optimized data structure bug free.  The library is currently maintained and
 * used worldwide.
 * 
 */
class Array4D : public boost::multi_array<double, 4> {
public:

    Array4D();
    Array4D(const Array4D&);

    Array4D(size_t d1, size_t d2, size_t d3, size_t d4);
    Array4D(vector< double > const &, size_t M, size_t N, size_t O, size_t P);
    Array4D(Array4D::array_view<4>::type &);

    // c++11 supports "constructor inheritance"
    using boost::multi_array<double, 4>::multi_array;

    virtual ~Array4D();

    Array4D & operator=(const Array4D &rhs);


    /** GC: 
     * Used to set the data from a file when read one parameter at a time
     */
    void setFirstDimension(vector<double> const & data, size_t pos);
    //void setFirstDimension(string const & filename, int pos, bool big_endian = true);

    void getFirstDimensionData(vector<double> & data, size_t pos);
    void getDataFromVector(vector<double> const & data, size_t M, size_t N, size_t O, size_t P);

    bool isCircular(size_t pos) const;

    void setCircular(size_t pos);

    void unsetCircular(size_t pos);

    bool setNames(const vector< string > & newNames);

    bool setName(size_t pos, const string & newName);

    string getName(size_t pos);

    size_t getSizeDim0() const;

    size_t getSizeDim1() const;

    size_t getSizeDim2() const;

    size_t getSizeDim3() const;

    bool myresize(index, index, index, index);

    // Randomize the array
    void randomize();

    void print(ostream &) const;
    void print_size(ostream &) const;
    friend ostream & operator<<(ostream &, Array4D const &);

    size_t get_nx();

    size_t get_ny();

    /**
     * The function sets the protected variables nx_ and ny_. These
     * two variables are set when the stations lies on a perfect
     * grid structure. They both must be set together because the
     * function makes sure that the values to be set are in consistency
     * with the number of stations in the data.
     * 
     * @param nx an size_t variable for the number of stations along x;
     * @param ny an size_t variable for the number of stations along y;
     * @return a boolean;
     */
    bool set_nx_ny(size_t nx, size_t ny);

    const vector<double> & get_xs() const;

    const vector<double> & get_ys() const;

    bool empty_xs() const;

    bool empty_ys() const;

    size_t xs_size() const;

    size_t ys_size() const;

    bool set_xs(vector<double> input);
    bool set_ys(vector<double> input);
    
    double xs_max() const;
    double xs_min() const;
    double ys_max() const;
    double ys_min() const;

protected:
    size_t nx_ = 0;
    size_t ny_ = 0;
    vector<double> xs_;
    vector<double> ys_;

private:

};

#endif /* ARRAY4D_H */

