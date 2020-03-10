/* 
 * File:   anen_grib.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on February, 2020, 11:10 AM
 */

/** @file */

// Needed for ifstream
#include <fstream>

#include "boost/program_options.hpp"
#include "boost/filesystem.hpp"

#include "Config.h"
#include "AnEnIS.h"
#include "AnEnSSE.h"
#include "Profiler.h"
#include "AnEnWriteNcdf.h"
#include "ForecastsPointer.h"
#include "ObservationsPointer.h"

#if defined(_USE_MPI_EXTENSION)
#include "AnEnReadGribMPI.h"
#else 
#include "AnEnReadGrib.h"
#endif


using namespace std;
using namespace boost::program_options;

void runAnEnGrib(
        const vector<string> & forecast_files,
        const vector<string> & analysis_files,
        const string & forecast_regex,
        const string & analysis_regex,
        const vector<size_t> & obs_id,
        const vector<ParameterGrib> & grib_parameters,
        const vector<int> & stations_index,
        const string & test_start_str,
        const string & test_end_str,
        const string & search_start_str,
        const string & search_end_str,
        const string & fileout,
        const string & algorithm,
        Config & config,
        size_t unit_in_seconds,
        bool delimited,
        bool overwrite,
        bool profile,
        bool save_tests,
        bool convert_wind,
        const string & u_name,
        const string & v_name,
        const string & spd_name,
        const string & dir_name) {


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

    /*
     * Read forecasts from files
     */
#if defined(_USE_MPI_EXTENSION)
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    AnEnReadGribMPI anen_read(config.verbose, config.worker_verbose);
#else
    AnEnReadGrib anen_read(config.verbose);
#endif

    ForecastsPointer forecasts;
    anen_read.readForecasts(forecasts, grib_parameters, forecast_files, forecast_regex,
            unit_in_seconds, delimited, stations_index);

    profiler.log_time_session("reading forecasts");

    /*
     * Read forecast analysis from files and convert them to observations
     */
    ForecastsPointer analysis;
    anen_read.readForecasts(analysis, grib_parameters, analysis_files, analysis_regex,
            unit_in_seconds, delimited, stations_index);

#if defined(_USE_MPI_EXTENSION)
    // Terminate the process if this is not a master process.
    // Subsequent parallelization is done with multi-threading.
    //
    if (world_rank != 0) {
        MPI_Finalize();
        exit(0);
    }
#endif

    profiler.log_time_session("reading analysis");

    // Convert wind parameters if necessary
    if (convert_wind) {
        forecasts.windTransform(u_name, v_name, spd_name, dir_name);
        analysis.windTransform(u_name, v_name, spd_name, dir_name);
    }

    // Convert analysis to observations
    if (config.verbose >= Verbose::Progress) cout << "Collapsing forecast analysis ..." << endl;
    ObservationsPointer observations;
    Functions::collapseLeadTimes(observations, analysis);

    // Convert string date times to Times objects
    const Times & forecast_times = forecasts.getTimes();
    Times test_times, search_times;

    forecast_times(test_start, test_end, test_times);
    forecast_times(search_start, search_end, search_times);

    profiler.log_time_session("preprocessing");


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
        if (config.verbose >= Verbose::Progress) cout << "Configuring multivariate analogs ..." << endl;

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

    profiler.log_time_session("generating analogs");


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

        if (algorithm == "IS") {
            
            AnEnIS* anen_is = dynamic_cast<AnEnIS *> (anen);
            anen_write.writeMultiAnEn(fileout, obs_map, *anen_is, test_times, search_times,
                    forecast_flts, forecast_parameters, forecast_stations, observations, overwrite);
            
        } else if (algorithm == "SSE") {
            
            AnEnSSE* anen_sse = dynamic_cast<AnEnSSE *> (anen);
            anen_write.writeMultiAnEn(fileout, obs_map, *anen_sse, test_times, search_times,
                    forecast_flts, forecast_parameters, forecast_stations, observations, overwrite);
            
        } else {
            throw runtime_error("The algorithm is not recognized");
        }
        
        profiler.log_time_session("writing multivariate analogs");
        
    } else {
        
        /*
         * If we are generating univariate analogs
         */

        if (algorithm == "IS") {
            
            AnEnIS* anen_is = dynamic_cast<AnEnIS *> (anen);
            anen_write.writeAnEn(fileout, *anen_is, test_times, search_times,
                    forecast_flts, forecast_parameters, forecast_stations, overwrite);
            
        } else if (algorithm == "SSE") {
            
            AnEnSSE* anen_sse = dynamic_cast<AnEnSSE *> (anen);
            anen_write.writeAnEn(fileout, *anen_sse, test_times, search_times,
                    forecast_flts, forecast_parameters, forecast_stations, overwrite);
            
        } else {
            throw runtime_error("The algorithm is not recognized");
        }
    }


    /*
     * Save test observations and forecasts
     */
    if (save_tests) {

        // Create test forecasts
        ForecastsPointer test_forecasts(
                forecasts.getParameters(), forecasts.getStations(),
                test_times, forecasts.getFLTs());

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
            ObservationsPointer test_observations(
                    observations.getParameters(),
                    observations.getStations(),
                    test_obs_times);

            // Copy subset values from original observations
            observations.subset(test_observations);

            // Save subset observations
            anen_write.writeObservations(fileout, test_observations, false, true);
        }
    }

    /*
     * Housekeeping
     */
    delete anen;

    profiler.log_time_session("writing results");
    if (profile) profiler.summary(cout);

    return;
}

int main(int argc, char** argv) {

    /**************************************************************************
     *                               Parse arguments                          *
     **************************************************************************/

    // Define variables to be parsed and extracted
    vector<string> config_files, parameters_level_type, parameters_name;
    vector<long> parameters_id, parameters_level;
    vector<bool> parameters_circular;
    vector<int> stations_index;
    vector<double> weights;
    vector<size_t> obs_id;

    string forecast_folder, analysis_folder, test_start, test_end, search_start, search_end;
    string forecast_regex, analysis_regex, fileout, algorithm, ext, u_name, v_name, spd_name, dir_name;
    bool delimited, overwrite, profile, save_tests, convert_wind;
    size_t unit_in_seconds;
    int verbose;

#if defined(_USE_MPI_EXTENSION)
    int worker_verbose;
#endif

    Config config;

    // Define available command line parameters
    options_description desc("Available options");
    desc.add_options()
            ("help,h", "Print help information for options.")
            ("config,c", value< vector<string> >(&config_files)->multitoken(), "Config files (.cfg). Command line options overwrite config files.")
            ("forecasts-folder", value<string>(&forecast_folder)->multitoken()->required(), "Folder for forecast GRIB files.")
            ("analysis-folder", value<string>(&analysis_folder)->multitoken()->required(), "Folder for analysis GRIB files.")
            ("ext", value<string>(&ext)->default_value(".grb2"), "[Optional] GRIB file extension")
            ("forecast-regex", value<string>(&forecast_regex)->required(), "Regular expression for forecast file names. The expression should have named groups for 'day', 'flt', and 'cycle'. An example is '.*nam_218_(?P<day>\\d{8})_(?P<cycle>\\d{2})\\d{2}_(?P<flt>\\d{3})\\.grb2$'")
            ("analysis-regex", value<string>(&analysis_regex)->required(), "Regular expression for analysis file names with the same format as for forecasts.")
            ("pars-name", value< vector<string> >(&parameters_name)->multitoken()->required(), "Parameters name.")
            ("pars-circular", value < vector<bool> >(&parameters_circular)->multitoken(), "[Optional] 1 for circular parameters and 0 for linear circulars.")
            ("pars-id", value< vector<long> >(&parameters_id)->multitoken()->required(), "Parameters ID.")
            ("levels", value< vector<long> >(&parameters_level)->multitoken()->required(), "Vertical level for each parameter ID.")
            ("level-types", value< vector<string> >(&parameters_level_type)->multitoken()->required(), "Level type for parameter ID.")
            ("weights", value< vector<double> >(&weights)->multitoken(), "[Optional] Weight for each parameter ID.")
            ("stations-index", value< vector<int> >(&stations_index)->multitoken(), "[Optional] Stations index to be read from files.")
            ("test-start", value<string>(&test_start)->required(), "Start date time for test with the format YYYY-mm-dd HH:MM:SS")
            ("test-end", value<string>(&test_end)->required(), "End date time for test.")
            ("search-start", value<string>(&search_start)->required(), "Start date time for search.")
            ("search-end", value<string>(&search_end)->required(), "End date time for search.")
            ("out", value<string>(&fileout)->required(), "Output file path.")
            ("algorithm", value<string>(&algorithm)->default_value("IS"), "[Optional] IS for Independent Search or SSE for Search Space Extension")
            ("delimited", bool_switch(&delimited)->default_value(false), "[Optional] Date strings in forecasts and analysis have separators.")
            ("overwrite", bool_switch(&overwrite)->default_value(false), "[Optional] Overwrite files and variables.")
            ("profile", bool_switch(&profile)->default_value(false), "[Optional] Print profiler's report.")
            ("unit-in-seconds", value<size_t>(&unit_in_seconds)->default_value(3600), "[Optional] The number of seconds for the unit of lead times. Usually lead times have hours as unit, so it defaults to 3600.")
            ("verbose,v", value<int>(&verbose)->default_value(2), "[Optional] Verbose level (0 - 4).")
#if defined(_USE_MPI_EXTENSION)
            ("worker-verbose", value<int>(&worker_verbose)->default_value(2), "[Optional] Verbose level for worker processes (0 - 4).")
#endif
            ("analogs", value<size_t>(&(config.num_analogs)), "[Optional] Number of analogs members.")
            ("sims", value<size_t>(&(config.num_sims)), "[Optional] Number of similarity members.")
            ("obs-id", value< vector<size_t> >(&obs_id)->multitoken(), "[Optional] Observation variable index. If multiple indices are provided, multivariate analogs will be generated.")
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
            << "Parallel Analogs Ensemble -- anen_grib_mpi "
#else
            << "Parallel Analogs Ensemble -- anen_grib "
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

    if (vm.count("worker-verbose")) {
        config.setWorkerVerbose(worker_verbose);
    }

    if (config.verbose >= Verbose::Progress) {
        cout
#if defined(_USE_MPI_EXTENSION)
            << "Parallel Analogs Ensemble -- anen_grib_mpi "
#else
            << "Parallel Analogs Ensemble -- anen_grib "
#endif
            << _APPVERSION << endl << _COPYRIGHT_MSG << endl;
    }


    /**************************************************************************
     *                   Run analog generation with grib files                *
     **************************************************************************/

    // Convert parameters
    vector<ParameterGrib> grib_parameters;
    FunctionsIO::toParameterVector(grib_parameters,
            parameters_name, parameters_id, parameters_level,
            parameters_level_type, parameters_circular, config.verbose);

    // List files from folders
    vector<string> forecast_files, analysis_files;
    FunctionsIO::listFiles(forecast_files, forecast_folder, ext);
    FunctionsIO::listFiles(analysis_files, analysis_folder, ext);
    
    if (forecast_files.size() == 0) throw runtime_error("No forecast files detected");
    if (analysis_files.size() == 0) throw runtime_error("No analysis files detected");
    
    if (config.verbose >= Verbose::Debug) {
        cout << "Forecast files:" << endl
                << Functions::format(forecast_files, "\n") << endl
                << "Analysis files:" << endl
                << Functions::format(analysis_files, "\n") << endl;
    }

#if defined(_USE_MPI_EXTENSION)
    MPI_Init(&argc, &argv);
#endif

    runAnEnGrib(forecast_files, analysis_files,
            forecast_regex, analysis_regex,
            obs_id, grib_parameters, stations_index, test_start, test_end, search_start, search_end,
            fileout, algorithm, config, unit_in_seconds, delimited, overwrite, profile, save_tests,
            convert_wind, u_name, v_name, spd_name, dir_name);

#if defined(_USE_MPI_EXTENSION)
    MPI_Finalize();
#endif

    return 0;
}
