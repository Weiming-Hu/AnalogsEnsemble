/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ObservationsPointer.h
 * Author: guido
 *
 * Created on January 27, 2020, 5:39 PM
 */

#ifndef OBSERVATIONSPOINTER_H
#define OBSERVATIONSPOINTER_H

#include "Observations.h"

typedef size_t vector3[3];

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

