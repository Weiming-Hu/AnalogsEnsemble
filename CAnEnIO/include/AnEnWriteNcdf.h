/* 
 * File:   AnEnWriteNcdf.h
 * Author: Weiming Hu (weiming@psu.edu)
 *
 * Created on February 10, 2020, 2:42 PM
 */

#ifndef ANENWRITENCDF_H
#define ANENWRITENCDF_H

#include <unordered_map> 
#include <netcdf>

#include "Config.h"
#include "AnEnIS.h"
#include "AnEnSSE.h"
#include "Forecasts.h"
#include "Observations.h"

/**
 * \class AnEnWriteNcdf
 * 
 * \brief AnEnWriteNcdf provides the functionality to write to NetCDF files.
 */
class AnEnWriteNcdf {
public:
    AnEnWriteNcdf();
    AnEnWriteNcdf(const AnEnWriteNcdf& orig);
    AnEnWriteNcdf(Verbose verbose);
    virtual ~AnEnWriteNcdf();

    /**
     * Write AnEn into to an NetCDF file
     * 
     * @param file The output file name
     * @param anen The AnEn object to write. Or it can be a pointer to AnEn.
     * @param algorithm The type to convert the pointer of AnEn to, either IS or SSE.
     * @param test_times The test times used to generate AnEn
     * @param search_times The search times used to generate AnEn
     * @param forecast_flts The lead times of AnEn forecasts
     * @param parameters The forecast parameters used to generate AnEn
     * @param stations The stations of AnEn forecasts
     * @param overwrite Whether to overwrite existing files
     * @param append Whether to append to existing files
     */
    void writeAnEn(const std::string & file, const AnEnIS &,
            const Times & test_times, const Times & search_times,
            const Times & forecast_flts, const Parameters &, const Stations &,
            bool overwrite = false, bool append = false) const;
    void writeAnEn(const std::string & file, const AnEnSSE &,
            const Times & test_times, const Times & search_times,
            const Times & forecast_flts, const Parameters &, const Stations &,
            bool overwrite = false, bool append = false) const;
    void writeAnEn(const std::string & file, const AnEn*, const std::string &,
            const Times & test_times, const Times & search_times,
            const Times & forecast_flts, const Parameters &, const Stations &,
            bool overwrite = false, bool append = false) const;

    /**
     * Write AnEn into an NetCDF file. Multivariate analogs are generated
     * using the index from AnEn.
     * @param file The output file name
     * @param obs_map An unordered map with variable names as the keys and the
     * corresponding observations ID as values. Variables names will be used 
     * in the output NetCDF file.
     * @param anen The AnEn object to write. Or it can be a pointer to AnEn.
     * @param algorithm The type to convert the pointer of AnEn to, either IS or SSE.
     * @param test_times The test times used to generate AnEn
     * @param search_times The search times used to generate AnEn
     * @param forecast_flts The lead times of AnEn forecasts
     * @param parameters The forecast parameters used to generate AnEn
     * @param stations The stations of AnEn forecasts
     * @param observations The observations used in AnEn generation
     * @param overwrite Whether to overwrite existing files
     * @param append Whether to append to existing files
     */
    void writeMultiAnEn(const std::string & file,
            const std::unordered_map<std::string, std::size_t> &, const AnEnIS &,
            const Times & test_times, const Times & search_times,
            const Times & forecast_flts, const Parameters &, const Stations &,
            const Observations&, bool overwrite = false, bool append = false) const;
    void writeMultiAnEn(const std::string & file,
            const std::unordered_map<std::string, std::size_t> &, const AnEnSSE &,
            const Times & test_times, const Times & search_times,
            const Times & forecast_flts, const Parameters &, const Stations &,
            const Observations&, bool overwrite = false, bool append = false) const;
    void writeMultiAnEn(const std::string & file,
            const std::unordered_map<std::string, std::size_t> &,
            const AnEn*, const std::string & algorithm,
            const Times & test_times, const Times & search_times,
            const Times & forecast_flts, const Parameters &, const Stations &,
            const Observations&, bool overwrite = false, bool append = false) const;

    /**
     * Write forecasts.
     * @param file The output file name
     * @param forecasts Forecasts
     * @param overwrite Whether to overwrite files
     * @param append Whether to append to files
     */
    void writeForecasts(const std::string & file, const Forecasts &,
            bool overwrite = false, bool append = false) const;

    /**
     * Write observations.
     * @param file The output file name
     * @param observations Observations
     * @param overwrite Whether to overwrite files
     * @param append Whether to append to files
     */
    void writeObservations(const std::string & file, const Observations &,
            bool overwrite = false, bool append = false) const;

    /*
     * Unlimited dimensions are not supported. Unlimited dimensions might be
     * a future feature.
     * 
     * To implement unlimited dimensions, you probably need to add dimensional
     * variables. Right now, dimensions are not associated with variables.
     * 
     * For good practices, please refer to 
     * https://www.unidata.ucar.edu/software/netcdf/docs/BestPractices.html
     */
    const static bool _unlimited_parameters;
    const static bool _unlimited_stations;
    const static bool _unlimited_test_times;
    const static bool _unlimited_search_times;
    const static bool _unlimited_times;
    const static bool _unlimited_flts;
    const static bool _unlimited_members;

protected:
    Verbose verbose_;
    
    std::array<std::string, 4> analogs_dim_;
    std::array<std::string, 4> sims_dim_;
    std::array<bool, 4 > unlimited_;

    void addBasicData_(netCDF::NcGroup &, const BasicData &) const;
    void addStations_(netCDF::NcGroup &, const Stations &, bool) const;
    void addMeta_(netCDF::NcGroup &) const;
    void setDimensions_();
};

#endif /* ANENWRITENCDF_H */

