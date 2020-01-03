/* 
 * File:   ForecastsArray.cpp
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <cervone@psu.edu>
 * 
 * Created on January 2, 2020, 5:55 PM
 */

#include "ForecastsArray.h"

using namespace std;

ForecastsArray::ForecastsArray() {
    data_ = Array4D(boost::extents[0][0][0][0],
            boost::fortran_storage_order());
}

ForecastsArray::ForecastsArray(const Parameters & parameters,
        const Stations & stations, const Times & times, const FLTs & flts) :
Forecasts(parameters, stations, times, flts){
    data_ = Array4D(boost::extents[0][0][0][0],
            boost::fortran_storage_order());
}

ForecastsArray::ForecastsArray(const ForecastsArray & orig) : Forecasts(orig) {
    if (this != &orig) {
        data_ = orig.data_;
    }
}

ForecastsArray::~ForecastsArray() {
}

