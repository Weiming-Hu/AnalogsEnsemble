/* 
 * File:   ObservationsPointer.h
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <cervone@psu.edu>
 *
 * Created on January 27, 2020, 5:39 PM
 */

#ifndef OBSERVATIONSPOINTER_H
#define OBSERVATIONSPOINTER_H

#include "Observations.h"

using vector3 = size_t[3];

class ObservationsPointer : public Observations {
public:
    ObservationsPointer();
    ObservationsPointer(const ObservationsPointer& orig);
    virtual ~ObservationsPointer();
    
    std::size_t num_elements() const override;

    const double* getValuesPtr() const override;
    double * getValuesPtr() override;

    void setDimensions(const Parameters & parameters,
            const Stations & stations, const Times & times) override;

    double getValue(std::size_t parameter_index,
            std::size_t station_index, std::size_t time_index) const override;
    void setValue(double val, std::size_t parameter_index,
            std::size_t station_index, std::size_t time_index) override;
    
    size_t toIndex(const vector3 & indices) const;
    
    void print(std::ostream &) const;
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
};

#endif /* OBSERVATIONSPOINTER_H */

