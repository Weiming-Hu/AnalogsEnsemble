/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   AnEnIS.h
 * Author: guido
 *
 * Created on January 8, 2020, 11:40 AM
 */

#ifndef ANENIS_H
#define ANENIS_H

#include "AnEn.h"

class AnEnIS : public AnEn {
public:
    //https://www.johndcook.com/blog/standard_deviation/
    AnEnIS();
    AnEnIS(const AnEnIS& orig);
    virtual ~AnEnIS();
    
    void compute(const Forecasts & forecasts, const Observations & observations,
            const Times & test_times, const Times & search_times) const override;
    
    protected:
        boost::multi_array<double, 4> SimilarityMetric_;
        boost::multi_array<double, 4> SimilarityIndex_;

        
};

#endif /* ANENIS_H */

