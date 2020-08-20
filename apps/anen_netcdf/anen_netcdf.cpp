/* 
 * File:   anen_netcdf.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on June 12, 2020, 10:12 AM
 */

/** @file */

// Needed for ifstream
#include <fstream>
#include <sstream>
#include <iomanip>

#include "boost/filesystem/convenience.hpp"
#include "boost/program_options.hpp"
#include "boost/filesystem.hpp"

#include "Config.h"
#include "AnEnIS.h"
#include "AnEnSSE.h"
#include "Profiler.h"
#include "AnEnReadNcdf.h"
#include "AnEnWriteNcdf.h"
#include "ForecastsPointer.h"
#include "ObservationsPointer.h"

#if defined(_USE_MPI_EXTENSION)
#include <mpi.h>

// Ncdf is needed when MPI is used. Ncdf is used to read the length of dimensions
#include "Ncdf.h"
#endif


using namespace std;
using namespace boost::program_options;
namespace fs = boost::filesystem;

void runAnEnNcdf(
        const string & forecast_file,
        const string & observation_file,
        int station_start,
        int station_count,
        const vector<size_t> & obs_id,
        const string & test_start_str,
        const string & test_end_str,
        const string & search_start_str,
        const string & search_end_str,
        const string & fileout,
        const string & algorithm,
        Config & config,
        bool overwrite,
        bool profile,
        bool save_tests,
        bool convert_wind,
        const string & u_name,
        const string & v_name,
        const string & spd_name,
        const string & dir_name,
        const string & torch_model) {


    /**************************************************************************
     *                     Read Forecasts and Analysis                        *
     **************************************************************************/

    Profiler profiler;
    profiler.start();

    // Create times
    Time test_start, test_end, search_start, search_end;

    try {
        test_start = Time(test_start_str);
        test_end = Time(test_end_str);
        search_start = Time(search_start_str);
        search_end = Time(search_end_str);
    } catch (exception & e) {
        ostringstream msg;
        msg << "Failed during extracting test/search times." << endl << endl
                << "Did you follow the format YYYY-mm-dd HH:MM:SS ?" << endl
                << "Do you have extra quotes?" << endl << endl
                << "I got test_start: " << test_start_str
                << ", test_end: " << test_end_str << endl
                << "search_start: " << search_start_str <<
                ", search_end: " << search_end_str << endl << endl
                << "A common mistake is using surrounding double quotes. You don't need them if you are using them."
                << endl << endl << "The messages below come from the original error message:"
                << endl << e.what();
        throw runtime_error(msg.str());
    }

    // Sanity checks for input times
    if (config.operation && test_start <= search_end) throw runtime_error("Search end must be prior to test start in operation");
    if (test_start > test_end) throw runtime_error("Test start cannot be later than test end");
    if (search_start > search_end) throw runtime_error("Search start cannot be later than search end");


    /*
     * Read input data
     */

    // Initialize a reader
    AnEnReadNcdf anen_read(config.verbose);

    // Read forecasts
    ForecastsPointer forecasts, forecasts_backup;
    anen_read.readForecasts(forecast_file, forecasts, station_start, station_count);

    profiler.log_time_session("Reading forecasts");

    // Read observations
    ObservationsPointer observations;
    anen_read.readObservations(observation_file, observations, station_start, station_count);

    profiler.log_time_session("Reading observations");


    /*
     * Extract test and search times
     */
    Times test_times, search_times;
    const Times & forecast_times = forecasts.getTimes();

    forecast_times(test_start, test_end, test_times);
    forecast_times(search_start, search_end, search_times);

    profiler.log_time_session("Extracting test/search times");


#if defined(_ENABLE_AI)
    /*
     * Convert forecast variables with AI
     */
    if (!torch_model.empty()) {
        if (save_tests) forecasts_backup = forecasts;
        profiler.log_time_session("Backing up original forecasts");

        if (config.verbose >= Verbose::Progress) cout << "Transforming forecasts variables to latent features with AI ..." << endl;
        forecasts.featureTransform(torch_model, config.verbose);

        if (config.verbose >= Verbose::Progress) cout << "Initialize weights to all 1s because weights in latent space do not matter!" << endl;
        config.weights = vector<double>(1, forecasts.getParameters().size());

        profiler.log_time_session("Transforming features");
    }

#endif    


    /*
     * Convert wind parameters if specified
     */
    if (convert_wind) {
        if (!torch_model.empty()) throw runtime_error("AI transformation and wind transformation cannot be used together!");

        if (config.verbose >= Verbose::Progress) cout << "Converting wind U/V to wind speed/direction ..." << endl;
        forecasts.windTransform(u_name, v_name, spd_name, dir_name);
        profiler.log_time_session("Calculating wind speed/direction");
    }

    
    /**************************************************************************
     *                       Generate Analogs Ensemble                        *
     **************************************************************************/


    /*
     * Check for multivariate analog generation
     */
    if (obs_id.size() == 0) {
        // Use the default observation ID in config, Nothing is changed.
    } else if (obs_id.size() == 1) {
        // Use this observation ID
        config.obs_var_index = obs_id[0];
    } else {
        // We have multiple observations ID. We are generating multivariate analogs.
        if (config.verbose >= Verbose::Progress) cout << "Multi-analogs functionality ON" << endl;

        config.save_analogs = false;
        config.save_analogs_time_index = true;
        config.save_sims_station_index = true;
    }


    /*
     * Generate analogs
     */
    AnEn* anen = nullptr;

    if (algorithm == "IS") {
        anen = new AnEnIS(config);
    } else if (algorithm == "SSE") {
        anen = new AnEnSSE(config);
    } else {
        throw runtime_error("The algorithm is not recognized");
    }

    anen->compute(forecasts, observations, test_times, search_times);

    profiler += anen->getProfile();


    /**************************************************************************
     *                             Write Results                              *
     **************************************************************************/

    /*
     * Write AnEn results to an NetCDF file
     */
    AnEnWriteNcdf anen_write(config.verbose);

    const auto & forecast_flts = forecasts.getFLTs();
    const auto & forecast_parameters = forecasts.getParameters();
    const auto & forecast_stations = forecasts.getStations();

    if (obs_id.size() > 1) {
        
        /*
         * If we are generating multivariate analogs
         */
        
        unordered_map<string, size_t> obs_map;
        Functions::createObsMap(obs_map, obs_id, observations.getParameters());

        anen_write.writeMultiAnEn(fileout, obs_map, anen, algorithm, test_times, search_times,
                forecast_flts, forecast_parameters, forecast_stations, observations, overwrite);

        profiler.log_time_session("Writing multivariate analogs");
        
    } else {
        
        /*
         * If we are generating univariate analogs
         */

        anen_write.writeAnEn(fileout, anen, algorithm, test_times, search_times,
                forecast_flts, forecast_parameters, forecast_stations, overwrite);

        profiler.log_time_session("Writing univariate analogs");
    }


    /*
     * Save test observations and forecasts
     */
    if (save_tests) {

        // Create test forecasts
        ForecastsPointer test_forecasts(forecasts.getParameters(), forecasts.getStations(), test_times, forecasts.getFLTs());

        // Copy subset values from original forecasts
        forecasts.subset(test_forecasts);

        /*
         * Forecasts and observations are appended to the same file as AnEn
         * results. Therefore, I don't want the output to overwrite the existing
         * file which would remove the file entirely, but to append to the 
         * existing file.
         * 
         * Therefore, I explicitly turn off overwriting (false) and
         * turn on appending (true).
         */
        anen_write.writeForecasts(fileout, test_forecasts, false, true);

#if defined(_ENABLE_AI)
        // Create test forecasts with the original values if AI transformation is applied
        if (!torch_model.empty()) {
            ForecastsPointer test_original_forecasts(
                    forecasts_backup.getParameters(), forecasts_backup.getStations(),
                    test_times, forecasts_backup.getFLTs());
            forecasts.subset(test_original_forecasts);
            anen_write.writeForecasts(fileout, test_forecasts, false, true, "OriginalForecasts");
        }
#endif

        // Create test observations times that should be saved
        size_t max_flt = max_element(forecast_flts.left.begin(), forecast_flts.left.end())->second.timestamp;
        Time obs_end_time(max_flt + test_end.timestamp);

        Times test_obs_times;
        const Times & obs_times = observations.getTimes();
        obs_times(test_start, obs_end_time, test_obs_times);

        if (test_obs_times.size() == 0) {

            if (config.verbose >= Verbose::Progress)
                cerr << "Warning: Observations do not cover the test time period."
                    << " No test observations are saved." << endl;

        } else {

            // Create test observations
            ObservationsPointer test_observations(observations.getParameters(), observations.getStations(), test_obs_times);

            // Copy subset values from original observations
            observations.subset(test_observations);

            // Save subset observations
            anen_write.writeObservations(fileout, test_observations, false, true);
        }

        profiler.log_time_session("Writing forecasts and observations");
    }

    /*
     * Housekeeping
     */
    delete anen;

    if (profile) profiler.summary(cout);

    return;
}

int main(int argc, char** argv) {


    /**************************************************************************
     *                               Parse arguments                          *
     **************************************************************************/

    // Define variables to be parsed and extracted
    //
    // Required variables
    //
    string fileout, torch_model;
    string forecast_file, observation_file;
    string test_start, test_end, search_start, search_end;

    // Optional variables
    int verbose;
    string algorithm;
    vector<size_t> obs_id;
    vector<string> config_files;
    int station_start, station_count;
    bool overwrite, profile, save_tests, convert_wind;
    string u_name, v_name, spd_name, dir_name;

    Config config;

    // Define available command line parameters
    options_description desc("Available options");
    desc.add_options()
            ("help,h", "Print help information for options.")
            ("config,c", value< vector<string> >(&config_files)->multitoken(), "Config files (.cfg). Command line options overwrite config files.")

            // Required arguments
            ("forecast-file", value<string>(&forecast_file)->required(), "An NetCDF file for forecasts")
            ("observation-file", value<string>(&observation_file)->required(), "An NetCDF file for observations")
            ("out", value<string>(&fileout)->required(), "Output file path.")
            ("test-start", value<string>(&test_start)->required(), "Start date time for test with the format YYYY-mm-dd HH:MM:SS")
            ("test-end", value<string>(&test_end)->required(), "End date time for test.")
            ("search-start", value<string>(&search_start)->required(), "Start date time for search.")
            ("search-end", value<string>(&search_end)->required(), "End date time for search.")

            // Optional arguments
            ("verbose,v", value<int>(&verbose), "[Optional] Verbose level (0 - 4).")

#if defined(_ENABLE_AI)
            ("torch-model", value<string>(&torch_model)->default_value(""), "[Optional] The pretrained model serialized from PyTorch. This is usually a *.pt file.")
#endif
            ("station-start", value<int>(&station_start)->default_value(-1), "[Optional] Start index of stations to process")
            ("station-count", value<int>(&station_count)->default_value(-1), "[Optional] The number of stations to process from the start")
            ("algorithm", value<string>(&algorithm)->default_value("IS"), "[Optional] IS for Independent Search or SSE for Search Space Extension")
            ("obs-id", value< vector<size_t> >(&obs_id)->multitoken(), "[Optional] Observation variable index. If multiple indices are provided, multivariate analogs will be generated.")
            ("overwrite", bool_switch(&overwrite)->default_value(false), "[Optional] Overwrite files and variables.")
            ("profile", bool_switch(&profile)->default_value(false), "[Optional] Print profiler's report.")
            ("weights", value< vector<double> >(&(config.weights))->multitoken(), "[Optional] Weight for each parameter ID.")
            ("analogs", value<size_t>(&(config.num_analogs)), "[Optional] Number of analogs members.")
            ("sims", value<size_t>(&(config.num_sims)), "[Optional] Number of similarity members.")
            ("max-par-nan", value<size_t>(&(config.max_par_nan)), "[Optional] Maximum allowed number of NA in parameters.")
            ("max-flt-nan", value<size_t>(&(config.max_flt_nan)), "[Optional] Maximum allowed number of NA in lead times.")
            ("flt-radius", value<size_t>(&(config.flt_radius)), "[Optional] The number of surrounding lead times to compare for trends.")
            ("num-nearest", value<size_t>(&(config.num_nearest)), "[Optional] Number of neighbor stations to search.")
            ("distance", value<double>(&(config.distance)), "[Optional] Distance threshold when searching for neighbors.")
            ("extend-obs", bool_switch(&(config.extend_obs))->default_value(config.extend_obs), "[Optional] Use observations from search stations.")
            ("operation", bool_switch(&(config.operation))->default_value(config.operation), "[Optional] Use operational mode.")
            ("prevent-search-future", bool_switch(&(config.prevent_search_future))->default_value(config.prevent_search_future), "[Optional] Prevent using observations that are later than the current test forecast.")
            ("save-analogs", bool_switch(&(config.save_analogs))->default_value(config.save_analogs), "[Optional] Save analogs.")
            ("save-analogs-time-index", bool_switch(&(config.save_analogs_time_index))->default_value(config.save_analogs_time_index), "[Optional] Save time indices of analogs.")
            ("save-sims", bool_switch(&(config.save_sims))->default_value(config.save_sims), "[Optional] Save similarity.")
            ("save-sims-time-index", bool_switch(&(config.save_sims_time_index))->default_value(config.save_sims_time_index), "[Optional] Save time indices of similarity.")
            ("save-sims-station-index", bool_switch(&(config.save_sims_station_index))->default_value(config.save_sims_station_index), "[Optional] Save station indices of similarity.")
            ("save-tests", bool_switch(&save_tests)->default_value(false), "[Optional] Save test forecasts and observations if available")
            ("no-quick", bool_switch(&(config.quick_sort))->default_value(config.quick_sort), "[Optional] Disable nth_element sort, use partial sort.")
            ("convert-wind", bool_switch(&(convert_wind))->default_value(false), "[Optional] Use this option if your forecasts have only wind U and V components and you need to convert them to wind speed and direction. Please also specify --name-u --name-v --name-spd --name-dir. Wind speed and direction values will be calculated internally and replacing U and V components respectively.")
            ("name-u", value<string>(&u_name)->default_value("U"), "[Optional] Parameter name for U component of wind")
            ("name-v", value<string>(&v_name)->default_value("V"), "[Optional] Parameter name for V component of wind")
            ("name-spd", value<string>(&spd_name)->default_value("windSpeed"), "[Optional] Parameter name for wind speed")
            ("name-dir", value<string>(&dir_name)->default_value("windDirection"), "[Optional] Parameter name for wind direction");

    // Get all the available options
    vector<string> available_options;
    auto lambda = [&available_options](const boost::shared_ptr<boost::program_options::option_description> option) {
        available_options.push_back("--" + option->long_name());
    };
    for_each(desc.options().begin(), desc.options().end(), lambda);

    // Parse the command line first
    variables_map vm;
    parsed_options parsed = command_line_parser(argc, argv).options(desc).allow_unregistered().run();
    store(parsed, vm);

    if (vm.count("help") || argc == 1) {
        // If help messages are requested or there are
        // no extra arguments other than the command line itself
        //
        cout
#if defined(_USE_MPI_EXTENSION)
            << "Parallel Analogs Ensemble -- anen_netcdf_mpi "
#else
            << "Parallel Analogs Ensemble -- anen_netcdf "
#endif
#if defined(_ENABLE_AI)
            << "with AI support "
#endif
            << _APPVERSION << endl << _COPYRIGHT_MSG << endl << endl << desc << endl;
        return 0;
    }

    // Collect unregistered arguments and guess the intended options
    auto unregistered_keys = collect_unrecognized(parsed.options, exclude_positional);
    if (unregistered_keys.size() != 0) {
        Functions::guess_arguments(unregistered_keys, available_options, cerr);
        return 1;
    }

    // Then parse the configuration file
    if (vm.count("config")) {
        // If configuration file is specified, read it first.
        // The variable won't be written until we call notify.
        //
        config_files = vm["config"].as< vector<string> >();
    }

    if (!config_files.empty()) {
        for (const auto & config_file : config_files) {
            ifstream ifs(config_file.c_str());
            if (!ifs) {
                cerr << "Error: Can't open configuration file " << config_file << endl;
                return 1;
            } else {
                auto parsed_config = parse_config_file(ifs, desc, true);

                auto unregistered_keys_config = collect_unrecognized(parsed_config.options, exclude_positional);
                if (unregistered_keys_config.size() != 0) {
                    Functions::guess_arguments(unregistered_keys_config, available_options, cout);
                    return 1;
                }

                store(parsed_config, vm);
            }
        }
    }

    notify(vm);

    if (vm.count("verbose")) {
        config.setVerbose(verbose);
    }


    /**************************************************************************
     *                     Run analog generation with NC files                *
     **************************************************************************/
    // Expand file paths
    forecast_file = fs::absolute(fs::path(forecast_file.c_str())).string();
    observation_file = fs::absolute(fs::path(observation_file.c_str())).string();

#if defined(_USE_MPI_EXTENSION)
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);
    if (provided != MPI_THREAD_FUNNELED) throw runtime_error("The MPI implementation does not provide MPI_THREAD_FUNNELED");


    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    if (station_count == -1) {
        station_start = 0;
        station_count = Ncdf::readDimLength(forecast_file, Config::_DIM_STATIONS);
    }

    // Determine what stations to process for this rank. In this case, our master can
    // also do work. So we need to add 1 to the world size when using the functions from
    // Functions to take into consideration the fact that These functions assume that master 
    // process does not do any work.
    //
    station_start += Functions::getStartIndex(station_count, world_size + 1, world_rank + 1);
    station_count = Functions::getSubTotal(station_count, world_size + 1, world_rank + 1);

    // Determine the new output file name
    stringstream padded_rank;
    padded_rank << "_Rank" << std::setw(to_string(world_size).length()) << std::setfill('0') << world_rank;
    fileout = boost::filesystem::change_extension(fileout, "").string() + padded_rank.str() + string(".nc");

    if (config.verbose >= Verbose::Detail) cout << "Rank " << world_rank << "/" << world_size <<
            " processes " << station_count << " stations from #" << station_start << " writing into " << fileout << endl;
#endif

    runAnEnNcdf(forecast_file, observation_file, station_start, station_count,
            obs_id, test_start, test_end, search_start, search_end, fileout, 
            algorithm, config, overwrite, profile, save_tests, convert_wind,
            u_name, v_name, spd_name, dir_name, torch_model);

#if defined(_USE_MPI_EXTENSION)
    MPI_Finalize();
#endif

    return 0;
}

