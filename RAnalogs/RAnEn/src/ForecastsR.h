/* 
 * File:   ForecastsR.h
 * Author: Weiming Hu <cervone@psu.edu>
 *
 * Created on January 21, 2020, 3:36 PM
 */

#ifndef FORECASTSR_H
#define FORECASTSR_H

#include "Forecasts.h"
#include "Offset.h"

class ForecastsR : public Forecasts {
public:
    ForecastsR();
    ForecastsR(const ForecastsR& orig);
    ForecastsR(SEXP sx_weights, SEXP sx_circulars,
            SEXP sx_times, SEXP sx_flts, SEXP sx_data);
    virtual ~ForecastsR();

    std::size_t num_elements() const override;

    const double* getValuesPtr() const override;
    double* getValuesPtr() override;

    void setDimensions(const Parameters & parameters,
            const Stations & stations, const Times & times,
            const Times & flts) override;

    double getValue(std::size_t parameter_index,
            std::size_t station_index, std::size_t time_index,
            std::size_t flt_index) const override;
    void setValue(double val, std::size_t parameter_index,
            std::size_t station_index, std::size_t time_index,
            std::size_t flt_index) override;

    void print(std::ostream &) const;
    friend std::ostream & operator<<(std::ostream &, const ForecastsR &);

protected:
    Offset offset_;
    double * data_;
    
    /**
     * This variable is used to keep track of whether the data memory is 
     * allocated internally and whether the pointer should be deleted during
     * the de-constructor.
     */
    bool internal_;
    
};

#endif /* FORECASTSR_H */

