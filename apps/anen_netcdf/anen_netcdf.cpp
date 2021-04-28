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
#include "AnEnSSEMS.h"
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
        int fcst_station_start, int fcst_station_count,
        const vector<size_t> fcst_stations_subset,
        int obs_station_start, int obs_station_count,
        const vector<size_t> & obs_id,
        const string & test_start_str,
        const string & test_end_str,
        const vector<string> & test_times_str,
        const string & search_start_str,
        const string & search_end_str,
        const string & fileout,
        const string & algorithm,
        Config & config,
        bool overwrite,
        bool profile,
        bool save_tests,
        bool unwrap_obs,
        bool convert_wind,
        const vector<string> & u_names,
        const vector<string> & v_names,
        const vector<string> & spd_names,
        const vector<string> & dir_names,
        const string & embedding_model,
        const string & similarity_model,
        long int ai_flt_radius,
        const string & fcst_grid_file) {


    /**************************************************************************
     *                     Read Forecasts and Analysis                        *
     **************************************************************************/

    Profiler profiler;
    profiler.start();

    // Create times
    Time test_start, test_end, search_start, search_end;
    Times test_times, search_times;

    try {

        if (test_times_str.empty()) {
            test_start = Time(test_start_str);
            test_end = Time(test_end_str);
        } else {
            test_start = Time(test_times_str.front());
            test_end = Time(test_times_str.back());
            for (const auto & test_time_str : test_times_str) test_times.push_back(Time(test_time_str));
        }

        search_start = Time(search_start_str);
        search_end = Time(search_end_str);

    } catch (exception & e) {
        ostringstream msg;
        msg << "Failed during extracting test/search times." << endl << endl
                << "Did you follow the format YYYY-mm-dd HH:MM:SS ?" << endl
                << "Do you have extra quotes?" << endl << endl
                << "I got test_start: " << test_start_str << endl
                << "test_end: " << test_end_str << endl
                << "test_times: " << Functions::format(test_times_str) << endl
                << "search_start: " << search_start_str << endl
                << "search_end: " << search_end_str << endl << endl
                << "A common mistake is using surrounding double quotes. You don't need them if you are using config files."
                << endl << endl << "The messages below come from the original error:"
                << endl << e.what();
        throw runtime_error(msg.str());
    }

    // Sanity checks for input times
    if (config.operation && test_start <= search_end) throw runtime_error("Search end must be prior to test start in operation");
    if (test_start > test_end) throw runtime_error("Test start cannot be later than test end");
    if (search_start > search_end) throw runtime_error("Search start cannot be later than search end");

    if (!test_times_str.empty())
        if (test_times.size() != test_times_str.size())
            throw runtime_error("Duplicates found in test times");


    /*
     * Read input data
     */

    // Initialize a reader
    AnEnReadNcdf anen_read(config.verbose);

    // Read forecasts
    ForecastsPointer forecasts, forecasts_backup;
    anen_read.readForecasts(forecast_file, forecasts, fcst_station_start, fcst_station_count);

    profiler.log_time_session("Reading forecasts");

    if (fcst_stations_subset.size() != 0) {

        Stations stations_subset;
        const Stations & stations = forecasts.getStations();
        for (const auto & i : fcst_stations_subset) stations_subset.push_back(stations.getStation(i));

        ForecastsPointer forecasts_subset;
        forecasts.subset(forecasts.getParameters(), stations_subset, forecasts.getTimes(), forecasts.getFLTs(), forecasts_subset);

        forecasts = forecasts_subset;
        profiler.log_time_session("Subsetting forecasts");
    }

    // Read observations
    ObservationsPointer observations;
    anen_read.readObservations(observation_file, observations, obs_station_start, obs_station_count);

    profiler.log_time_session("Reading observations");


    /*
     * Extract test and search times
     */
    const Times & forecast_times = forecasts.getTimes();

    // Only subset test times when test times str is empty.
    // Otherwise, test times have already been assigned.
    //
    if (test_times_str.empty()) {
        forecast_times(test_start, test_end, test_times);
    }

    forecast_times(search_start, search_end, search_times);

    profiler.log_time_session("Extracting test/search times");


#if defined(_ENABLE_AI)
    /*
     * Convert forecast variables with AI
     */
    if (!embedding_model.empty()) {

        // Set up the grid
        if (!fcst_grid_file.empty()) forecasts.setGrid(fcst_grid_file, config.verbose);

        if (save_tests) forecasts_backup = forecasts;
        profiler.log_time_session("Backing up original forecasts");

        if (config.verbose >= Verbose::Progress) cout << "Transforming forecasts variables to latent features with AI ..." << endl;
        forecasts.featureTransform(embedding_model, config.verbose, ai_flt_radius);

        if (config.verbose >= Verbose::Progress) cout << "Initialize weights to all 1s because weights in latent space do not matter!" << endl;
        config.weights = vector<double>(forecasts.getParameters().size(), 1);

        profiler.log_time_session("Transforming features");

        if (convert_wind) {
            convert_wind = false;
            if (config.verbose >= Verbose::Warning)
                cerr << "Warning: AI transformation and wind transformation cannot be used together! Wind transformation is turned off!" << endl;
        }
    }

#endif    


    /*
     * Convert wind parameters if specified
     */
    if (convert_wind) {
        if (config.verbose >= Verbose::Progress) cout << "Converting wind U/V to wind speed/direction ..." << endl;
        for (size_t name_index = 0; name_index < u_names.size(); name_index++) {
            forecasts.windTransform(u_names[name_index], v_names[name_index], spd_names[name_index], dir_names[name_index]);
        }
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

        if (forecasts.getStations().size() == observations.getStations().size()) anen = new AnEnSSE(config);
        else anen = new AnEnSSEMS(config);

    } else {
        throw runtime_error("The algorithm is not recognized");
    }

#if defined(_ENABLE_AI)
    if (!similarity_model.empty()) anen->load_similarity_model(similarity_model);
#endif

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
    const auto & observation_stations = observations.getStations();

    if (obs_id.size() > 1) {
        
        /*
         * If we are generating multivariate analogs
         */
        
        unordered_map<string, size_t> obs_map;
        Functions::createObsMap(obs_map, obs_id, observations.getParameters());

        anen_write.writeMultiAnEn(fileout, obs_map, anen, algorithm, test_times, search_times,
                forecast_flts, forecast_parameters, observation_stations, observations, overwrite);

        profiler.log_time_session("Writing multivariate analogs");
        
    } else {
        
        /*
         * If we are generating univariate analogs
         */

        anen_write.writeAnEn(fileout, anen, algorithm, test_times, search_times,
                forecast_flts, forecast_parameters, observation_stations, overwrite);

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
        if (!embedding_model.empty()) {
            ForecastsPointer test_original_forecasts(
                    forecasts_backup.getParameters(), forecasts_backup.getStations(), test_times, forecasts_backup.getFLTs());
            forecasts_backup.subset(test_original_forecasts);
            anen_write.writeForecasts(fileout, test_original_forecasts, false, true, "OriginalForecasts");
        }
#endif

        // Create test observations times that should be saved
        size_t max_flt = max_element(forecast_flts.left.begin(), forecast_flts.left.end())->second.timestamp;
        Time obs_end_time(max_flt + test_end.timestamp);

        Times test_obs_times;
        const Times & obs_times = observations.getTimes();
        obs_times(test_start, obs_end_time, test_obs_times);

        if (test_obs_times.size() == 0) {

            if (config.verbose >= Verbose::Warning)
                cerr << "Warning: Observations do not cover the test time period."
                    << " No test observations are saved." << endl;

        } else {

            // Create test observations
            ObservationsPointer test_observations(observations.getParameters(), observations.getStations(), test_obs_times);

            // Copy subset values from original observations
            observations.subset(test_observations);

            if (unwrap_obs) {
                if (config.verbose >= Verbose::Progress) {
                    cout << "Saving the unwrapped observations as forecasts ..." << endl;
                }

                ForecastsPointer unwrapped_observations;
                Functions::unwrapTimeSeries(unwrapped_observations, test_times, forecasts.getFLTs(), test_observations);

                anen_write.writeForecasts(fileout, unwrapped_observations, false, true, "AlignedObservations");

            } else {
                // Save subset observations
                anen_write.writeObservations(fileout, test_observations, false, true);
            }
        }

        profiler.log_time_session("Writing forecasts and observations");
    }

    /*
     * Housekeeping
     */
    delete anen;

    if (config.verbose >= Verbose::Progress) cout << "anen_netcdf complete!" << endl;
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
    string fileout, embedding_model, similarity_model;
    string forecast_file, observation_file;
    string test_start, test_end, search_start, search_end;

    // Optional variables
    int verbose;
    string algorithm, fcst_grid_file;
    vector<size_t> obs_id, fcst_stations_subset;
    vector<string> config_files, u_names, v_names, spd_names, dir_names, test_times_str;
    int fcst_station_start, fcst_station_count, obs_station_start, obs_station_count;
    bool overwrite, profile, save_tests, unwrap_obs, convert_wind;
    long int ai_flt_radius;

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
            ("test-start", value<string>(&test_start), "[Optional] Start date time for test with the format YYYY-mm-dd HH:MM:SS")
            ("test-end", value<string>(&test_end), "[Optional] End date time for test.")
            ("test-times", value< vector<string> >(&test_times_str)->multitoken(), "[Optional] The date times for test with the format YYYY-mm-dd HH:MM:SS. This will overwrite '--test-start' and '--test-end'")
            ("search-start", value<string>(&search_start)->required(), "Start date time for search.")
            ("search-end", value<string>(&search_end)->required(), "End date time for search.")

            // Optional arguments
            ("verbose,v", value<int>(&verbose), "[Optional] Verbose level (0 - 4).")

#if defined(_ENABLE_AI)
            ("ai-embedding", value<string>(&embedding_model)->default_value(""), "[Optional] The pretrained AI model serialized from PyTorch for embeddings. This is usually a *.pt file.")
            ("ai-similarity", value<string>(&similarity_model)->default_value(""), "[Optional] The pretrained AI model serialized from PyTorch for similarity. This is usually a *.pt file.")
            ("ai-flt-radius", value<long int>(&ai_flt_radius)->default_value(1), "[Optional] The number of surrounding lead times used for AI embedding.")
#endif
            ("fcst-station-start", value<int>(&fcst_station_start)->default_value(-1), "[Optional] Start index of forecast stations to process")
            ("fcst-station-count", value<int>(&fcst_station_count)->default_value(-1), "[Optional] The number of forecast stations to process from the start")
            ("fcst-stations-subset", value< vector<size_t> >(&fcst_stations_subset)->multitoken(), "[Optional] AFTER THE FORECASTS HAVE BEEN READ INTO MEMORY, use these indices to subset the stations.")
            ("obs-station-start", value<int>(&obs_station_start)->default_value(-1), "[Optional] Start index of observation stations to process")
            ("obs-station-count", value<int>(&obs_station_count)->default_value(-1), "[Optional] The number of observation stations to process from the start")
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
            ("extend-obs", bool_switch(&(config.extend_obs))->default_value(config.extend_obs), "[Optional] Use observations from search stations. Change this in *.cfg")
            ("exclude-closest-location", bool_switch(&(config.exclude_closest_location))->default_value(config.exclude_closest_location), "[Optional] Whether to exclude the closest station in the search stations. Only valid for SSE.")
            ("operation", bool_switch(&(config.operation))->default_value(config.operation), "[Optional] Use operational mode.")
            ("prevent-search-future", bool_switch(&(config.prevent_search_future))->default_value(config.prevent_search_future), "[Optional] Prevent using observations that are later than the current test forecast. Change this in *.cfg")
            ("no-norm", bool_switch(&(config.no_norm))->default_value(config.no_norm), "[Optional] Whether to skip standard deviation normalization")
            ("save-analogs", bool_switch(&(config.save_analogs))->default_value(config.save_analogs), "[Optional] Save analogs. Change this in *.cfg")
            ("save-analogs-time-index", bool_switch(&(config.save_analogs_time_index))->default_value(config.save_analogs_time_index), "[Optional] Save time indices of analogs.")
            ("save-sims", bool_switch(&(config.save_sims))->default_value(config.save_sims), "[Optional] Save similarity.")
            ("save-sims-time-index", bool_switch(&(config.save_sims_time_index))->default_value(config.save_sims_time_index), "[Optional] Save time indices of similarity.")
            ("save-sims-station-index", bool_switch(&(config.save_sims_station_index))->default_value(config.save_sims_station_index), "[Optional] Save station indices of similarity.")
            ("save-tests", bool_switch(&save_tests)->default_value(false), "[Optional] Save test forecasts and observations if available")
            ("unwrap-test-obs", bool_switch(&unwrap_obs)->default_value(false), "[Optional] When saving test observations (--save-tests), unwrap observation time series to align it with forecasts")
            ("quick-sort", bool_switch(&(config.quick_sort))->default_value(config.quick_sort), "[Optional] Use nth_element sort. Change this in *.cfg ")
            ("convert-wind", bool_switch(&(convert_wind))->default_value(false), "[Optional] Use this option if your forecasts have only wind U and V components and you need to convert them to wind speed and direction. Please also specify --name-u --name-v --name-spd --name-dir. Wind speed and direction values will be calculated internally and replacing U and V components respectively.")
            ("name-u", value< vector<string> >(&u_names)->multitoken(), "[Optional] Parameter name(s) for U component of wind")
            ("name-v", value< vector<string> >(&v_names)->multitoken(), "[Optional] Parameter name(s) for V component of wind")
            ("name-spd", value< vector<string> >(&spd_names)->multitoken(), "[Optional] Parameter name(s) for wind speed")
            ("name-dir", value< vector<string> >(&dir_names)->multitoken(), "[Optional] Parameter name(s) for wind direction")
            ("fcst-grid", value<string>(&fcst_grid_file), "[Optional] A grid file to be associated with forecasts. Currently only used within the spatial metric with AI.");

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

    if (vm.count("test-times")) {
        // Test times are specified. This overwrites test start and end

        if (config.verbose >= Verbose::Warning) {
            if (!test_start.empty()) cerr << "Warning: Both --test-times and --test-start are specified. --test-times take priority!" << endl;
            if (!test_end.empty()) cerr << "Warning: Both --test-times and --test-end are specified. --test-times take priority!" << endl;
        }

        test_start.clear();
        test_end.clear();
        
        sort(test_times_str.begin(), test_times_str.end());

    } else {
        // Must specify start and end for tests
        if ( (!vm.count("test-start")) | (!vm.count("test-end")) ) {
            throw runtime_error("You must specify either (--test-times) or (--test-start and --test-end)");
        }
    }

    // Check whether wind names are consistent
    if (convert_wind) {
        if (!(u_names.size() == v_names.size() && u_names.size() == spd_names.size() && u_names.size() == dir_names.size())) {
            ostringstream msg;
            msg << "There should be the same number of U, V, wind speed, and wind direction names!\n"
                << "Got " << u_names.size() << " u names, " << v_names.size() << " v names; "
                << spd_names.size() << " speed names; and " << dir_names.size() << " direction names!" << endl;
            throw runtime_error(msg.str());
        }

        if (u_names.size() == 0 ) {
            throw runtime_error("Specify --name-u, --name-v, --name-spd, and --name-dir!");
        }
    }


    /**************************************************************************
     *                     Run analog generation with NC files                *
     **************************************************************************/
    // Expand file paths
    forecast_file = fs::absolute(fs::path(forecast_file.c_str())).string();
    observation_file = fs::absolute(fs::path(observation_file.c_str())).string();

#if defined(_USE_MPI_EXTENSION)
    if (algorithm == "SSE") {
        throw runtime_error("The MPI implementation for SSE is not provided yet.");
    }

    if (fcst_stations_subset.size() != 0) {
        throw runtime_error("The MPI implementation for subsetting stations is not provided yet.");
    }

    if (fcst_station_start != obs_station_start || fcst_station_count != obs_station_count) {
        throw runtime_error("Reading different subsets of forecast and observation stations are not allowed with MPI!");
    }

    if (fcst_station_start != -1) {
        throw runtime_error("Currently, MPI must read from the beginning of the stations");
    }

    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);
    if (provided != MPI_THREAD_FUNNELED) throw runtime_error("The MPI implementation does not provide MPI_THREAD_FUNNELED");


    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    if (fcst_station_count == -1) {
        fcst_station_start = 0;
        obs_station_start = 0;

        fcst_station_count = Ncdf::readDimLength(forecast_file, Config::_DIM_STATIONS);
        obs_station_count = fcst_station_count;
    }

    // Determine what stations to process for this rank. In this case, our master can
    // also do work. So we need to add 1 to the world size when using the functions from
    // Functions to take into consideration the fact that These functions assume that master 
    // process does not do any work.
    //
    fcst_station_start += Functions::getStartIndex(fcst_station_count, world_size + 1, world_rank + 1);
    fcst_station_count = Functions::getSubTotal(fcst_station_count, world_size + 1, world_rank + 1);

    obs_station_start = fcst_station_start;
    obs_station_count = fcst_station_count;

    // Determine the new output file name
    stringstream padded_rank;
    padded_rank << "_Rank" << std::setw(to_string(world_size).length()) << std::setfill('0') << world_rank;
    fileout = boost::filesystem::change_extension(fileout, "").string() + padded_rank.str() + string(".nc");

    if (config.verbose >= Verbose::Detail) cout << "Rank " << world_rank << "/" << world_size <<
            " processes " << fcst_station_count << " stations [:] from #" << fcst_station_start << " will be writing to " << fileout << endl;
#endif

    runAnEnNcdf(forecast_file, observation_file, fcst_station_start, fcst_station_count, fcst_stations_subset, obs_station_start, obs_station_count,
            obs_id, test_start, test_end, test_times_str, search_start, search_end, fileout, 
            algorithm, config, overwrite, profile, save_tests, unwrap_obs, convert_wind,
            u_names, v_names, spd_names, dir_names, embedding_model, similarity_model, ai_flt_radius, fcst_grid_file);

#if defined(_USE_MPI_EXTENSION)
    MPI_Finalize();
#endif

    return 0;
}

