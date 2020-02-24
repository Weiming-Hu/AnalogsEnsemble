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

using vector4 = size_t[4];

/**
 * \class Array4DPointer 
 * 
 * \brief Array4DPointer is an implementation of the abstract class Array4D.
 */
class Array4DPointer : virtual public Array4D {
public:
    Array4DPointer();
    Array4DPointer(const Array4DPointer& orig);
    Array4DPointer(std::size_t, std::size_t, std::size_t, std::size_t);
    virtual ~Array4DPointer();

    virtual const std::size_t* shape() const override;
    virtual std::size_t num_elements() const override;

    virtual const double* getValuesPtr() const override;
    virtual double * getValuesPtr() override;

    virtual void resize(std::size_t, std::size_t, std::size_t, std::size_t) override;
    virtual void initialize(double value) override;

    virtual double getValue(std::size_t, std::size_t, std::size_t, std::size_t) const override;
    virtual void setValue(double val, std::size_t, std::size_t, std::size_t, std::size_t) override;

    virtual void subset(const std::vector<std::size_t>&, const std::vector<std::size_t>&,
            const std::vector<std::size_t>&, const std::vector<std::size_t>&, Array4D&) const override;

    virtual void print(std::ostream &) const override;
    friend std::ostream & operator<<(std::ostream &, const Array4DPointer &);

    Array4DPointer & operator=(const Array4DPointer &);

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

    std::size_t toIndex_(std::size_t, std::size_t, std::size_t, std::size_t) const;

    void allocateMemory_();
};

#endif /* ARRAY4DPOINTER_H */

