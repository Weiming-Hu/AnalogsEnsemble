/* 
 * File:   anen_transform.cpp
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

using namespace std;
using namespace boost::program_options;
namespace fs = boost::filesystem;


void runTransformation(
        const string & forecast_file,
        const string & forecast_grid_file,
        const string & embedding_model,
        const string & out_file,
        long int flt_radius,
        Verbose verbose,
        bool overwrite) {

    /*
     * Read forecasts
     */

    AnEnReadNcdf anen_read(verbose);
    ForecastsPointer forecasts;
    anen_read.readForecasts(forecast_file, forecasts);

    /*
     * Set up forecast grid if specified
     */

    if (!forecast_grid_file.empty()) forecasts.setGrid(forecast_grid_file, verbose);

    /*
     * Transform forecast features
     */

    forecasts.featureTransform(embedding_model, verbose, flt_radius);

    /*
     * Write latent features
     */

    AnEnWriteNcdf anen_write(verbose);
    anen_write.writeForecasts(out_file, forecasts, overwrite, false);

    return;
}


int main(int argc, char** argv) {

    // Initialization
    string forecast_file, forecast_grid_file, embedding_model, out_file;
    vector<string> config_files;
    long int flt_radius;
    bool overwrite;
    int verbose;

    // Set up arguments
    options_description desc("Available options");
    desc.add_options()
            ("help,h", "Print help information for options.")
            ("config,c", value< vector<string> >(&config_files)->multitoken(), "Config files (.cfg). Command line options overwrite config files.")

            ("forecast-file", value<string>(&forecast_file)->required(), "An NetCDF file for forecasts")
            ("ai-embedding", value<string>(&embedding_model)->required(), "The pretrained AI model")
            ("out", value<string>(&out_file)->required(), "Output file path")

            ("ai-flt-radius", value<long int>(&flt_radius)->default_value(1), "[Optional] The number of lead time radius used for AI embedding.")
            ("grid-file", value<string>(&forecast_grid_file)->default_value(""), "[Optional] A forecast grid file")
            ("overwrite", bool_switch(&overwrite)->default_value(false), "[Optional] Overwrite files")
            ("verbose,v", value<int>(&verbose), "[Optional] Verbose level (0 - 4)");

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
        cout << "Parallel Analogs Ensemble -- anen_transform " << _APPVERSION << endl << _COPYRIGHT_MSG << endl << endl << desc << endl;
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

    runTransformation(forecast_file, forecast_grid_file, embedding_model, out_file, flt_radius, Functions::itov(verbose), overwrite);

    return 0;
}
