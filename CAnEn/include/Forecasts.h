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

#if defined(_ENABLE_AI)
#include <ATen/ATen.h>
#include <torch/script.h>
#endif

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
    /**
     * This function is available when ENABLE_AI is turned on during the program compilation. This
     * function converts the original forecast variables into latent features by using a pretrained
     * AI representation model. 
     *
     * @param embedding_model_path The PyTorch model
     * @param verbose Verbose level
     * @param lead_time_radius The number of nearby lead times to use. This is only used when your AI
     * model allows for temporal representation.
     */
    virtual void featureTransform(const std::string & embedding_model_path, Verbose verbose = Verbose::Warning, long int lead_time_radius = 1);
#endif

    Forecasts & operator=(const Forecasts &);
    
    virtual void print(std::ostream &) const;
    friend std::ostream& operator<<(std::ostream&, Forecasts const &);

protected:
    Times flts_;

#if defined(_ENABLE_AI)
    virtual void featureTransform_1D_(at::Tensor & output, torch::jit::script::Module & module,
            long int num_parameters, long int num_stations, long int num_times, long int num_lead_times, Verbose verbose);
    virtual void featureTransform_2D_(at::Tensor & output, torch::jit::script::Module & module, long int flt_radius,
            long int num_parameters, long int num_stations, long int num_times, long int num_lead_times, Verbose verbose);
#endif

};

#endif /* FORECASTS_H */

