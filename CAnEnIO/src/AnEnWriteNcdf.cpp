/* 
 * File:   AnEnWriteNcdf.cpp
 * Author: Weiming Hu (weiming@psu.edu)
 * 
 * Created on February 10, 2020, 2:42 PM
 */

#include "Ncdf.h"
#include "Functions.h"
#include "AnEnWriteNcdf.h"

#include "boost/filesystem.hpp"

#include <algorithm>
#include <functional>

using namespace std;
using namespace netCDF;

namespace filesys = boost::filesystem;

const bool AnEnWriteNcdf::_unlimited_parameters = false;
const bool AnEnWriteNcdf::_unlimited_stations = false;
const bool AnEnWriteNcdf::_unlimited_test_times = false;
const bool AnEnWriteNcdf::_unlimited_search_times = false;
const bool AnEnWriteNcdf::_unlimited_times = false;
const bool AnEnWriteNcdf::_unlimited_flts = false;
const bool AnEnWriteNcdf::_unlimited_members = false;

AnEnWriteNcdf::AnEnWriteNcdf() {
    Config config;
    verbose_ = config.verbose;
    setDimensions_();
}

AnEnWriteNcdf::AnEnWriteNcdf(const AnEnWriteNcdf& orig) {
    verbose_ = orig.verbose_;
    setDimensions_();
}

AnEnWriteNcdf::AnEnWriteNcdf(Verbose verbose) : verbose_(verbose) {
    setDimensions_();
}

AnEnWriteNcdf::~AnEnWriteNcdf() {
}

void
AnEnWriteNcdf::writeAnEn(const string & file, const AnEnIS & anen,
        const Times & test_times, const Times & search_times,
        const Times & forecast_flts, const Parameters & forecast_parameters,
        const Stations & forecast_stations, bool overwrite, bool append) const {

    if (verbose_ >= Verbose::Progress) cout << "Writing AnEn ..." << endl;

    // Check file path availability
    Ncdf::checkExists(file, overwrite, append);
    Ncdf::checkExtension(file);

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
    if (anen.save_analogs()) Ncdf::writeArray4D(nc, anen.analogs_value(), Config::_ANALOGS, analogs_dim_, unlimited_);
    if (anen.save_analogs_time_index()) Ncdf::writeArray4D(nc, anen.analogs_time_index(), Config::_ANALOGS_TIME_IND, analogs_dim_, unlimited_);
    if (anen.save_sims()) Ncdf::writeArray4D(nc, anen.sims_metric(), Config::_SIMS, sims_dim_, unlimited_);
    if (anen.save_sims_time_index()) Ncdf::writeArray4D(nc, anen.sims_time_index(), Config::_SIMS_TIME_IND, sims_dim_, unlimited_);

    // Save configuration variables as global attributes
    Ncdf::writeAttribute(nc, Config::_NUM_ANALOGS, (int) anen.num_analogs(), NcType::nc_INT, overwrite);
    Ncdf::writeAttribute(nc, Config::_NUM_SIMS, (int) anen.num_sims(), NcType::nc_INT, overwrite);
    Ncdf::writeAttribute(nc, Config::_OBS_ID, (int) anen.obs_var_index(), NcType::nc_INT, overwrite);
    Ncdf::writeAttribute(nc, Config::_NUM_PAR_NA, (int) anen.max_par_nan(), NcType::nc_INT, overwrite);
    Ncdf::writeAttribute(nc, Config::_NUM_FLT_NA, (int) anen.max_flt_nan(), NcType::nc_INT, overwrite);
    Ncdf::writeAttribute(nc, Config::_FLT_RADIUS, (int) anen.flt_radius(), NcType::nc_INT, overwrite);
    Ncdf::writeAttribute(nc, Config::_OPERATION, (int) anen.operation(), NcType::nc_INT, overwrite);
    Ncdf::writeAttribute(nc, Config::_QUICK, (int) anen.quick_sort(), NcType::nc_INT, overwrite);
    Ncdf::writeAttribute(nc, Config::_PREVENT_SEARCH_FUTURE, (int) anen.prevent_search_future(), NcType::nc_INT, overwrite);

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
    Ncdf::writeVector(nc, Config::_TEST_TIMES, Config::_DIM_TEST_TIMES, test_timestamps, NcType::nc_UINT64, _unlimited_test_times);
    Ncdf::writeVector(nc, Config::_SEARCH_TIMES, Config::_DIM_SEARCH_TIMES, search_timestamps, NcType::nc_UINT64, _unlimited_search_times);
    Ncdf::writeVector(nc, Config::_FLTS, Config::_DIM_FLTS, flt_timestamps, NcType::nc_UINT64, _unlimited_flts);
    Ncdf::writeStringVector(nc, Config::_PAR_NAMES, Config::_DIM_PARS, parameter_names, _unlimited_parameters);

    // Write meta information
    addMeta_(nc);

    // The file handler will automatically be closed when it is out of scope.
    // For C++ API older than 4.3.0, this function was not available.
    //
    // nc.close();
    return;
}

void
AnEnWriteNcdf::writeAnEn(const string& file, const AnEnSSE& anen,
        const Times& test_times, const Times& search_times,
        const Times& forecast_flts, const Parameters& forecast_parameters,
        const Stations& forecast_stations, bool overwrite, bool append) const {

    /*************************************************************************
     *                   Write AnEnSSE object                                *
     *************************************************************************/

    // Call the overloaded function
    const AnEnIS *p_anen = &anen;
    writeAnEn(file, *p_anen, test_times, search_times, forecast_flts,
            forecast_parameters, forecast_stations, overwrite, append);

    // Since the file has already been created, we open the file
    NcFile nc(file, NcFile::FileMode::write, NcFile::FileFormat::nc4);

    // Save stations index
    if (anen.save_sims_station_index()) Ncdf::writeArray4D(nc, anen.sims_station_index(), Config::_SIMS_STATION_IND, sims_dim_, unlimited_);

    // Save configuration variables as global attributes
    Ncdf::writeAttribute(nc, Config::_NUM_NEAREST, (int) anen.num_nearest(), NcType::nc_INT, overwrite);
    Ncdf::writeAttribute(nc, Config::_DISTANCE, anen.distance(), NcType::nc_DOUBLE, overwrite);
    Ncdf::writeAttribute(nc, Config::_EXTEND_OBS, (int) anen.extend_obs(), NcType::nc_INT, overwrite);

    // The file handler will automatically be closed when it is out of scope.
    // For C++ API older than 4.3.0, this function was not available.
    //
    // nc.close();

    return;
}

void
AnEnWriteNcdf::writeMultiAnEn(const string& file,
        const unordered_map<string, size_t> & obs_map, const AnEnIS& anen,
        const Times& test_times, const Times& search_times,
        const Times& forecast_flts, const Parameters& parameters,
        const Stations& stations, const Observations& observations,
        bool overwrite, bool append) const {

    if (verbose_ >= Verbose::Progress) cout << "Writing multivariate AnEn ..." << endl;

    /*
     * Sanity check
     */

    // Analog values should not be saved. Multivariate analog values will
    // be generated using the input observations ID.
    //
    if (anen.save_analogs()) throw runtime_error("Analogs value should not be saved internally when generating multivariate AnEn. Set config.save_analogs = false");

    // Analogs time index must be saved
    if (!anen.save_analogs_time_index()) throw runtime_error("Analogs time index should be saved when generating multivariate AnEn. Set config.save_analogs_time_index = true");


    /*
     * Write data
     * 
     * This is composed of two steps:
     * - writing AnEnIS
     * - appending multivariate analogs
     */

    // Write AnEn
    writeAnEn(file, anen, test_times, search_times, forecast_flts, parameters, stations, overwrite, append);

    // Append multivariate analogs
    NcFile nc(file, NcFile::FileMode::write, NcFile::FileFormat::nc4);
    const auto & analogs_time_index = anen.analogs_time_index();

    for (const auto & pair : obs_map) {
        Array4DPointer analogs;

        // Generate analog values based on the time index
        Functions::toValues(analogs, pair.second, analogs_time_index, observations);

        // Append analog to the existing file
        Ncdf::writeArray4D(nc, analogs, pair.first, analogs_dim_, unlimited_);
    }

    return;
}

void
AnEnWriteNcdf::writeMultiAnEn(const string& file,
        const unordered_map<string, size_t>& obs_map, const AnEnSSE& anen,
        const Times& test_times, const Times& search_times,
        const Times& forecast_flts, const Parameters& parameters,
        const Stations& stations, const Observations& observations,
        bool overwrite, bool append) const {

    if (verbose_ >= Verbose::Progress) cout << "Writing multivariate AnEn ..." << endl;

    /*
     * Sanity check
     */

    // Analog values should not be saved. Multivariate analog values will
    // be generated using the input observations ID.
    //
    if (anen.save_analogs()) throw runtime_error("Analogs value should not be saved internally when generating multivariate AnEn. Set config.save_analogs = false");

    // Analogs time index and similarity station index must be saved
    if (!anen.save_analogs_time_index()) throw runtime_error("Analogs time index should be saved when generating multivariate AnEn. Set config.save_analogs_time_index = true");
    if (!anen.save_sims_station_index()) throw runtime_error("Similarity station index should be saved when generating multivariate AnEn. Set config.save_analogs_time_index = true");


    /*
     * Write data
     * 
     * This is composed of two steps:
     * - writing AnEnSSE
     * - appending multivariate analogs
     */

    // Write AnEn
    writeAnEn(file, anen, test_times, search_times, forecast_flts, parameters, stations, overwrite, append);

    // Append multivariate analogs
    NcFile nc(file, NcFile::FileMode::write, NcFile::FileFormat::nc4);
    const auto & analogs_time_index = anen.analogs_time_index();
    const auto & sims_station_index = anen.sims_station_index();

    for (const auto & pair : obs_map) {
        Array4DPointer analogs;

        // Generate analog values based on the time index
        if (anen.extend_obs()) Functions::toValues(analogs, pair.second, analogs_time_index, sims_station_index, observations);
        else Functions::toValues(analogs, pair.second, analogs_time_index, observations);

        // Append analog to the existing file
        Ncdf::writeArray4D(nc, analogs, pair.first, analogs_dim_, unlimited_);
    }

    return;
}

void
AnEnWriteNcdf::writeForecasts(const string& file,
        const Forecasts & forecasts, bool overwrite, bool append) const {

    if (verbose_ >= Verbose::Progress) cout << "Writing forecasts ..." << endl;

    // Check file path availability
    bool file_exists = Ncdf::checkExists(file, overwrite, append);
    Ncdf::checkExtension(file);

    // If file exists, I always append. Note that if append is false, 
    // program will fail during the check phase.
    //
    // If file does not exists, I create a new file.
    //
    NcFile::FileMode nc_filemode;
    if (file_exists) nc_filemode = NcFile::FileMode::write;
    else nc_filemode = NcFile::FileMode::newFile;

    // Create an NetCDF file object for writing
    //
    // Note that, in older versions of the NetCDF C++ interface, there are
    // no such members functions, NcFile::open and NcFile::close. So
    // I'm trying to avoid them for better compatibility.
    //
    // NetCDF version 4 supports adding groups. To avoid naming conflicts,
    // forecasts are created under a group if the file already exists.
    //
    NcFile nc(file, nc_filemode, NcFile::FileFormat::nc4);

    // Write to child group if the file already exists.
    // Write to parent group if the file is newly created.
    //
    NcGroup nc_group;
    if (file_exists) {
        nc.addGroup("Forecasts");
        nc_group = nc.getGroup("Forecasts");
    } else {
        nc_group = nc;
    }


    /*************************************************************************
     *                   Write Forecasts object                              *
     *************************************************************************/
    /*
     * Forecasts are inherited from Array4D and BasicData. Therefore, I only
     * need to implement the writing for
     * 
     * - BasicData
     * - Array4D
     * - Forecasts own protected members: forecast lead times
     */

    // Add basic data
    addBasicData_(nc_group, forecasts);

    // Add Array4D
    array<string, 4> data_dim = {Config::_DIM_PARS, Config::_DIM_STATIONS, Config::_DIM_TIMES, Config::_DIM_FLTS};
    array<bool, 4 > unlimited = {_unlimited_parameters, _unlimited_stations, _unlimited_times, _unlimited_flts};
    
    Ncdf::writeArray4D(nc_group, forecasts, Config::_DATA, data_dim, unlimited);

    // Add the protected member forecast lead times
    vector<size_t> flt_timestamps;
    forecasts.getFLTs().getTimestamps(flt_timestamps);
    Ncdf::writeVector(nc_group, Config::_FLTS, Config::_DIM_FLTS, flt_timestamps, NcType::nc_UINT64, _unlimited_flts);
    return;
}

void
AnEnWriteNcdf::writeObservations(const string & file,
        const Observations & observations, bool overwrite, bool append) const {

    if (verbose_ >= Verbose::Progress) cout << "Writing observations ..." << endl;

    // Check file path availability
    bool file_exists = Ncdf::checkExists(file, overwrite, append);
    Ncdf::checkExtension(file);

    // If file exists, I always append. Note that if append is false, 
    // program will fail during the check phase.
    //
    // If file does not exists, I create a new file.
    //
    NcFile::FileMode nc_filemode;
    if (file_exists) nc_filemode = NcFile::FileMode::write;
    else nc_filemode = NcFile::FileMode::newFile;

    // Create an NetCDF file object for writing
    //
    // Note that, in older versions of the NetCDF C++ interface, there are
    // no such members functions, NcFile::open and NcFile::close. So
    // I'm trying to avoid them for better compatibility.
    //
    // NetCDF version 4 supports adding groups. To avoid naming conflicts,
    // forecasts are created under a group if the file already exists.
    //
    NcFile nc(file, nc_filemode, NcFile::FileFormat::nc4);

    // Write to child group if the file already exists.
    // Write to parent group if the file is newly created.
    //
    NcGroup nc_group;
    if (file_exists) {
        nc.addGroup("Observations");
        nc_group = nc.getGroup("Observations");
    } else {
        nc_group = nc;
    }


    /*************************************************************************
     *                   Write Observations object                           *
     *************************************************************************/
    /*
     * Observations are inherited from BasicData. Therefore, I only
     * need to implement the writing for
     * 
     * - BasicData
     * - Observations data
     */

    // Add basic data
    addBasicData_(nc_group, observations);

    // Check whether observation data array is column major
    const auto & arr = observations.getValuesPtr();

    if (observations.num_elements() >= 2) {
        if (observations.getValue(1, 0, 0) != arr[1]) {
            throw runtime_error("The observation data array is not column major");
        }
    }

    // Create dimensions
    NcDim dim0, dim1, dim2;
    dim0 = Ncdf::getDimension(nc, Config::_DIM_PARS, _unlimited_parameters, observations.getParameters().size());
    dim1 = Ncdf::getDimension(nc, Config::_DIM_STATIONS, _unlimited_stations, observations.getStations().size());
    dim2 = Ncdf::getDimension(nc, Config::_DIM_TIMES, _unlimited_times, observations.getTimes().size());

    /*
     * Create an NetCDF variable. Note the reversed dimension order so that we
     * can copy values from the column-major ordered pointer directly. The first
     * dimension in the initializer list is the slowest varying dimension
     */
    auto var = nc_group.addVar(Config::_DATA, NC_DOUBLE, {dim2, dim1, dim0});

    // Add observation data
    var.putVar(arr);

    return;
}

void
AnEnWriteNcdf::addBasicData_(netCDF::NcGroup& nc_group, const BasicData& basic_data) const {

    // Write parameters
    vector<string> parameter_names;
    vector<bool> circular_flags;

    const Parameters & parameters = basic_data.getParameters();

    parameters.getNames(parameter_names);
    parameters.getCirculars(circular_flags);

    // If a parameter is not circular, remove the name from the circular vector
    vector<string> circular_names = parameter_names;
    for (size_t i = 0; i < circular_flags.size(); ++i) {
        if (!circular_flags[i]) {
            circular_names[i].clear();
        }
    }

    // Write parameter names to file
    Ncdf::writeStringVector(nc_group, Config::_PAR_NAMES, Config::_DIM_PARS, parameter_names, _unlimited_parameters);
    Ncdf::writeStringVector(nc_group, Config::_CIRCULARS, Config::_DIM_PARS, circular_names, _unlimited_parameters);

    // Write stations
    const Stations & stations = basic_data.getStations();
    addStations_(nc_group, stations, _unlimited_stations);

    // Write times
    vector<size_t> timestamps;
    basic_data.getTimes().getTimestamps(timestamps);
    Ncdf::writeVector(nc_group, Config::_TIMES, Config::_DIM_TIMES, timestamps, NcType::nc_UINT64, _unlimited_times);

    return;
}

void
AnEnWriteNcdf::addStations_(netCDF::NcGroup& nc, const Stations & stations, bool unlimited) const {

    // Get station coordinates and names
    vector<string> names;
    vector<double> xs, ys;

    stations.getCoordinates(xs, ys);
    stations.getNames(names);

    // Add values to the NetCDF file as unlimited dimensions
    Ncdf::writeVector(nc, Config::_XS, Config::_DIM_STATIONS, xs, NcType::nc_DOUBLE, unlimited);
    Ncdf::writeVector(nc, Config::_YS, Config::_DIM_STATIONS, ys, NcType::nc_DOUBLE, unlimited);

    // Check whether all stations have default station names
    bool no_names = all_of(names.begin(), names.end(), [](const string & name) {return name == Config::_NAME;});

    if (no_names) {
        // Skip writing station names
    } else {
        // Write station names
        Ncdf::writeStringVector(nc, Config::_STATION_NAMES, Config::_DIM_STATIONS, names, unlimited);
    }

    return;
}

void
AnEnWriteNcdf::addMeta_(netCDF::NcGroup & nc) const {
    Ncdf::writeStringAttribute(nc, "Institute", "GEOlab @ Penn State", true);
    Ncdf::writeStringAttribute(nc, "Institute Link", "http://geolab.psu.edu", true);
    Ncdf::writeStringAttribute(nc, "Package", "Parallel Analog Ensemble", true);
    Ncdf::writeStringAttribute(nc, "Package Version", _APPVERSION, true);
    Ncdf::writeStringAttribute(nc, "Package Link", "https://weiming-hu.github.io/AnalogsEnsemble", true);
    Ncdf::writeStringAttribute(nc, "Report Issues", "https://github.com/Weiming-Hu/AnalogsEnsemble/issues", true);
    return;
}

void
AnEnWriteNcdf::setDimensions_() {
    
    // Analogs are four dimensions [stations][test times][forecast lead times][members]
    analogs_dim_ = {Config::_DIM_STATIONS, Config::_DIM_TEST_TIMES, Config::_DIM_FLTS, Config::_DIM_ANALOGS};
    
    // Similarities are four dimensions [stations][test times][forecast lead times][members]
    sims_dim_ = {Config::_DIM_STATIONS, Config::_DIM_TEST_TIMES, Config::_DIM_FLTS, Config::_DIM_SIMS};
    
    // Set whether the dimensions should be unlimited. Right now, only limited dimensions are implemented.
    unlimited_ = {_unlimited_stations, _unlimited_test_times, _unlimited_flts, _unlimited_members};
    
    return;
}
