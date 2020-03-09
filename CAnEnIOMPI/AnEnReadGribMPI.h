/* 
 * File:   AnEnReadGribMPI.h
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on March, 2020, 11:10 AM
 */


#ifndef ANENTREADGRIBMPI_H
#define ANENTREADGRIBMPI_H

#include <mpi.h>
#include "AnEnReadGrib.h"

class AnEnReadGribMPI: public AnEnReadGrib {

public:
    AnEnReadGribMPI();
    AnEnReadGribMPI(const AnEnReadGribMPI& orig);
    AnEnReadGribMPI(Verbose master_verbose);
    AnEnReadGribMPI(Verbose master_verbose, Verbose worker_verbose);
    virtual ~AnEnReadGribMPI();

    void readForecasts(Forecasts & forecasts,
            const std::vector<ParameterGrib> & grib_parameters,
            const std::vector<std::string> & files,
            const std::string & regex_str,
            unsigned long unit_in_seconds = 3600,
            bool delimited = false,
            std::vector<int> stations_index = {}) const;

private:
    Verbose worker_verbose_;
};

#endif /* ANENTREADGRIBMPI_H */
