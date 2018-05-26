/* 
 * File:   AnEnIO.h
 * Author: Weiming Hu (weiming@psu.edu)
 *
 * Created on May 4, 2018, 11:16 AM
 */

#ifndef ANENIO_H
#define ANENIO_H

#include <netcdf>
#include <string>

#include "Times.h"
#include "Stations.h"
#include "Observations.h"
#include "Forecasts.h"

class AnEnIO {
public:
    AnEnIO();
    AnEnIO(const AnEnIO& orig) = delete;
    virtual ~AnEnIO();

    static int checkForecasts(std::string);
    static int checkObservations(std::string);

    static int readForecasts(std::string, Forecasts&);
    static int readObservations(std::string, Observations&);

    static int readData(std::string, Times &);
    static int readParameters(std::string, Parameters &);
    static int readStations(std::string, Stations &);
    static int readTimes(std::string, Times &);

    static void handleError(int);

protected:

    enum errorType {
        SUCCESS = 0,
        FILE_NOT_FOUND = -1,
        VAR_NAME_NOT_FOUND = -2,
        WRONG_TYPE = -3,
        INSUFFICIENT_MEMORY = -50,
        UNKNOWN_ERROR = -100
    };

    int open_file_(std::string, netCDF::NcFile &) const;

    int check_variables_exist_(std::string, netCDF::NcFile &) const;
    int check_variables_shape_(std::string, netCDF::NcFile &) const;


    /************************************************************************
     *                      Template Functions                              *
     ************************************************************************/

    // Read variables as a vector. The variable to be read should be either one
    // of the following cases:
    //     1. The variable has only one dimensions;
    //     2. The variable has two dimensions, and the variable type is char.
    //        So this variable can be converted to a vector of string;
    //

    template<typename T>
    int read_vector_(netCDF::NcFile nc, netCDF::NcVar,
            std::vector<T> & vector) const {
        return (0);
    }

};

#endif /* ANENIO_H */

