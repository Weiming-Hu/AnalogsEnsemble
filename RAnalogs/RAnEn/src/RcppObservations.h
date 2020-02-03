/* 
 * File:   RcppObservations.h
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on January 21, 2020, 3:37 PM
 */

#ifndef OBSERVATIONSR_H
#define OBSERVATIONSR_H

#include "ObservationsPointer.h"
#include <Rcpp.h>

class ObservationsR : public ObservationsPointer {
public:
    ObservationsR() = delete;
    ObservationsR(const ObservationsR &) = delete;
    
    ObservationsR(SEXP sx_data, SEXP sx_names, SEXP sx_times);

    virtual ~ObservationsR();

    void print(std::ostream &) const;
    friend std::ostream & operator<<(std::ostream &, const ObservationsR &);
};

#endif /* OBSERVATIONSR_H */

