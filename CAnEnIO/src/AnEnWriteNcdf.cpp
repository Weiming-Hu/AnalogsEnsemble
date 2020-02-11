/* 
 * File:   AnEnWriteNcdf.cpp
 * Author: Weiming Hu (weiming@psu.edu)
 * 
 * Created on February 10, 2020, 2:42 PM
 */

#include "Ncdf.h"
#include "AnEnWriteNcdf.h"

#include <boost/filesystem.hpp>

using namespace std;
using namespace netCDF;

namespace filesys = boost::filesystem;

/*
 * I'm defining the following:
 * - Unlimited dimensions: num_stations, num_test_times;
 * - Fixed length dimensions: num_flts, num_analogs/similairty;
 */
const bool AnEnWriteNcdf::_unlimited_stations = true;
const bool AnEnWriteNcdf::_unlimited_test_times = true;
const bool AnEnWriteNcdf::_unlimited_flts = false;
const bool AnEnWriteNcdf::_unlimited_members = false;

AnEnWriteNcdf::AnEnWriteNcdf() {
    Config config;
    verbose_ = config.verbose;
}

AnEnWriteNcdf::AnEnWriteNcdf(const AnEnWriteNcdf& orig) {
    verbose_ = orig.verbose_;
}

AnEnWriteNcdf::~AnEnWriteNcdf() {
}

void
AnEnWriteNcdf::writeAnEn(const std::string & file, const AnEnIS & anen,
        const Times & test_times, const Times & search_times,
        const Times & forecast_flts, const Parameters & forecast_parameters,
        const Stations & forecast_stations, bool overwrite) const {

    // Check file path availability
    if (filesys::exists(file)) {
        if (overwrite) {
            if (verbose_ >= Verbose::Progress) cout << "Overwrite " << file << endl;
            filesys::remove(file);
        } else {
            ostringstream msg;
            msg << "File " << file << " exists. Use overwrite = true";
            throw runtime_error(msg.str());
        }
    }

    // Create an NetCDF file object for writing
    NcFile nc(file, NcFile::FileMode::newFile, NcFile::FileFormat::nc4);

    /*************************************************************************
     *                   Write AnEnIS object                                 *
     *************************************************************************/
    /* 
     * AnEnIS has the following components to be written:
     * - configuration variables
     * - 4D arrays that are controlled by save_* boolean variables
     */

    // Save array if they are generated
    array<string, 4> analogs_dim = {Config::_DIM_STATIONS, Config::_DIM_TEST_TIMES, Config::_DIM_FLTS, Config::_DIM_ANALOGS};
    array<string, 4> sims_dim = {Config::_DIM_STATIONS, Config::_DIM_TEST_TIMES, Config::_DIM_FLTS, Config::_DIM_SIMS};
    array<bool, 4 > unlimited = {_unlimited_stations, _unlimited_test_times, _unlimited_flts, _unlimited_members};

    if (anen.save_analogs()) Ncdf::writeArray4D(nc, anen.analogs_value(), Config::_ANALOGS, analogs_dim, unlimited);
    if (anen.save_analogs_time_index()) Ncdf::writeArray4D(nc, anen.analogs_time_index(), Config::_ANALOGS_TIME_IND, analogs_dim, unlimited);
    if (anen.save_sims()) Ncdf::writeArray4D(nc, anen.sims_metric(), Config::_SIMS, sims_dim, unlimited);
    if (anen.save_sims_time_index()) Ncdf::writeArray4D(nc, anen.sims_time_index(), Config::_SIMS_TIME_IND, sims_dim, unlimited);

    // Save configuration variables as global attributes
    Ncdf::writeAttributes(nc, Config::_NUM_ANALOGS, (int) anen.num_analogs(), NcType::nc_INT, overwrite);
    Ncdf::writeAttributes(nc, Config::_NUM_SIMS, (int) anen.num_sims(), NcType::nc_INT, overwrite);
    Ncdf::writeAttributes(nc, Config::_OBS_ID, (int) anen.obs_var_index(), NcType::nc_INT, overwrite);
    Ncdf::writeAttributes(nc, Config::_NUM_PAR_NA, (int) anen.max_par_nan(), NcType::nc_INT, overwrite);
    Ncdf::writeAttributes(nc, Config::_NUM_FLT_NA, (int) anen.max_flt_nan(), NcType::nc_INT, overwrite);
    Ncdf::writeAttributes(nc, Config::_FLT_RADIUS, (int) anen.flt_radius(), NcType::nc_INT, overwrite);
    Ncdf::writeAttributes(nc, Config::_OPERATION, (int) anen.operation(), NcType::nc_INT, overwrite);
    Ncdf::writeAttributes(nc, Config::_QUICK, (int) anen.quick_sort(), NcType::nc_INT, overwrite);
    Ncdf::writeAttributes(nc, Config::_PREVENT_SEARCH_FUTURE, (int) anen.prevent_search_future(), NcType::nc_INT, overwrite);

    // Save weights with fixed length dimension of num_parameters
    Ncdf::writeVector(nc, Config::_WEIGHTS, Config::_DIM_PARS, anen.weights(), NcType::nc_DOUBLE, false);


    /*************************************************************************
     *                   Write Extra variables                               *
     *************************************************************************/
    vector<size_t> test_timestamps, search_timestamps, flt_timestamps;
    vector<string> parameter_names;

    // Get values from the objects to write
    test_times.getTimestamps(test_timestamps);
    search_times.getTimestamps(search_timestamps);
    forecast_flts.getTimestamps(flt_timestamps);
    forecast_parameters.getNames(parameter_names);

    // Write to the file
    addStations_(nc, forecast_stations, _unlimited_stations);
    Ncdf::writeVector(nc, Config::_TEST_TIMES, Config::_DIM_TEST_TIMES, test_timestamps, NcType::nc_DOUBLE, _unlimited_test_times);
    Ncdf::writeVector(nc, Config::_FLTS, Config::_DIM_FLTS, flt_timestamps, NcType::nc_DOUBLE, _unlimited_flts);
    Ncdf::writeVector(nc, Config::_SEARCH_TIMES, Config::_DIM_SEARCH_TIMES, search_timestamps, NcType::nc_DOUBLE, false);
    Ncdf::writeStringVector(nc, Config::_PAR_NAMES, Config::_DIM_PARS, parameter_names, false);
    
    nc.close();
    return;
}

void
AnEnWriteNcdf::writeAnEn(const std::string& file, const AnEnSSE& anen,
        const Times& test_times, const Times& search_times,
        const Times& forecast_flts, const Parameters& forecast_parameters,
        const Stations& forecast_stations, bool overwrite) const {
    
    /*************************************************************************
     *                   Write AnEnSSE object                                *
     *************************************************************************/

    // Call the overloaded function
    const AnEnIS *p_anen = &anen;
    writeAnEn(file, *p_anen, test_times, search_times, forecast_flts,
            forecast_parameters, forecast_stations, overwrite);
    
    // Since the file has already been created, we open the file
    NcFile nc(file, NcFile::FileMode::write, NcFile::FileFormat::nc4);

    // Save stations index
    array<string, 4> sims_dim = {Config::_DIM_STATIONS, Config::_DIM_TEST_TIMES, Config::_DIM_FLTS, Config::_DIM_SIMS};
    array<bool, 4 > unlimited = {_unlimited_stations, _unlimited_test_times, _unlimited_flts, _unlimited_members};
    if (anen.save_sims_station_index()) Ncdf::writeArray4D(nc, anen.sims_station_index(), Config::_SIMS_STATION_IND, sims_dim, unlimited);

    // Save configuration variables as global attributes
    Ncdf::writeAttributes(nc, Config::_NUM_NEAREST, (int) anen.num_nearest(), NcType::nc_INT, overwrite);
    Ncdf::writeAttributes(nc, Config::_DISTANCE,  anen.distance(), NcType::nc_DOUBLE, overwrite);
    Ncdf::writeAttributes(nc, Config::_EXTEND_OBS, (int) anen.extend_obs(), NcType::nc_INT, overwrite);

    nc.close();
    return;
}

void
AnEnWriteNcdf::addStations_(netCDF::NcFile& nc, const Stations & stations, bool unlimited) const {

    // Get station coordinates and names
    vector<string> names;
    vector<double> xs, ys;

    stations.getCoordinates(xs, ys);
    stations.getNames(names);

    // Add values to the NetCDF file as unlimited dimensions
    Ncdf::writeVector(nc, Config::_XS, Config::_DIM_STATIONS, xs, NcType::nc_DOUBLE, unlimited);
    Ncdf::writeVector(nc, Config::_YS, Config::_DIM_STATIONS, ys, NcType::nc_DOUBLE, unlimited);
    Ncdf::writeStringVector(nc, Config::_STATION_NAMES, Config::_DIM_STATIONS, names, unlimited);
    
    return;
}