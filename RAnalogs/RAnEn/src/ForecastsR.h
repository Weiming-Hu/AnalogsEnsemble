/* 
 * File:   ForecastsR.h
 * Author: Weiming Hu <cervone@psu.edu>
 *
 * Created on January 21, 2020, 3:36 PM
 */

#ifndef FORECASTSR_H
#define FORECASTSR_H

#include "ForecastsPointer.h"
#include <Rcpp.h>

class ForecastsR : public ForecastsPointer {
public:
    ForecastsR() = delete;
    ForecastsR(const ForecastsR& orig) = delete;
    
    ForecastsR(SEXP sx_weights, SEXP sx_circulars,
            SEXP sx_times, SEXP sx_flts, SEXP sx_data);
    virtual ~ForecastsR();
    
    void print(std::ostream &) const;
    friend std::ostream & operator<<(std::ostream &, const ForecastsR &);
};

#endif /* FORECASTSR_H */

