/* 
 * File: fileSlice.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on June 11, 2019, 10:20 PM
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

void runFileSlice(const string & file_type,
                  const string & in_file,
                  const string & out_file,
                  vector<size_t> & slice_pars,
                  vector<size_t> & slice_stations,
                  vector<size_t> & slice_times,
                  vector<size_t> & slice_flts,
                  int verbose) {
    AnEnIO io_out("Write", out_file, file_type, verbose);

    if (file_type == "Forecasts") {
        AnEnIO io_in("Read", in_file, file_type, verbose);
        Forecasts_array forecasts_in;

        if (verbose >= 3) cout << GREEN << "Reading forecasts ..." << RESET << endl;
        handleError(io_in.readForecasts(forecasts_in));

        size_t max_val = 0;

        // Slice parameters
        anenPar::Parameters parameters;
        if (slice_pars.size() == 0) {
            // If parameters are not sliced, keep all paramteres
            slice_pars.resize(forecasts_in.getParametersSize());
            iota(slice_pars.begin(), slice_pars.end(), 0);

            parameters = forecasts_in.getParameters();

        } else {
            // If parameters are sliced, only read the specified parameters.
            if (verbose >= 3) cout << GREEN << "Slicing parameters ..." << RESET << endl;
            max_val = *(max_element(slice_pars.begin(), slice_pars.end()));
            if (max_val >= forecasts_in.getParametersSize())
                throw runtime_error("Error: Parameter slice index is out of boundary.");

            const auto & parameters_in = forecasts_in.getParameters().get<anenPar::by_insert>();
            for (const auto & i : slice_pars) {
                anenPar::Parameter parameter = parameters_in[i];
                parameters.push_back(parameter);
            }
        }

        // Slice stations
        anenSta::Stations stations;
        if (slice_stations.size() == 0) {
            // If stations are not sliced, keep all stations
            slice_stations.resize(forecasts_in.getStationsSize());
            iota(slice_stations.begin(), slice_stations.end(), 0);

            stations = forecasts_in.getStations();

        } else {
            // If stations are sliced, only read the specified stations.
            if (verbose >= 3) cout << GREEN << "Slicing stations ..." << RESET << endl;
            max_val = *(max_element(slice_stations.begin(), slice_stations.end()));
            if (max_val >= forecasts_in.getStationsSize())
                throw runtime_error("Error: Stations slice index is out of boundary.");

            const auto & stations_in = forecasts_in.getStations().get<anenSta::by_insert>();
            for (const auto & i : slice_stations) {
                anenSta::Station station = stations_in[i];
                stations.push_back(station);
            }
        }

        // Slice times
        anenTime::Times times;
        if (slice_times.size() == 0) {
            // If times are not sliced, keep all times
            slice_times.resize(forecasts_in.getTimesSize());
            iota(slice_times.begin(), slice_times.end(), 0);

            times = forecasts_in.getTimes();

        } else {
            // If times are sliced, only read the specified times.
            max_val = *(max_element(slice_times.begin(), slice_times.end()));
            if (max_val >= forecasts_in.getTimesSize())
                throw runtime_error("Error: Times slice index is out of boundary.");

            if (verbose >= 3) cout << GREEN << "Slicing times ..." << RESET << endl;
            const auto & times_in = forecasts_in.getTimes().get<anenTime::by_insert>();
            for (const auto & i : slice_times) {
                double time = times_in[i];
                times.push_back(time);
            }
        }

        // Slice flts
        anenTime::FLTs flts;
        if (slice_flts.size() == 0) {
            // If FLTs are not sliced, keep all FLTs
            slice_flts.resize(forecasts_in.getFLTsSize());
            iota(slice_flts.begin(), slice_flts.end(), 0);

            flts = forecasts_in.getFLTs();

        } else {
            // If FLTs are sliced, only read the specified FLTs.
            max_val = *(max_element(slice_flts.begin(), slice_flts.end()));
            if (max_val >= forecasts_in.getFLTsSize())
                throw runtime_error("Error: FLTs slice index is out of boundary.");
        
            if (verbose >= 3) cout << GREEN << "Slicing FLTs ..." << RESET << endl;
            const auto & flts_in = forecasts_in.getFLTs().get<anenTime::by_insert>();
            for (const auto & i : slice_flts) {
                double flt = flts_in[i];
                flts.push_back(flt);
            }
        }

        // Create the sliced data object
        Forecasts_array forecasts_out(parameters, stations, times, flts);
        auto & data_out = forecasts_out.data();
        auto & data_in = forecasts_in.data();

        for (size_t i = 0; i < data_out.shape()[0]; i++) {
            for (size_t j = 0; j < data_out.shape()[1]; j++) {
                for (size_t k = 0; k < data_out.shape()[2]; k++) {
                    for (size_t l = 0; l < data_out.shape()[3]; l++) {
                        data_out[i][j][k][l] = data_in
                            [slice_pars[i]][slice_stations[j]][slice_times[k]][slice_flts[l]];
                    }
                }
            }
        }

        if (verbose >= 3) cout << GREEN << "Writing forecasts ..." << RESET << endl;
        handleError(io_out.writeForecasts(forecasts_out));

    } else {
        cerr << BOLDRED << "Error: Only support Forecasts!" << RESET << endl;
    }

    return;
}

int main(int argc, char** argv) {

    namespace po = boost::program_options;

    // Required variables
    string file_type, in_file, out_file;
    vector<size_t> slice_pars, slice_stations, slice_times, slice_flts;

    // Optional variables
    int verbose = 0;
    vector<string> config_files;

    try {
        po::options_description desc("Available options");
        desc.add_options()
                ("help,h", "Print help information for options.")
                ("config,c", po::value< vector<string> >(&config_files)->multitoken(), "Set the configuration files. Command line options overwrite options in configuration file. ")

                ("type,t", po::value<string>(&file_type)->required(), "Set the type of the files to read. It only supports Forecasts for now.")
                ("in,i", po::value<string>(&in_file)->required(), "Set the file name to read.")
                ("out,o", po::value<string>(&out_file)->required(), "Set the name of the output file.")

                ("slice-pars", po::value< vector<size_t> >(&slice_pars)->multitoken(), "Set the index of parameters to keep in the output file.")
                ("slice-stations", po::value< vector<size_t> >(&slice_stations)->multitoken(), "Set the index of stations to keep in the output file.")
                ("slice-times", po::value< vector<size_t> >(&slice_times)->multitoken(), "Set the index of times to keep in the output file.")
                ("slice-flts", po::value< vector<size_t> >(&slice_flts)->multitoken(), "Set the index of forecast lead times to keep in the output file.")
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

        cout << BOLDGREEN << "Parallel Ensemble Forecasts --- File Slice "
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
            config_files = vm["config"].as< vector<string> >();
        }
        
        if (!config_files.empty()) {
            for (const auto & config_file : config_files) {
                ifstream ifs(config_file.c_str());
                if (!ifs) {
                    cerr << BOLDRED << "Error: Can't open configuration file " << config_file << RESET << endl;
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
        }

        notify(vm);
    } catch (...) {
        handle_exception(current_exception());
        return 1;
    }

    if (verbose >= 5) {
        cout << "Input parameters: " << endl
                << "file_type: " << file_type << endl
                << "in_file: " << in_file << endl
                << "out_file: " << out_file << endl
                << "slice_pars: " << slice_pars << endl
                << "slice_stations: " << slice_stations << endl
                << "slice_times: " << slice_times << endl
                << "slice_flts: " << slice_flts << endl
                << "verbose: " << verbose << endl;
    }

    if (slice_pars.size() == 0 && slice_stations.size() == 0 &&
        slice_times.size() == 0 && slice_flts.size() == 0) {
            cerr << BOLDRED << "Error: You need to slice at least one dimension, otherwise you are just copying and pasting." <<
                RESET << endl;
    }
    
    try {
        runFileSlice(file_type, in_file, out_file, slice_pars, slice_stations, slice_times, slice_flts, verbose);
    } catch (...) {
        handle_exception(current_exception());
        return 1;
    }

    return 0;
}
