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

#include "AnEnRead.h"
#include "Analogs.h"
#include "AnEnDefaults.h"

/**
 * \class AnEnReadNcdf
 * 
 * \brief AnEnRead derives from AnEnRead and it provides the functionality to
 * read the NetCDF format. The recognized NetCDF file structure is below
 * 
 * https://weiming-hu.github.io/AnalogsEnsemble/2019/01/16/NetCDF-File-Types.html
 */
class AnEnReadNcdf : public AnEnRead {
public:

    enum class FileType {
        Forecasts, Observations, Similarity,
        Analogs, StandardDeviation
    };

    AnEnReadNcdf();
    AnEnReadNcdf(AnEnDefaults::Verbose verbose);
    AnEnReadNcdf(const AnEnReadNcdf& orig);
    virtual ~AnEnReadNcdf();

    void readForecasts(const std::string & file_path,
            Forecasts & forecasts) const override;
    void readForecasts(
            const std::string & file_path, Forecasts & forecasts,
            std::vector<size_t> start, std::vector<size_t> count) const;
    
    void readObservations(const std::string & file_path,
            Observations & observations) const override;
    void readObservations(
            const std::string & file_path, Observations & observations,
            std::vector<size_t> start, std::vector<size_t> count) const;
    
    void readAnalogs(
            const std::string & file_path, Analogs & analogs,
            std::vector<size_t> start = {},
            std::vector<size_t> count = {}) const;
    
    
protected:
    AnEnDefaults::Verbose verbose_;

    void read_(const netCDF::NcFile & nc, Parameters & parameters,
            size_t start = 0, size_t count = 0) const;
    void read_(const netCDF::NcFile & nc, Stations & stations,
            size_t start = 0, size_t count = 0,
            const std::string & dim_name_prefix = "",
            const std::string & var_name_prefix = "") const;
    void read_(const netCDF::NcFile & nc, Times & times,
            const std::string & var_name,
            size_t start = 0, size_t count = 0) const;
    
    void checkFileType_(const netCDF::NcFile & nc, FileType file_type) const;
    
    /**************************************************************************
     *                       Protected Template Functions                     *
     **************************************************************************/
    
    template <typename T>
    void read_(const netCDF::NcFile & nc, T * p_vals,
            const std::string & var_name = "Data",
            std::vector<size_t> start = {},
            std::vector<size_t> count = {}) const;
};

// Template functions are defined in the following file
#include "AnEnReadNcdf.tpp"

#endif /* ANENREADNCDF_H */