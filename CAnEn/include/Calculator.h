/* 
 * File:   Calculator.h
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on January 17, 2020, 9:17 AM
 */

#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <vector>
#include <cstddef>

/**
 * \class Calculator
 * 
 * \brief Calculator is designed for vector arithmetics, including linear and
 * circular variables like wind directions. For circular variables, input
 * and output are both in degrees. The conversion between degrees and radiance
 * is carried out internally.
 */
class Calculator {
public:
    Calculator();
    Calculator(bool);
    Calculator(bool, std::size_t);
    Calculator(const Calculator& orig);
    virtual ~Calculator();
    
    /**
     * Change the circular status of the variable we are calculating.
     * @param A boolean.
     */
    void setCircular(bool);
    bool isCircular() const;
    
    /**
     * Reserve space inside the calculator. This will make adding numbers to
     * the calculator sequence in constant time.
     * @param The size of the space to be reserved
     */
    void reserve(std::size_t);
    
    /**
     * Add a value into the calculation queue
     * @param A double value
     */
    void pushValue(double);
    
    /**
     * Clear all values in the calculator.
     */
    void clearValues();
    
    /**
     * Clear all values and reset the circular status.
     */
    void reset();
    
    /**
     * Get the running average of all pushed values.
     * @return An average
     */
    double mean();
    
    /**
     * Get the running variance of all pushed values.
     * @return A variance
     */
    double variance();
    
    /**
     * Get the running standard deviation of all pushed values
     * @return A standard deviation
     */
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

