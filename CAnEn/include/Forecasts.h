/* 
 * File:   Forecasts.h
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <cervone@psu.edu>
 *
 * Created on April 18, 2018, 12:40 AM
 */

#ifndef FORECASTS_H
#define FORECASTS_H

#include "BasicData.h"
#include "Array4D.h"

/**
 * \class Forecasts
 * 
 * \brief Forecasts is an abstract class that extends BasicData and the abstract
 * class Array4D. It defines the interface of how to interact with the underlying
 * data storage through the abstract class Array4D. This interface is accepted
 * by the Analog Ensemble algorithm.
 */
class Forecasts : virtual public Array4D, public BasicData {
public:
    Forecasts();
    Forecasts(Forecasts const & orig);
    Forecasts(const Parameters &, const Stations &,
            const Times &, const Times &);
    
    virtual ~Forecasts();

    /**************************************************************************
     *                          Pure Virtual Functions                        *
     **************************************************************************/
    
    /**
     * Sets the dimensions of forecasts and allocates memory for data values.
     * @param parameters The Parameters container.
     * @param stations The Stations container.
     * @param times The Times container.
     * @param flts The FLTs container.
     */
    virtual void setDimensions(const Parameters & parameters,
            const Stations & stations, const Times & times,
            const Times & flts) = 0;
    
    virtual void subset(Forecasts & forecasts_subset) const = 0;
    

    /**************************************************************************
     *                           Member Functions                             *
     **************************************************************************/
    
    Times const & getFLTs() const;
    Times & getFLTs();
    
    std::size_t getFltTimeStamp(std::size_t index) const;
    std::size_t getFltTimeIndex(std::size_t timestamp) const;
    std::size_t getFltTimeIndex(const Time &) const;
    
    virtual void print(std::ostream &) const;
    friend std::ostream& operator<<(std::ostream&, Forecasts const &);

protected:
    Times flts_;
};

#endif /* FORECASTS_H */

