/* 
 * File:   AnEnReadNcdf.h
 * Author: Weiming Hu (weiming@psu.edu)
 *
 * Created on December 30, 2019, 11:10 AM
 */

#ifndef ANENREADNCDF_H
#define ANENREADNCDF_H

#include <array>
#include <netcdf>

#include "AnEnRead.h"
#include "Analogs.h"
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
        Forecasts, Observations, Similarity, Analogs, StandardDeviation
    };

    // A higher level will contain all messages from the lower levels. For
    // example, The progress level will contain errors and warnings.
    //

    enum class Verbose {
        Silence = 0, Error = 1, Warning = 2, Progress = 3, Detail = 4, Debug = 5
    };

    AnEnReadNcdf();
    AnEnReadNcdf(Verbose verbose);
    AnEnReadNcdf(const AnEnReadNcdf& orig);
    virtual ~AnEnReadNcdf();

    void readForecasts(
            const std::string & file_path,
            Forecasts_array & forecasts) const override;
    void readForecasts(
            const std::string & file_path,
            Forecasts_array & forecasts,
            std::array<size_t, 4> start,
            std::array<size_t, 4> count,
            std::array<ptrdiff_t, 4> stride = {1, 1, 1, 1}) const;

    void readObservations(
            const std::string & file_path,
            Observations_array & observations) const override;
    void readObservations(
            const std::string & file_path,
            Observations_array & observations,
            std::array<size_t, 3> start,
            std::array<size_t, 3> count,
            std::array<ptrdiff_t, 3> stride = {1, 1, 1}) const;

    //    void readSimilarityMatrices(
    //            const std::string & file_path,
    //            SimilarityMatrices & sims) const;
    //    void readSimilarityMatrices(
    //            const std::string & file_path,
    //            SimilarityMatrices & sims,
    //            std::vector<size_t> start,
    //            std::vector<size_t> count,
    //            std::vector<ptrdiff_t> stride = {1, 1, 1, 1, 1}) const;
    //
    //    void readAnalogs(
    //            const std::string & file_path,
    //            Analogs & analogs) const;
    //    void readAnalogs(
    //            const std::string & file_path,
    //            Analogs & analogs,
    //            std::vector<size_t> start,
    //            std::vector<size_t> count,
    //            std::vector<ptrdiff_t> stride = {1, 1, 1, 1, 1}) const;
    //
    //    void readStandardDeviation(
    //            const std::string & file_path,
    //            StandardDeviation & sds) const;
    //    void readStandardDeviation(
    //            const std::string & file_path,
    //            StandardDeviation & sds,
    //            std::vector<size_t> start,
    //            std::vector<size_t> count,
    //            std::vector<ptrdiff_t> stride = {1, 1, 1}) const;

protected:
    Verbose verbose_;

    /*
     * The name prefix for member dimensions in file type Analogs
     */
    const static std::string _MEMBER_DIM_PREFIX_;

    /*
     * The name prefix for member variables in file type Analogs
     */
    const static std::string _MEMBER_VAR_PREFIX_;

    /*
     * The name prefix for search dimensions in file type SimilarityMatrices
     */
    const static std::string _SEARCH_DIM_PREFIX_;

    /*
     * The name prefix for search variables in file type SimilarityMatrices
     */
    const static std::string _SEARCH_VAR_PREFIX_;

    void readParameters_(
            const netCDF::NcFile & nc, anenPar::Parameters & parameters) const;
    void readParameters_(
            const netCDF::NcFile & nc, anenPar::Parameters & parameters,
            size_t start, size_t count, ptrdiff_t stride = 1) const;

    void readStations_(
            const netCDF::NcFile & nc, anenSta::Stations & stations,
            const std::string & dim_name_prefix = "",
            const std::string & var_name_prefix = "") const;
    void readStations_(
            const netCDF::NcFile & nc, anenSta::Stations & stations,
            size_t start, size_t count, ptrdiff_t stride = 1,
            const std::string & dim_name_prefix = "",
            const std::string & var_name_prefix = "") const;

    void readTimes_(
            const netCDF::NcFile & nc, anenTime::Times & times,
            const std::string & var_name = "Times") const;
    void readTimes_(
            const netCDF::NcFile & nc, anenTime::Times & times,
            size_t start, size_t count, ptrdiff_t stride = 1,
            const std::string & var_name = "Times") const;

    void readFLTs_(const netCDF::NcFile & nc, anenTime::FLTs & flts) const;
    void readFLTs_(
            const netCDF::NcFile & nc, anenTime::FLTs & flts,
            size_t start, size_t count, ptrdiff_t stride = 1) const;

    void readForecastsArrayData_(Forecasts_array & forecasts) const;
    void readForecastsArrayData_(
            Forecasts_array & forecasts,
            std::array<size_t, 4> start, std::array<size_t, 4> count,
            std::array<ptrdiff_t, 4> stride = {1, 1, 1, 1}) const;


    void checkPath_(const std::string & file_path) const;
    void checkFileType_(const netCDF::NcFile & nc, FileType file_type) const;
    void checkDim_(const netCDF::NcFile & nc, const std::string & name) const;
    void checkVar_(const netCDF::NcFile & nc, const std::string & name) const;
};

#endif /* ANENREADNCDF_H */