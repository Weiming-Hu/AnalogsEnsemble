/* 
 * File:   AnEnReadGribMPI.h
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on March, 2020, 11:10 AM
 */


#ifndef AnEnREADGRIBMPI_H
#define AnEnREADGRIBMPI_H

#include <mpi.h>
#include "AnEnReadGrib.h"

/**
 * \class AnEnReadGribMPI
 * 
 * \brief AnEnReadGribMPI provides the functionality to read the Grib2 format
 * in parallel with MPI.
 */
class AnEnReadGribMPI: public AnEnReadGrib {

public:
    AnEnReadGribMPI();
    AnEnReadGribMPI(const AnEnReadGribMPI& orig);
    AnEnReadGribMPI(Verbose master_verbose);
    AnEnReadGribMPI(Verbose master_verbose, Verbose worker_verbose);
    virtual ~AnEnReadGribMPI();

    /**
     * Read grib files in parallel. This function has the same signature as
     * AnEnReadGrib::readForecasts.
     *
     * Please note that, it is assumed that the executable is launched with
     * MPI launcher program, e.g. mpirun or mpiexec, with multiple processes. This function
     * assign different tasks to different processes based on the process ID,
     * but this function DOES NOT terminate any process. The developer is 
     * responsible to terminate any processes in the main function.
     *
     * @param forecasts Forecasts
     * @param grib_parameters Parameters to read
     * @param files Files to read
     * @param regex_str Regular expression to extract day, cycle, and flt information
     * @param unit_in_seconds The number of seconds associated with the unit of flt and cycle
     * @param delimited Whether the day string to be extracted are delimited
     * @param stations_index Stations index to read
     */
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

#endif /* AnEnREADGRIBMPI_H */

