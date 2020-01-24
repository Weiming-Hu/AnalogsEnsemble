/* 
 * File:   Calculator.h
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on January 17, 2020, 9:17 AM
 */

#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <vector>
#include <stdlib.h>

/**
 * Calculator is designed for vector arithmetics, including linear and circular
 * variables like wind directions.
 * 
 * For circular variables, input and output are both in degrees. The conversion
 * between degrees and radiance is carried out internally.
 */
class Calculator {
public:
    Calculator();
    Calculator(bool);
    Calculator(bool, std::size_t);
    Calculator(const Calculator& orig);
    virtual ~Calculator();
    
    void setCircular(bool);
    bool isCircular() const;
    
    void reserve(std::size_t);
    void pushValue(double);
    void clearValues();
    void reset();
    
    double mean();
    double variance();
    double sd();

    static const double _DEG2RAD;
    static const double _RAD2DEG;
    
protected:
    bool circular_;
    double mean_sin_;
    double mean_cos_;
    double mean_linear_;
    double S_;
    std::vector<double> values_;
    
    double estimatorYamartino_();
};

#endif /* CALCULATOR_H */

