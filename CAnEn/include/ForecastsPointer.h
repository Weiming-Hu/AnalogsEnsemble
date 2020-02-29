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

/**
 * \class ForecastsPointer
 * 
 * \brief ForecastsPointer is an implementation of the abstract class
 * Forecasts. It also inherits from Array4DPointer so that the underlying
 * data structure is handled by a pointer which is optimized for performance.
 */
class ForecastsPointer : virtual public Forecasts, virtual public Array4DPointer {
public:
    ForecastsPointer();
    ForecastsPointer(const ForecastsPointer& orig);
    ForecastsPointer(const Parameters &, const Stations &, const Times &, const Times &);
    virtual ~ForecastsPointer();

    virtual void setDimensions(
            const Parameters & parameters, const Stations & stations,
            const Times & times, const Times & flts) override;
    
    virtual void windTransform(
            const std::string & name_u, const std::string & name_v,
            const std::string & name_spd, const std::string & name_dir) override;
    
    virtual void subset(Forecasts& forecasts_subset) const override;

    virtual void print(std::ostream &) const override;
    friend std::ostream & operator<<(std::ostream &, const ForecastsPointer &);

    static const size_t _DIM_PARAMETER;
    static const size_t _DIM_STATION;
    static const size_t _DIM_TIME;
    static const size_t _DIM_FLT;
};

#endif /* FORECASTSPOINTER_H */

