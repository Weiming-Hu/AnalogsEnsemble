/* 
 * File:   ForecastsArray.h
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <cervone@psu.edu>
 *
 * Created on January 2, 2020, 5:55 PM
 */

#ifndef FORECASTSARRAY_H
#define FORECASTSARRAY_H

#include "Forecasts.h"
#include <boost/multi_array.hpp>

/**
 * \class ForecastsArray
 * 
 * \brief ForecastsArray class is an child class of class Forecasts. Data 
 * are stored using the Array4D class.
 */
class ForecastsArray : public Forecasts {
public:
    ForecastsArray();
    ForecastsArray(const ForecastsArray& orig);
    ForecastsArray(const Parameters &, const Stations &,
            const Times &, const Times &);

    virtual ~ForecastsArray();

    size_t size() const override;

    const double* getValuesPtr() const override;
    double* getValuesPtr() override;

    void setDimensions(const Parameters &, const Stations &,
            const Times &, const Times &) override;

    double getValue(std::size_t parameter_index, std::size_t station_index,
            std::size_t time_index, std::size_t flt_index) const override;

    void setValue(double val, std::size_t parameter_index,
            std::size_t station_index, std::size_t time_index,
            std::size_t flt_index) override;

    void print(std::ostream &) const override;
    friend std::ostream& operator<<(std::ostream&, const ForecastsArray&);

protected:
    boost::multi_array<double, 4> data_;
    void updateDataDims_(bool initialize_values = true);
};

#endif /* FORECASTSARRAY_H */

