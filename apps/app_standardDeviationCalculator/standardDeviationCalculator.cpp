/* 
 * File:   standardDeviationCalculator.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on November 29, 2018, 17:30
 */

/** @file */

#include "MathFunctions.h"
#include "AnEnIO.h"
#include "CommonExeFunctions.h"

#include "boost/program_options.hpp"
#include "boost/multi_array.hpp"
#include "boost/filesystem.hpp"
#include "boost/exception/all.hpp"

#include <functional>
#include <iostream>
#include <sstream>
#include <cmath>

using namespace std;

void runStandardDeviationCalculator(
        const vector<string> & in_files,
        const vector<size_t> & starts,
        const vector<size_t> & counts,
        const string & out_file,
        int verbose) {
    
    // Check start and count
    bool partial_read = true;
    if (starts.size() == 0 || counts.size() == 0) {
        partial_read = false;
        
    } else {
        if (!(starts.size() == 4 * in_files.size()
                && counts.size() == 4 * in_files.size())) {
            cout << BOLDRED << "Error: start and count should both have an integer multiplication of 4 values."
                    << RESET << endl;
            return;
        }
    }

    // Create an output device
    AnEnIO io_out("Write", out_file, "StandardDeviation", verbose);

    // Reshape data
    Forecasts_array forecasts;
    if (verbose >= 3)
        cout << GREEN << "Combining forecasts along the time dimension..." << RESET << endl;

    if (partial_read) {
        if (AnEnIO::combineForecastsArray(in_files, forecasts, 2, verbose, starts, counts) !=
                errorType::SUCCESS) {
            throw runtime_error("Error: Failed when combining forecasts.");
        }
    } else {
        if (AnEnIO::combineForecastsArray(in_files, forecasts, 2, verbose) !=
                errorType::SUCCESS) {
            throw runtime_error("Error: Failed when combining forecasts.");
        }
    }
    
    // Compute standard deviation
    if (verbose >= 3)
        cout << GREEN << "Compute standard deviation ..." << RESET << endl;
    MathFunctions functions(verbose);
    StandardDeviation sds;
    handleError(functions.computeStandardDeviation(forecasts, sds));

    // Write standard deviation
    if (verbose >= 3) cout << GREEN
            << "Write the standard deviation ..." << RESET << endl;

    handleError(io_out.writeStandardDeviation(sds,
            forecasts.getParameters(),
            forecasts.getStations(),
            forecasts.getFLTs()));
    
    if (verbose >= 3)
        cout << GREEN << "Done!" << RESET << endl;
    
    return;
}

int main(int argc, char** argv) {

#if defined(_ENABLE_MPI)
    AnEnIO::handle_MPI_Init();
#endif

    namespace po = boost::program_options;

    // Required variables
    vector<string> in_files;
    string out_file;

    // Optional variables
    int verbose = 0;
    string config_file;
    vector<size_t> starts, counts;

    try {
        po::options_description desc("Available options");
        desc.add_options()
                ("help,h", "Print help information for options.")
                ("config,c", po::value<string>(&config_file), "Set the configuration file path. Command line options overwrite options in configuration file. ")

                ("in,i", po::value< vector<string> >(&in_files)->required()->multitoken(), "Set the input forecast NetCDF files to read.")
                ("out,o", po::value<string>(&out_file)->required(), "Set the output NetCDF file to store the standard deviation.")

                ("verbose,v", po::value<int>(&verbose)->default_value(2), "Set the verbose level.")
                ("start", po::value< vector<size_t> >(&starts)->multitoken(), "Set the start indices for each file.")
                ("count", po::value< vector<size_t> >(&counts)->multitoken(), "Set the count numbers for each file.");
        
        // process unregistered keys and notify users about my guesses
        vector<string> available_options;
        auto lambda = [&available_options](const boost::shared_ptr<boost::program_options::option_description> option) {
            available_options.push_back("--" + option->long_name());
        };
        for_each(desc.options().begin(), desc.options().end(), lambda);

        // Parse the command line options first
        po::variables_map vm;
        po::parsed_options parsed = po::command_line_parser(argc, argv).options(desc).allow_unregistered().run();
        store(parsed, vm);

        cout << BOLDGREEN << "Parallel Ensemble Forecasts --- Standard Deviation Calculator "
            << _APPVERSION << RESET << endl << GREEN << _COPYRIGHT_MSG << RESET << endl;

        if (vm.count("help") || argc == 1) {
            cout << desc << endl;
            return 0;
        }

        auto unregistered_keys = po::collect_unrecognized(parsed.options, po::exclude_positional);
        if (unregistered_keys.size() != 0) {
            guess_arguments(unregistered_keys, available_options);
            return 1;
        }

        // Then parse the configuration file
        if (vm.count("config")) {
            // If configuration file is specified, read it first.
            // The variable won't be written until we call notify.
            //
            config_file = vm["config"].as<string>();
        }

        if (!config_file.empty()) {
            ifstream ifs(config_file.c_str());
            if (!ifs) {
                cout << BOLDRED << "Error: Can't open configuration file " << config_file << RESET << endl;
                return 1;
            } else {
                auto parsed_config = parse_config_file(ifs, desc, true);

                auto unregistered_keys_config = po::collect_unrecognized(parsed_config.options, po::exclude_positional);
                if (unregistered_keys_config.size() != 0) {
                    guess_arguments(unregistered_keys_config, available_options);
                    return 1;
                }

                store(parsed_config, vm);
            }
        }
        
        notify(vm);
    } catch (...) {
        handle_exception(current_exception());
        return 1;
    }
    
    if (verbose >= 5) {
        cout << "Input parameters:" << endl
                << "in_files: " << in_files << endl
                << "out_file: " << out_file << endl
                << "verbose: " << verbose << endl
                << "config_file: " << config_file << endl
                << "start: " << starts << endl
                << "count: " << counts << endl;
    }
    
    try{
        runStandardDeviationCalculator(in_files, starts, counts, out_file, verbose);
    } catch (...) {
        handle_exception(current_exception());
        return 1;
    }

#if defined(_ENABLE_MPI)
    AnEnIO::handle_MPI_Finalize();
#endif
    
    return 0;
}
