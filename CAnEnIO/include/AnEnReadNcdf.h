/* 
 * File:   AnEnReadNcdf.h
 * Author: Weiming Hu (weiming@psu.edu)
 *
 * Created on December 30, 2019, 11:10 AM
 */

#ifndef ANENREADNCDF_H
#define ANENREADNCDF_H

#include <array>
#include <vector>
#include <netcdf>
#include <stdexcept>

#include "AnEnRead.h"
#include "Analogs.h"
#include "colorTexts.h"
#include "SimilarityMatrices.h"
#include "StandardDeviation.h"

/**
 * \class AnEnReadNcdf
 * 
 * \brief AnEnRead derives from AnEnRead and it provides the functionality to
 * read the NetCDF format. The recognized NetCDF file structure is specified below
 * 
 * https://weiming-hu.github.io/AnalogsEnsemble/2019/01/16/NetCDF-File-Types.html
 */
class AnEnReadNcdf : public AnEnRead {
public:

    enum class FileType {
        Forecasts, Observations, Similarity,
        Analogs, StandardDeviation
    };

    // A higher level will contain all messages from the lower levels. For
    // example, The progress level will contain errors and warnings.
    //

    enum class Verbose {
        Silence = 0, Error = 1, Warning = 2,
        Progress = 3, Detail = 4, Debug = 5
    };

    AnEnReadNcdf();
    AnEnReadNcdf(Verbose verbose);
    AnEnReadNcdf(const AnEnReadNcdf& orig);
    virtual ~AnEnReadNcdf();

    void readForecasts(
            const std::string & file_path,
            Forecasts_array & forecasts) const override;
    // TODO
    void readObservations(
            const std::string & file_path,
            Observations_array & observations) const override;

protected:
    Verbose verbose_;

    void read_(const netCDF::NcFile & nc, anenPar::Parameters & parameters,
            size_t start = 0, size_t count = 0) const;
    void read_(const netCDF::NcFile & nc, anenSta::Stations & stations,
            size_t start = 0, size_t count = 0,
            const std::string & dim_name_prefix = "",
            const std::string & var_name_prefix = "") const;
    void read_(const netCDF::NcFile & nc, anenTime::Times & times,
            size_t start = 0, size_t count = 0,
            const std::string & var_name = "Times") const;
    void read_(const netCDF::NcFile & nc, anenTime::FLTs & flts,
            size_t start = 0, size_t count = 0) const;
    void read_(Forecasts_array & forecasts) const;
    void read_(Forecasts_array & forecasts,
            std::array<size_t, 4> start, std::array<size_t, 4> count) const;

    void checkFileType_(const netCDF::NcFile & nc, FileType file_type) const;

    void fastInsert_(
            anenPar::Parameters & parameters, size_t dim_len,
            const std::vector<std::string> & names,
            std::vector<std::string> & circulars,
            const std::vector<double> & weights) const;
    void fastInsert_(
            anenSta::Stations & stations, size_t dim_len,
            const std::vector<std::string> & names,
            const std::vector<double> & xs,
            const std::vector<double> & ys) const;
};

#endif /* ANENREADNCDF_H */