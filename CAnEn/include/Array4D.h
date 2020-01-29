/* 
 * File:   Array4D.h
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <cervone@psu.edu>
 *
 * Created on January 29, 2020, 12:36 PM
 */

#ifndef ARRAY4D_H
#define ARRAY4D_H

#include <iostream>
#include <vector>

using vector4 = size_t[4];

class Array4D {
public:
    Array4D();
    Array4D(const Array4D& orig);
    Array4D(std::size_t dim0, std::size_t dim1, std::size_t dim2, std::size_t dim3);
    virtual ~Array4D();
    
    std::vector<size_t> shape() const;
    std::size_t num_elements() const;

    const double* getValuesPtr() const;
    double * getValuesPtr();

    void resize(std::size_t dim0, std::size_t dim1, std::size_t dim2, std::size_t dim3);
    void initialize(double value);

    double getValue(std::size_t dim0, std::size_t dim1, std::size_t dim2, std::size_t dim3) const;
    void setValue(double val,
            std::size_t dim0, std::size_t dim1, std::size_t dim2, std::size_t dim3);

    std::size_t toIndex(std::size_t dim0, std::size_t dim1, std::size_t dim2, std::size_t dim3) const;

    Array4D & operator=(const Array4D & rhs);
    
    void print(std::ostream &) const;
    friend std::ostream & operator<<(std::ostream &, const Array4D &);
    
    static const double _DEFAULT_VALUE;
    
protected:
    vector4 dims_;
    double * data_;

    /**
     * This variable is used to keep track of whether the data memory is 
     * allocated internally and whether the pointer should be deleted during
     * the de-constructor.
     */
    bool allocated_;

    void allocateMemory_();
};

#endif /* ARRAY4D_H */

