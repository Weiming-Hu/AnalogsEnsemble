/* 
 * File:   anen.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on February, 2020, 11:10 AM
 */

/** @file */

#include "boost/program_options.hpp"
#include "boost/filesystem.hpp"

#include "Config.h"
#include "AnEnIS.h"
#include "AnEnSSE.h"
#include "Profiler.h"
#include "AnEnReadGrib.h"
#include "ForecastsPointer.h"
#include "ObservationsPointer.h"
#include "AnEnWriteNcdf.h"

using namespace std;
using namespace boost::program_options;

void runAnEnGrib(
        const vector<string> & forecast_files,
        const vector<string> & analysis_files,
        const string & regex_day_str,
        const string & regex_flt_str,
        const string & regex_cycle_str,
        const vector<ParameterGrib> & grib_parameters,
        const vector<int> & stations_index,
        const string & test_start,
        const string & test_end,
        const string & search_start,
        const string & search_end,
        const string & fileout,
        const string & algorithm,
        const Config & config,
        size_t unit_in_seconds,
        bool delimited,
        bool overwrite,
        bool profile) {


    /**************************************************************************
     *                     Read Forecasts and Analysis                        *
     **************************************************************************/
    
    Profiler profiler;
    profiler.start();

    /*
     * Read forecasts from files
     */
    AnEnReadGrib anen_read(config.verbose);
    ForecastsPointer forecasts;
    anen_read.readForecasts(forecasts, grib_parameters, forecast_files,
            regex_day_str, regex_flt_str, regex_cycle_str,
            unit_in_seconds, delimited, stations_index);
    profiler.log_time_session("reading forecasts");

    /*
     * Read forecast analysis from files and convert them to observations
     */
    ForecastsPointer analysis;
    anen_read.readForecasts(forecasts, grib_parameters, analysis_files,
            regex_day_str, regex_flt_str, regex_cycle_str,
            unit_in_seconds, delimited, stations_index);
    profiler.log_time_session("reading analysis");

    // Convert analysis to observations
    ObservationsPointer observations;
    Functions::collapseLeadTimes(observations, analysis);
    profiler.log_time_session("converting analysis");

    // Convert string date times to Times objects
    const Times & forecast_times = forecasts.getTimes();
    Times test_times, search_times;

    forecast_times(test_start, test_end, test_times, false);
    forecast_times(search_start, search_end, search_times, false);


    /**************************************************************************
     *                       Generate Analogs Ensemble                        *
     **************************************************************************/

    /*
     * Generate analogs
     */
    AnEn* anen = nullptr;

    if (algorithm == "AnEnIS") {
        anen = new AnEnIS(config);
    } else if (algorithm == "AnEnSSE") {
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
     * Write results to disk
     */
    AnEnWriteNcdf anen_write(config.verbose);

    const auto & forecast_flts = forecasts.getFLTs();
    const auto & forecast_parameters = forecasts.getParameters();
    const auto & forecast_stations = forecasts.getStations();

    if (algorithm == "AnEnIS") {
        AnEnIS* anen_is = dynamic_cast<AnEnIS *> (anen);
        anen_write.writeAnEn(fileout, *anen_is, test_times, search_times,
                forecast_flts, forecast_parameters, forecast_stations, overwrite);
        delete anen_is;
    } else if (algorithm == "AnEnSSE") {
        AnEnSSE* anen_sse = dynamic_cast<AnEnSSE *> (anen);
        anen_write.writeAnEn(fileout, *anen_sse, test_times, search_times,
                forecast_flts, forecast_parameters, forecast_stations, overwrite);
        delete anen_sse;
    } else {
        throw runtime_error("The algorithm is not recognized");
    }

    /*
     * Housekeeping
     */
    // Delete the pointer based on the algorithm used
    if (algorithm == "IS") {
        delete dynamic_cast<AnEnIS *> (anen);
    } else if (algorithm == "SSE") {
        delete dynamic_cast<AnEnSSE *> (anen);
    } else {
        throw std::runtime_error("algorithm not recognized");
    }

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

    string forecast_folder, analysis_folder, regex_day_str, regex_flt_str;
    string regex_cycle_str, test_start, test_end, search_start, search_end;
    string fileout, algorithm, ext;
    bool delimited, overwrite, profile;
    size_t unit_in_seconds;
    int verbose;

    Config config;

    // Define available command line parameters
    options_description desc("Available options");
    desc.add_options()
            ("help,h", "Print help information for options.")
            ("config,c", value< vector<string> >(&config_files)->multitoken(), "Config files (.cfg). Command line options overwrite config files.")
            ("forecasts-folder", value<string>(&forecast_folder)->multitoken()->required(), "Folder for forecast GRIB files.")
            ("analysis-folder", value<string>(&analysis_folder)->multitoken()->required(), "Folder for analysis GRIB files.")
            ("ext", value<string>(&ext)->default_value(".grb2"), "[Optional] GRIB file extension")
            ("regex-day", value<string>(&regex_day_str)->required(), "Regular expression for the date. Regular expressions are used on file names.")
            ("regex-flt", value<string>(&regex_flt_str)->required(), "Regular expression for lead times.")
            ("regex-cycle", value<string>(&regex_cycle_str), "[Optional] Regular expression for initialization cycle time.")
            ("pars-name", value< vector<string> >(&parameters_name)->multitoken()->required(), "Parameters name.")
            ("pars-circular", value < vector<bool> >(&parameters_circular)->multitoken(), "[Optional] 1 for circular parameters and 0 for linear circulars.")
            ("pars-id", value< vector<long> >(&parameters_id)->multitoken()->required(), "Parameters ID.")
            ("levels", value< vector<long> >(&parameters_level)->multitoken()->required(), "Vertical level for each parameter ID.")
            ("level-types", value< vector<string> >(&parameters_level_type)->multitoken()->required(), "Level type for parameter ID.")
            ("weights", value< vector<double> >(&weights)->multitoken(), "[Optional] Weight for each parameter ID.")
            ("stations-index", value< vector<int> >(&stations_index)->multitoken(), "[Optional] Stations index to be read from files.")
            ("test-start", value<string>(&test_start)->required(), "Start date time for test with the format YYYY/MM/DD HH:MM:SS")
            ("test-end", value<string>(&test_end)->required(), "End date time for test.")
            ("search-start", value<string>(&search_start)->required(), "Start date time for search.")
            ("search-end", value<string>(&search_end)->required(), "End date time for search.")
            ("out", value<string>(&fileout)->required(), "Output file path.")
            ("algorithm", value<string>(&algorithm)->default_value("AnEnIS"), "[Optional] AnEnIS or AnEnSSE")
            ("delimited", bool_switch(&delimited)->default_value(false), "[Optional] Date strings in forecasts and analysis have separators.")
            ("overwrite", bool_switch(&overwrite)->default_value(false), "[Optional] Overwrite files and variables.")
            ("profile", bool_switch(&profile)->default_value(false), "[Optional] Print profiler's report.")
            ("unit-in-seconds", value<size_t>(&unit_in_seconds)->default_value(3600), "[Optional] The number of seconds for the unit of lead times. Usually lead times have hours as unit, so it defaults to 3600.")
            ("verbose,v", value<int>(&verbose)->default_value(2), "[Optional] Verbose level (0 - 4).")
            ("analogs", value<size_t>(&(config.num_analogs)), "[Optional] Number of analogs members.")
            ("sims", value<size_t>(&(config.num_sims)), "[Optional] Number of similarity members.")
            ("obs-id", value<size_t>(&(config.obs_var_index)), "[Optional] Observation variable index.")
            ("max-par-nan", value<size_t>(&(config.max_par_nan)), "[Optional] Maximum allowed number of NA in parameters.")
            ("max-flt-nan", value<size_t>(&(config.max_flt_nan)), "[Optional] Maximum allowed number of NA in lead times.")
            ("flt-radius", value<size_t>(&(config.flt_radius)), "[Optional] The number of surrounding lead times to compare for trends.")
            ("num-nearest", value<size_t>(&(config.num_nearest)), "[Optional] Number of neighbor stations to search.")
            ("distance", value<double>(&(config.distance)), "[Optional] Distance threshold when searching for neighbors.")
            ("extend-obs", bool_switch(&(config.extend_obs)), "[Optional] Use observations from search stations.")
            ("operation", bool_switch(&(config.operation)), "[Optional] Use operational mode.")
            ("prevent-search-future", bool_switch(&(config.prevent_search_future)), "[Optional] Prevent using observations that are later than the current test forecast.")
            ("extend-obs", bool_switch(&(config.extend_obs)), "[Optional] Whether to use observations from search stations.")
            ("save-analogs", bool_switch(&(config.save_analogs)), "[Optional] Save analogs.")
            ("save-analogs-time-index", bool_switch(&(config.save_analogs_time_index)), "[Optional] Save time indices of analogs.")
            ("save-sims", bool_switch(&(config.save_sims)), "[Optional] Save similarity.")
            ("save-sims-time-index", bool_switch(&(config.save_sims_time_index)), "[Optional] Save time indices of similarity.")
            ("save-sims-station-index", bool_switch(&(config.save_sims_station_index)), "[Optional] Save station indices of similarity.")
            ("quick", bool_switch(&(config.quick_sort)), "[Optional] Use quick sort.");

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
        cout << desc << endl;
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
        config.setVerbose(vm["verbose"].as<int>());
    }

    if (config.verbose >= Verbose::Progress) {
        cout << "Parallel Analogs Ensemble -- anen "
                << _APPVERSION << endl << "GEOlab @ Penn State" << endl;
    }


    /**************************************************************************
     *                   Run analog generation with grib files                *
     **************************************************************************/

    // Convert parameter settings to grib parameters
    bool has_circular = false;
    size_t num_parameters = parameters_id.size();

    if (num_parameters != parameters_level.size()) throw runtime_error("Different numbers of parameters ID and levels");
    if (num_parameters != parameters_name.size()) throw runtime_error("Different numbers of parameters ID and names");
    if (num_parameters != parameters_level_type.size()) throw runtime_error("Different numbers of parameters ID and level types");
    if (parameters_circular.size() != 0) {
        has_circular = true;
        if (num_parameters != parameters_circular.size()) throw runtime_error("Different numbers of parameters ID and circular flags");
    }
    
    // List files from folders
    vector<string> forecast_files, analysis_files;
    FunctionsIO::listFiles(forecast_files, forecast_folder, ext);
    FunctionsIO::listFiles(analysis_files, analysis_folder, ext);

    if (config.verbose >= Verbose::Debug) {
        cout << "Forecast files:" << endl
                << Functions::format(forecast_files, "\n") << endl
                << "Analysis files:" << endl
                << Functions::format(analysis_files, "\n") << endl;
    }

    // Convert parameters
    vector<ParameterGrib> grib_parameters;
    for (size_t parameter_i = 0; parameter_i < num_parameters; ++parameter_i) {
        ParameterGrib grib_parameter(parameters_name[parameter_i],
                Config::_CIRCULAR, parameters_id[parameter_i],
                parameters_level[parameter_i], parameters_level_type[parameter_i]);

        if (has_circular) grib_parameter.setCircular(parameters_circular[parameter_i]);
        grib_parameters.push_back(grib_parameter);
        if (config.verbose >= Verbose::Debug) cout << grib_parameter << endl;
    }

    runAnEnGrib(forecast_files, analysis_files,
            regex_day_str, regex_flt_str, regex_cycle_str,
            grib_parameters, stations_index, test_start, test_end, search_start, search_end,
            fileout, algorithm, config, unit_in_seconds, delimited, overwrite, profile);

    return 0;
}