/* 
 * File:   Normalization.h
 * Author: Weiming Hu <cervone@psu.edu>
 * 
 * Created on January 14, 2020, 1:36 PM
 */

#ifndef NORMALIZATION_H
#define NORMALIZATION_H

#include "Functions.h"

class Normalization {
public:
    Normalization();
    Normalization(const Normalization& orig);
    virtual ~Normalization();
    
    double fixedSdLinear();
    double accumSdLinear();
    
    double fixedSdCircular();
    double accumSdCircular();
    
private:
    bool operational_;
    
    double M_;
    double S_

};

#endif /* NORMALIZATION_H */

