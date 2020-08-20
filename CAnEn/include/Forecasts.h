/* 
 * File:   Forecasts.h
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <cervone@psu.edu>
 *
 * Created on April 18, 2018, 12:40 AM
 */

#ifndef FORECASTS_H
#define FORECASTS_H

#include "Config.h"
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
    
    /**
     * In some forecast model output, wind components U and V are available, but
     * not wind speed and direction. This function is used to calculate wind
     * speed and direction from U and V components.
     *
     * Note that U will be replaced by wind speed and V will be replaced by
     * wind direction.
     *
     * @param name_u Parameter name for U component of wind
     * @param name_v Parameter name for V component of wind
     * @param name_spd Parameter name for wind speed
     * @param name_dir Parameter name for wind direction
     */
    virtual void windTransform(
            const std::string & name_u, const std::string & name_v,
            const std::string & name_spd, const std::string & name_dir) = 0;
    
    /*
     * Subset Forecasts. Dimensions to be extracted are determined from the function input Forecasts.
     * The input Forecasts should have already allocated enough memory by calling setDimensions prior
     * to this function cal.
     */
    virtual void subset(Forecasts & forecasts_subset) const = 0;

    /*
     * Subset Forecasts. Dimensions are specified by the input arguments. The input Forecasts
     * shoudl be an empty Forecasts that will hold all subset information and values at the end of
     * this function call.
     */
    virtual void subset(const Parameters &, const Stations &, const Times&, const Times&, Forecasts &) const = 0;

    /**************************************************************************
     *                           Member Functions                             *
     **************************************************************************/
    
    Times const & getFLTs() const;
    Times & getFLTs();
    
    std::size_t getFltTimeStamp(std::size_t index) const;
    std::size_t getFltTimeIndex(std::size_t timestamp) const;
    std::size_t getFltTimeIndex(const Time &) const;

#if defined(_ENABLE_AI)
    virtual void featureTransform(const std::string & embedding_model_path, Verbose verbose = Verbose::Warning);
#endif
    
    virtual void print(std::ostream &) const;
    friend std::ostream& operator<<(std::ostream&, Forecasts const &);

protected:
    Times flts_;
};

#endif /* FORECASTS_H */

