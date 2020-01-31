/* 
 * File:   Array4D.h
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <cervone@psu.edu>
 *
 * Created on January 29, 2020, 12:36 PM
 */

#ifndef ARRAY4DPOINTER_H
#define ARRAY4DPOINTER_H

#include "Array4D.h"

class Array4DPointer : public Array4D {
public:
    Array4DPointer();
    Array4DPointer(const Array4DPointer& orig);
    Array4DPointer(std::size_t, std::size_t, std::size_t, std::size_t);
    virtual ~Array4DPointer();
    
    const std::size_t* shape() const override;
    std::size_t num_elements() const override;

    const double* getValuesPtr() const override;
    double * getValuesPtr() override;

    void resize(std::size_t, std::size_t, std::size_t, std::size_t) override;
    void initialize(double value) override;

    double getValue(std::size_t, std::size_t, std::size_t, std::size_t) const override;
    void setValue(double val, std::size_t, std::size_t, std::size_t, std::size_t) override;

    Array4DPointer & operator=(const Array4DPointer & rhs);
    
    void print(std::ostream &) const;
    friend std::ostream & operator<<(std::ostream &, const Array4DPointer &);
    
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
  
    std::size_t toIndex_(std::size_t dim0, std::size_t dim1, std::size_t dim2, std::size_t dim3) const;

    void allocateMemory_();
};

#endif /* ARRAY4DPOINTER_H */

