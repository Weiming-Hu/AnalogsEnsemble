/* 
 * File:   ObservationsArray.h
 * Author: Weiming Hu (weiming@psu.edu)
 *
 * Created on May 4, 2018, 11:27 AM
 */
#ifndef OBSERVATIONSARRAY_H
#define OBSERVATIONSARRAY_H

#include "Observations.h"
#include "colorTexts.h"
#include <boost/multi_array.hpp>

using namespace std;


/**
 * \class Observations_array
 * 
 * \brief Observations_array class is an implementation of the class 
 * Observations. Data are stored using the boost::multi_array<double, 3> class.
 */
class ObservationsArray : public Observations {
public:
    
    ObservationsArray();
    ObservationsArray(const ObservationsArray& orig);
    ObservationsArray(Parameters, Stations, Times);

    virtual ~ObservationsArray();

        size_t size() const override;
    
    const double* getValuesPtr() const override;
    double* getValuesPtr() override;
    
    void setDimensions(const Parameters & parameters,
            const Stations & stations, const Times & times) override;
 
    double getValue(std::size_t parameter_index,
            std::size_t station_index, std::size_t time_index) const override;
       
    void setValue(double val, std::size_t parameter_index,
            std::size_t station_index, std::size_t time_index) override;
    
   
    void print(std::ostream&) const override;
    friend std::ostream& operator<<(std::ostream&, const ObservationsArray&);

private:
    /**
     * The private variable data_ is initialized with Fortran storage order,
     * a.k.a column-based order. This is because the values read from NetCDF
     * are in column-based order (boost::fortran_storage_order()).
     */
    boost::multi_array<double, 3> data_;
    
     void updateDataDims_(bool initialize_values = true);

};

#endif /* OBSERVATIONSARRAY_H */

