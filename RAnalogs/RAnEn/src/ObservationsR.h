/* 
 * File:   ObservationsR.h
 * Author: Weiming Hu <cervone@psu.edu>
 *
 * Created on January 21, 2020, 3:37 PM
 */

#ifndef OBSERVATIONSR_H
#define OBSERVATIONSR_H

#include "Observations.h"
#include "Offset.h"

class ObservationsR : public Observations {
public:
    ObservationsR();
    ObservationsR(const ObservationsR& orig);
    ObservationsR(SEXP sx_times, SEXP sx_data);
    virtual ~ObservationsR();

    std::size_t num_elements() const override;

    const double* getValuesPtr() const override;
    double * getValuesPtr() override;

    void setDimensions(const Parameters & parameters,
            const Stations & stations, const Times & times);

    double getValue(std::size_t parameter_index,
            std::size_t station_index, std::size_t time_index) const;
    void setValue(double val, std::size_t parameter_index,
            std::size_t station_index, std::size_t time_index);

protected:
    Offset offset_;
    Rcpp::NumericVector data_;

};

#endif /* OBSERVATIONSR_H */

