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

BasicData::BasicData(const Parameters & parameters, const Stations & stations,
        const Times & times) {
    parameters_ = parameters;
    stations_ = stations;
    times_ = times;
}

BasicData::BasicData(const BasicData& orig) {
    if (this != &orig) {
        parameters_ = orig.parameters_;
        stations_ = orig.stations_;
        times_ = orig.times_;
    }
}

BasicData::~BasicData() {
}

const Parameters &
BasicData::getParameters() const {
    return parameters_;
}

const Stations &
BasicData::getStations() const {
    return stations_;
}

const Times &
BasicData::getTimes() const {
    return times_;
}

Parameters &
BasicData::getParameters() {
    return parameters_;
}

Stations &
BasicData::getStations() {
    return stations_;
}

Times &
BasicData::getTimes() {
    return times_;
}