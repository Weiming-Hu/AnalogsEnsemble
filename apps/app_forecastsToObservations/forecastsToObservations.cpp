/* 
 * File: forecastsToObservations.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on September 17, 2018, 5:20 PM
 */

/** @file */

#include "AnEn.h"
#include "AnEnIO.h"
#include "colorTexts.h"
#include "CommonExeFunctions.h"

#include "boost/program_options.hpp"
#include "boost/filesystem.hpp"
#include "boost/exception/all.hpp"

#include <iostream>

using namespace std;

void runForecastsToObservations( const string & file_in, const string & file_out,
        int time_match_mode, int verbose) {

    if (verbose >= 3) cout << GREEN << "Converting Observations to Forecasts" << RESET << endl;

    // Initialization
    Forecasts_array forecasts;
    Observations_array observations;

    // Read forecasts
    AnEnIO io("Read", file_in, "Forecasts", verbose),
           io_out("Write", file_out, "Observations", verbose);
    io.readForecasts(forecasts);

    // Copy parameters and stations from forecasts to observations
    observations.setParameters(forecasts.getParameters());
    observations.setStations(forecasts.getStations());

    // Create times for observations
    anenTime::Times obs_times, fcsts_times = forecasts.getTimes();
    anenTime::FLTs fcsts_flts = forecasts.getFLTs();
    double time = 0;

    for (size_t i = 0; i < fcsts_times.size(); i++) {
        for (size_t j = 0; j < fcsts_flts.size(); j++) {
            time = fcsts_times[i] + fcsts_flts[j];
            obs_times.push_back(time);
        }
    }

    observations.setTimes(obs_times);
    observations.updateDataDims();

    AnEn anen(verbose);
    AnEn::TimeMapMatrix mapping;
    anen.handleError(anen.computeObservationsTimeIndices(
            forecasts.getTimes(), forecasts.getFLTs(),
            observations.getTimes(), mapping, time_match_mode));

    if (verbose >= 4) cout << "Time mapping from Forecasts to Observations:"
        << endl << mapping << endl;

    if (verbose >= 4)
        cout << "Forecasts Times:" << endl << fcsts_times
            << "Forecasts FLTs: " << fcsts_flts << "Observations Times: "
            << endl << obs_times;

    auto & obs_data = observations.data();
    auto & fcsts_data = forecasts.data();

    size_t num_pars = forecasts.getParametersSize(),
           num_stations = forecasts.getStationsSize(),
           num_times = forecasts.getTimesSize(),
           num_flts = forecasts.getFLTsSize();

#if defined(_OPENMP)
#pragma omp parallel for default(none) schedule(static) shared(num_pars, \
num_stations, num_times, num_flts, obs_data, fcsts_data, mapping) collapse(4)
#endif
    for (size_t i_par = 0; i_par < num_pars; i_par++) {
        for (size_t i_station = 0; i_station < num_stations; i_station++) {
            for (size_t i_time = 0; i_time < num_times; i_time++) {
                for (size_t i_flt = 0; i_flt < num_flts; i_flt++) {
                    obs_data[i_par][i_station][mapping(i_time, i_flt)] =
                        fcsts_data[i_par][i_station][i_time][i_flt];
                }
            }
        }
    }

    // Write observations
    io_out.handleError(io_out.writeObservations(observations));

    if (verbose >= 3) cout << GREEN << "Done!" << RESET << endl;

    return;
}

int main(int argc, char** argv) {

    namespace po = boost::program_options;

    // Required variables
    string file_in, file_out;

    // Optional variables
    int verbose = 0, time_match_mode = 0;
    string config_file;

    try {
        po::options_description desc("Available options");
        desc.add_options()
                ("help,h", "Print help information for options.")
                ("config,c", po::value<string>(&config_file), "Set the configuration file path. Command line options overwrite options in configuration file. ")

                ("in,i", po::value<string>(&file_in)->required(), "Set the Forecasts file to read.")
                ("out,o", po::value<string>(&file_out)->required(), "Set the Observations file to write.")

                ("time-match-mode", po::value<int>(&time_match_mode)->default_value(0), "Set the match mode for generating TimeMapMatrix. 0 for strict and 1 for loose search.")
                ("verbose,v", po::value<int>(&verbose)->default_value(2), "Set the verbose level.");

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

        if (vm.count("help") || argc == 1) {
            cout << GREEN << "Analog Ensemble program --- Forecasts to Observations"
                << RESET << endl << desc << endl;
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

    if (verbose >= 4) {
        cout << "Input parameters: " << endl
            << "file_in: " << file_in << endl
            << "file_out: " << file_out << endl
            << "verbose: " << verbose << endl
            << "time_match_mode: " << time_match_mode << endl
            << "config_file: " << config_file << endl;
    }

    try {
        runForecastsToObservations(file_in, file_out, time_match_mode, verbose);
    } catch (...) {
        handle_exception(current_exception());
        return 1;
    }
    
    return 0;
}


