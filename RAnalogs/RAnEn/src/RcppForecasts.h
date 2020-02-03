/* 
 * File:   RcppForecasts.h
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on January 21, 2020, 3:36 PM
 */

#ifndef FORECASTSR_H
#define FORECASTSR_H

#include "ForecastsPointer.h"
#include <Rcpp.h>

// [[Rcpp::plugins(cpp11)]]

class ForecastsR : public ForecastsPointer {
public:
    ForecastsR() = delete;
    ForecastsR(const ForecastsR& orig) = delete;
    
    ForecastsR(SEXP sx_data, SEXP sx_parameters_name, SEXP sx_circulars,
            SEXP sx_xs, SEXP sx_ys, SEXP sx_stations_name, SEXP sx_times, SEXP sx_flts);

    virtual ~ForecastsR();
    
    void show() const;
};

#endif /* FORECASTSR_H */

