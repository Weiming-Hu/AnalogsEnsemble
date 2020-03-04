/* 
 * File:   AnEnReadNcdf.h
 * Author: Weiming Hu (weiming@psu.edu)
 *
 * Created on December 30, 2019, 11:10 AM
 */

#ifndef ANENREADNCDF_H
#define ANENREADNCDF_H

#include <vector>
#include <netcdf>

#include "Config.h"
#include "Forecasts.h"
#include "Observations.h"

/**
 * \class AnEnReadNcdf
 * 
 * \brief AnEnRead provides the functionality to read the NetCDF format.
 * The recognized NetCDF file structure is below
 * 
 * https://weiming-hu.github.io/AnalogsEnsemble/2019/01/16/NetCDF-File-Types.html
 */
class AnEnReadNcdf {
public:

    enum class FileType {
        Forecasts, Observations
    };

    AnEnReadNcdf();
    AnEnReadNcdf(Verbose verbose);
    AnEnReadNcdf(const AnEnReadNcdf& orig);
    virtual ~AnEnReadNcdf();

    void readForecasts(const std::string & file_path,
            Forecasts & forecasts) const;
    void readForecasts(
            const std::string & file_path, Forecasts & forecasts,
            std::vector<size_t> start, std::vector<size_t> count) const;

    void readObservations(const std::string & file_path,
            Observations & observations) const;
    void readObservations(
            const std::string & file_path, Observations & observations,
            std::vector<size_t> start, std::vector<size_t> count) const;
    
    /**
     * Read different components from an NetCDF group object
     * @param nc NcGroup to read
     * @param parameters Parameters to store results
     * @param stations Stations to store results
     * @param times Times to store results
     * @param var_name Variable names to read for times
     * @param start Index to start reading
     * @param count Total numbers to read
     */
    void read(const netCDF::NcGroup & nc, Parameters & parameters,
            size_t start = 0, size_t count = 0) const;
    void read(const netCDF::NcGroup & nc, Stations & stations,
            size_t start = 0, size_t count = 0,
            const std::string & dim_name_prefix = "",
            const std::string & var_name_prefix = "") const;
    void read(const netCDF::NcGroup & nc, Times & times,
            const std::string & var_name,
            size_t start = 0, size_t count = 0) const;

    
    /**************************************************************************
     *                          Template Functions                            *
     **************************************************************************/

    template <typename T>
    void read(const netCDF::NcGroup & nc, T * p_vals,
            const std::string & var_name = Config::_DATA,
            std::vector<size_t> start = {},
    std::vector<size_t> count = {}) const;

protected:
    Verbose verbose_;

    void checkFileType_(const netCDF::NcFile & nc, FileType file_type) const;
};

// Template functions are defined in the following file
#include "AnEnReadNcdf.tpp"

#endif /* ANENREADNCDF_H */