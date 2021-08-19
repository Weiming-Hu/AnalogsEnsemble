/* 
 * File:   ObservationsPointer.h
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <weiming@psu.edu>
 *
 * Created on January 27, 2020, 5:39 PM
 */

#ifndef OBSERVATIONSPOINTER_H
#define OBSERVATIONSPOINTER_H

#include "Observations.h"

using vector3 = size_t[3];

/**
 * \class ObservationsPointer
 * 
 * \brief ObservationsPointer is an implementation of the abstract class
 * Observations. The underlying storage uses a pointer which is optimized 
 * for performance.
 */
class ObservationsPointer : virtual public Observations {
public:
    ObservationsPointer();
    ObservationsPointer(const ObservationsPointer& orig);
    ObservationsPointer(const Parameters &, const Stations &, const Times &);
    virtual ~ObservationsPointer();

    std::size_t num_elements() const override;

    const double* getValuesPtr() const override;
    double * getValuesPtr() override;

    virtual void setDimensions(const Parameters & parameters,
            const Stations & stations, const Times & times) override;

    virtual void initialize(double) override;

    virtual double getValue(std::size_t parameter_index,
            std::size_t station_index, std::size_t time_index) const override;
    virtual void setValue(double val, std::size_t parameter_index,
            std::size_t station_index, std::size_t time_index) override;

    virtual void subset(Observations&) const override;
    virtual void subset(const Parameters &, const Stations &, const Times &, Observations &) const override;


    virtual void print(std::ostream &) const override;
    friend std::ostream & operator<<(std::ostream &, const ObservationsPointer &);

    static const size_t _DIM_PARAMETER;
    static const size_t _DIM_STATION;
    static const size_t _DIM_TIME;

protected:
    vector3 dims_;
    double * data_;

    /**
     * This variable is used to keep track of whether the data memory is 
     * allocated internally and whether the pointer should be deleted during
     * the de-constructor.
     */
    bool allocated_;

    size_t toIndex_(size_t dim0, size_t dim1, size_t dim2) const;

    void allocateMemory_();

    void subset_data_(const Parameters &, const Stations &, const Times &, Observations &) const;
    void subset_data_(const std::vector<std::size_t>&,
            const std::vector<std::size_t>&,
            const std::vector<std::size_t>&, double*) const;
};

#endif /* OBSERVATIONSPOINTER_H */

