/* 
 * File:   grib_convert.cpp
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

void runGribConvert(
        const vector<string> & forecast_files,
        const string & regex_str,
        const vector<ParameterGrib> & grib_parameters,
        const vector<int> & stations_index,
        const string & fileout,
        size_t unit_in_seconds,
        bool delimited,
        bool overwrite,
        bool collapse_lead_times,
        Verbose verbose,
#if defined(_USE_MPI_EXTENSION)
        Verbose worker_verbose,
#endif
        bool convert_wind,
        const string & u_name,
        const string & v_name,
        const string & spd_name,
        const string & dir_name) {

    /*
     * Read files
     */
#if defined(_USE_MPI_EXTENSION)
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    if (world_rank == 0) {
#endif
        if (verbose >= Verbose::Detail) {
            cout << "The master process will create " << Functions::getAvailableThreads() << " threads during multi-thread parallelization" << endl;
        }
#if defined(_USE_MPI_EXTENSION)
    }

    AnEnReadGribMPI anen_read(verbose, worker_verbose);
#else
    AnEnReadGrib anen_read(verbose);
#endif

    ForecastsPointer forecasts;
    anen_read.readForecasts(forecasts, grib_parameters, forecast_files,
            regex_str, unit_in_seconds, delimited, stations_index);

#if defined(_USE_MPI_EXTENSION)
    if (world_rank != 0) return;
#endif

    if (convert_wind) forecasts.windTransform(u_name, v_name, spd_name, dir_name);

    /*
     * Collapse the lead time dimension in forecasts if this is intended.
     * This is helpful in cases that model analysis files are used as 
     * an observation alternative.
     */
    ObservationsPointer observations;
    if (collapse_lead_times) {
        if (verbose >= Verbose::Progress) cout << "Collapsing forecast analysis ..." << endl;
        Functions::collapseLeadTimes(observations, forecasts);
    }

    /*
     * Write NetCDF
     */
    AnEnWriteNcdf anen_write(verbose);
    if (collapse_lead_times) anen_write.writeObservations(fileout, observations, overwrite);
    else anen_write.writeForecasts(fileout, forecasts, overwrite);

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

    string forecast_folder, regex_str;
    string fileout, time_start_str, time_end_str, u_name, v_name, spd_name, dir_name; 
    bool delimited, overwrite, collapse_lead_times, convert_wind;
    size_t unit_in_seconds;
    Verbose verbose;

#if defined(_USE_MPI_EXTENSION)
    Verbose worker_verbose;
#endif

    // Define available command line parameters
    options_description desc("Available options");
    desc.add_options()
            ("help,h", "Print help information for options.")
            ("config,c", value< vector<string> >(&config_files)->multitoken(), "Config files (.cfg). Command line options overwrite config files.")
            ("forecasts-folder", value<string>(&forecast_folder)->multitoken()->required(), "Folder for forecast GRIB files.")
            ("regex", value<string>(&regex_str), "Regular expression for file names. The expression should have named groups for 'day', 'flt', and 'cycle'. An example is '.*nam_218_(?P<day>\\d{8})_(?P<cycle>\\d{2})\\d{2}_(?P<flt>\\d{3})\\.grb2$'")
            ("pars-name", value< vector<string> >(&parameters_name)->multitoken()->required(), "Parameters name.")
            ("pars-circular", value < vector<bool> >(&parameters_circular)->multitoken(), "[Optional] 1 for circular parameters and 0 for linear circulars.")
            ("pars-id", value< vector<long> >(&parameters_id)->multitoken()->required(), "Parameters ID.")
            ("levels", value< vector<long> >(&parameters_level)->multitoken()->required(), "Vertical level for each parameter ID.")
            ("level-types", value< vector<string> >(&parameters_level_type)->multitoken()->required(), "Level type for parameter ID.")
            ("out", value<string>(&fileout)->required(), "Output file path.")
            ("stations-index", value< vector<int> >(&stations_index)->multitoken(), "[Optional] Stations index to be read from files.")
            ("time-start", value<string>(&time_start_str), "[Optional] Start date time for reading.")
            ("time-end", value<string>(&time_end_str), "[Optional] End date time for reading.")
            ("delimited", bool_switch(&delimited)->default_value(false), "[Optional] Date strings in forecasts and analysis have separators.")
            ("overwrite", bool_switch(&overwrite)->default_value(false), "[Optional] Overwrite files and variables.")
            ("collapse-lead-times", bool_switch(&collapse_lead_times)->default_value(false), "[Optional] Collapse forecast lead times. This is helpful when you are reading and converting model analysis files to observation NetCDF files.")
            ("unit-in-seconds", value<size_t>(&unit_in_seconds)->default_value(3600), "[Optional] The number of seconds for the unit of lead times. Usually lead times have hours as unit, so it defaults to 3600.")
            ("verbose,v", value<int>()->default_value(1), "[Optional] Verbose level (0 - 4).")
#if defined(_USE_MPI_EXTENSION)
            ("worker-verbose", value<int>()->default_value(1), "[Optional] Verbose level for worker processes (0 - 4).")
#endif
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
            << "Parallel Analogs Ensemble -- grib_convert_mpi "
#else
            << "Parallel Analogs Ensemble -- grib_convert "
#endif
            << _APPVERSION << endl << _COPYRIGHT_MSG << endl << endl
            << desc << endl;
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
        verbose = Functions::itov(vm["verbose"].as<int>());
    }

#if defined(_USE_MPI_EXTENSION)
    if (vm.count("worker-verbose")) {
        worker_verbose = Functions::itov(vm["worker-verbose"].as<int>());
    }
#endif

    if (verbose >= Verbose::Progress) {
        cout
#if defined(_USE_MPI_EXTENSION)
            << "Parallel Analogs Ensemble -- grib_convert_mpi "
#else
            << "Parallel Analogs Ensemble -- grib_convert "
#endif
            << _APPVERSION << endl << _COPYRIGHT_MSG << endl;
    }


    /**************************************************************************
     *                        Run grib convert                                *
     **************************************************************************/

    // Convert parameter settings to a vector of parameters
    vector<ParameterGrib> grib_parameters;
    FunctionsIO::toParameterVector(grib_parameters,
            parameters_name, parameters_id, parameters_level,
            parameters_level_type, parameters_circular, verbose);

    // List files from folders
    vector<string> forecast_files, analysis_files;
    FunctionsIO::listFiles(forecast_files, forecast_folder, regex_str);

    if (forecast_files.size() == 0) throw runtime_error("No forecast files detected. Check --forecasts-folder and --regex.");
    if (verbose >= Verbose::Detail) cout << forecast_files.size()
            << " forecasts files extracted based on the regular expression." << endl;

#if defined(_USE_MPI_EXTENSION)
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);
    if (provided != MPI_THREAD_FUNNELED) throw runtime_error("The MPI implementation does not provide MPI_THREAD_FUNNELED");
#endif

    runGribConvert(forecast_files, regex_str, grib_parameters, stations_index, fileout,
            unit_in_seconds, delimited, overwrite, collapse_lead_times, verbose,
#if defined(_USE_MPI_EXTENSION)
            worker_verbose,
#endif
            convert_wind, u_name, v_name, spd_name, dir_name);

#if defined(_USE_MPI_EXTENSION)
    MPI_Finalize();
#endif
    return 0;
}

