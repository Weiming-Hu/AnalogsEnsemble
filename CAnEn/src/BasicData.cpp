/* 
 * File:   BasicData.cpp
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <weiming@psu.edu>
 * 
 * Created on January 2, 2020, 3:18 PM
 */

#include "BasicData.h"

BasicData::BasicData() {
}

BasicData::BasicData(const BasicData& orig) {
}

BasicData::~BasicData() {
}

anenPar::Parameters const &
Forecasts::getParameters() const {
    return parameters_;
}

anenSta::Stations const &
Forecasts::getStations() const {
    return stations_;
}

anenTime::Times const &
Forecasts::getTimes() const {
    return times_;
}

anenTime::FLTs const &
Forecasts::getFLTs() const {
    return flts_;
}

anenPar::Parameters &
Forecasts::getParameters() {
    return parameters_;
}

anenSta::Stations &
Forecasts::getStations() {
    return stations_;
}

anenTime::Times &
Forecasts::getTimes() {
    return times_;
}