/* 
 * File:   ObservationsPointer.h
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <cervone@psu.edu>
 *
 * Created on January 27, 2020, 5:39 PM
 */

#ifndef FORECASTSPOINTER_H
#define FORECASTSPOINTER_H

#include "Forecasts.h"
#include "Array4DPointer.h"

class ForecastsPointer : public Forecasts {
public:
    ForecastsPointer();
    ForecastsPointer(const ForecastsPointer& orig);
    ForecastsPointer(const Parameters &, const Stations &, const Times &, const Times &);
    virtual ~ForecastsPointer();

    std::size_t num_elements() const override;

    const double* getValuesPtr() const override;
    double * getValuesPtr() override;

    void setDimensions(
            const Parameters & parameters, const Stations & stations,
            const Times & times, const Times & flts) override;

    double getValue(
            std::size_t parameter_index, std::size_t station_index,
            std::size_t time_index, std::size_t flt_index) const override;
    void setValue(double val,
            std::size_t parameter_index, std::size_t station_index,
            std::size_t time_index, std::size_t flt_index) override;

    void print(std::ostream &) const;
    friend std::ostream & operator<<(std::ostream &, const ForecastsPointer &);

    static const size_t _DIM_PARAMETER;
    static const size_t _DIM_STATION;
    static const size_t _DIM_TIME;
    static const size_t _DIM_FLT;

protected:
    Array4DPointer data_;
};

#endif /* FORECASTSPOINTER_H */

