/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   AnEnSSE.h
 * Author: guido
 *
 * Created on January 29, 2020, 5:08 PM
 */


#ifndef ANENSSE_H
#define ANENSSE_H

#include "AnEnIS.h"

using namespace boost::numeric::ublas;
 
class AnEnSSE : public AnEnIS {
public:
    AnEnSSE();
    AnEnSSE(const AnEnSSE& orig);
    virtual ~AnEnSSE();

    virtual void compute(const Forecasts & forecasts,
            const Observations & observations,
            std::vector<std::size_t> & fcsts_test_index,
            std::vector<std::size_t> & fcsts_search_index) override;

    const Array4D & getSimsStation() {
        return sims_station_;
    }

    AnEnSSE & operator=(const AnEnSSE & rhs);

protected:

    virtual void preprocess_(const Forecasts & forecasts,
            const Observations & observations,
            std::vector<std::size_t> & fcsts_test_index,
            std::vector<std::size_t> & fcsts_search_index) override;

    Array4D sims_station_;
    matrix<size_t>  search_stations_;

};

#endif /* ANENSSE_H */

