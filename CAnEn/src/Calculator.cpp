/* 
 * File:   Calculator.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 * 
 * Created on January 17, 2020, 9:17 AM
 */

#include "Calculator.h"

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#include <math.h>
#endif

#include <stdexcept>

using namespace std;

static const double _DEFAULT_CIRCULAR = false;
static const double _DEFAULT_MEAN_SIN = 0;
static const double _DEFAULT_MEAN_COS = 0;
static const double _DEFAULT_MEAN_LINEAR = 0;
static const double _DEFAULT_S = 0;

const double Calculator::_DEG2RAD = M_PI / 180;
const double Calculator::_RAD2DEG = 180 / M_PI;

/*
 *  This is the best estimator that Yamartino has found
 *               2 / sqrt(3) - 1 = 0.1547
 */
static const double _YAMARTINO = 0.1547;

Calculator::Calculator() : 
circular_(_DEFAULT_CIRCULAR), mean_sin_(_DEFAULT_MEAN_SIN),
mean_cos_(_DEFAULT_MEAN_COS), mean_linear_(_DEFAULT_MEAN_LINEAR),
S_(_DEFAULT_S) {
}

Calculator::Calculator(bool circular) :
circular_(circular), mean_sin_(_DEFAULT_MEAN_SIN),
mean_cos_(_DEFAULT_MEAN_COS), mean_linear_(_DEFAULT_MEAN_LINEAR),
S_(_DEFAULT_S) {

}

Calculator::Calculator(bool circular, size_t size) :
circular_(circular), mean_sin_(_DEFAULT_MEAN_SIN),
mean_cos_(_DEFAULT_MEAN_COS), mean_linear_(_DEFAULT_MEAN_LINEAR),
S_(_DEFAULT_S) {
    reserve(size);
}

Calculator::Calculator(const Calculator& orig) {
    if (this != &orig) {
        circular_ = orig.circular_;
        mean_sin_ = orig.mean_sin_;
        mean_cos_ = orig.mean_cos_;
        mean_linear_ = orig.mean_linear_;
        S_ = orig.S_;
        values_ = orig.values_;
    }
}

Calculator::~Calculator() {
}

void
Calculator::setCircular(bool circular) {
    circular_ = circular;
    return;
}

bool
Calculator::isCircular() const {
    return circular_;
}

void
Calculator::reserve(size_t size) {
    /*
     * Reserve one element larger than the desired size to prevent reallocation
     * 
     * Please see the example here:
     * http://www.cplusplus.com/reference/vector/vector/reserve/
     */
    values_.reserve(size + 1);
    return;
}

void
Calculator::pushValue(double value) {
    
    // Push the value into the vector
    values_.push_back(value);
    
    // Update member values
    if (circular_) {
        
        // Calculate the running average of sine and cosine
        value *= _DEG2RAD;
        mean_sin_ += (sin(value) - mean_sin_) / values_.size();
        mean_cos_ += (cos(value) - mean_cos_) / values_.size();

    } else {
        
        // Calculate the linear running average
        double tmp = mean_linear_ + (value - mean_linear_) / values_.size();
        
        // Calculate the linear running multiplier
        S_ += (value - tmp) * (value - mean_linear_);
        
        // Update the running average member variable
        mean_linear_ = tmp;
    }
    
    return;
}

void
Calculator::clear() {
    values_.clear();
    circular_ = _DEFAULT_CIRCULAR;
    mean_sin_ = _DEFAULT_MEAN_SIN;
    mean_cos_ = _DEFAULT_MEAN_COS;
    mean_linear_ = _DEFAULT_MEAN_LINEAR;
    S_ = _DEFAULT_S;
    return;
}


double
Calculator::mean() {
    if (values_.size() == 0) return NAN;
    if (circular_) return atan2(mean_sin_, mean_cos_) * _RAD2DEG;
    else return mean_linear_;
}

double
Calculator::variance() {
    if (values_.size() < 2) return NAN;
    if (circular_) throw runtime_error("Circular variance not implemented");
    else return S_ / (values_.size() - 1);
}

double
Calculator::sd() {
    if (values_.size() < 2) return NAN;
    if (circular_) return estimatorYamartino_();
    else return sqrt(variance());
}

double
Calculator::estimatorYamartino_() {
    double e = sqrt(1.0 - (pow(mean_sin_, 2.0) + pow(mean_cos_, 2.0)));
    double asine = asin(e);
    double ex3 = pow(e, 3);
    double sd = asine * (1 + _YAMARTINO * ex3);
    return (sd * _RAD2DEG);
}