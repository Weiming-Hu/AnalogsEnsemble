/* 
 * File:   ObservationsR.h
 * Author: Weiming Hu <cervone@psu.edu>
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
    ObservationsR(SEXP sx_times, SEXP sx_data);
    virtual ~ObservationsR();
};

#endif /* OBSERVATIONSR_H */

