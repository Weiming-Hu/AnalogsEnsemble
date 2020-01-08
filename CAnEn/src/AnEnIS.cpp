/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   AnEnIS.cpp
 * Author: guido
 * 
 * Created on January 8, 2020, 11:40 AM
 */

#include "AnEnIS.h"

AnEnIS::AnEnIS() {
}

AnEnIS::AnEnIS(const AnEnIS& orig) : AnEn(orig) {
}

AnEnIS::~AnEnIS() {
}

void
AnEnIS::compute(const Forecasts & forecasts, const Observations & observations,
        const Times & test_times, const Times & search_times) const {
    return;
}

