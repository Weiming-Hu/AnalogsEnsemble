/* 
 * File:   windFieldCalculator.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on October 2, 2018, 9:20 PM
 */

/** @file */

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

#ifndef M_PI
#define M_PI 3.14159265358979323846  /* pi */
#endif

using namespace std;

void runWindFieldCalculator(
        const string & file_in, const string & file_type,
        const string & file_out,
        const string & var_U_name, const string & var_V_name,
        const string & var_dir_name, const string & var_speed_name,
        int verbose) {

    // Define the functions to compute wind speed
    if (verbose >= 3) cout << GREEN << "Define functions for computation ..." << RESET << endl;
    function< double (double, double) > func_speed =
            [](double d1, double d2) {
                return (sqrt(d1 * d1 + d2 * d2));
            };

    // Define the function to compute wind direction
    function< double (double, double) > func_dir =
            [] (double d1, double d2) {
                double dir = fmod(atan2(d1, d2) * 180 / M_PI, 360);
                return (dir < 0 ? (dir + 360) : dir);
            };

    // Define parameters
    anenPar::Parameter windSpeed(var_speed_name, false),
            windDir(var_dir_name, true);

    size_t num_parameters, num_stations, num_times, num_flts,
            i_U, i_V, i_speed, i_dir;

    AnEnIO io("Read", file_in, file_type, verbose);

    if (file_type == "Forecasts") {

        if (verbose >= 3) cout << GREEN << "Processing forecasts file ..." << RESET << endl;

        Forecasts_array forecasts;
        io.handleError(io.readForecasts(forecasts));

        auto & parameters = forecasts.getParameters();
        parameters.push_back(windSpeed);
        parameters.push_back(windDir);

        i_U = parameters.getParameterIndex(
                parameters.getParameterByName(var_U_name).getID());
        i_V = parameters.getParameterIndex(
                parameters.getParameterByName(var_V_name).getID());
        i_speed = parameters.getParameterIndex(
                parameters.getParameterByName(var_speed_name).getID());
        i_dir = parameters.getParameterIndex(
                parameters.getParameterByName(var_dir_name).getID());

        num_parameters = forecasts.getParametersSize();
        num_stations = forecasts.getStationsSize();
        num_times = forecasts.getTimesSize();
        num_flts = forecasts.getFLTsSize();

        auto & data = forecasts.data();

        // Because we are enlarging the size, the original values are kept.
        if (verbose >= 3) cout << GREEN << "Resizing data ..." << RESET << endl;
        forecasts.updateDataDims(false);

        if (verbose >= 3) cout << GREEN << "Computing ..." << RESET << endl;
#ifdef _OPENMP
#pragma omp parallel for default(none) schedule(static) collapse(3) \
shared(num_stations, num_times, num_flts, data, i_speed, i_U, i_V, i_dir, \
func_dir, func_speed)
#endif
        for (size_t j = 0; j < num_stations; j++) {
            for (size_t k = 0; k < num_times; k++) {
                for (size_t l = 0; l < num_flts; l++) {
                    data[i_speed][j][k][l] = func_speed(data[i_U][j][k][l], data[i_V][j][k][l]);
                    data[i_dir][j][k][l] = func_dir(data[i_U][j][k][l], data[i_V][j][k][l]);
                }
            }
        }

        if (verbose >= 3) cout << GREEN << "Writing file " << file_out << " ... " << RESET << endl;
        io.setMode("Write", file_out);
        io.handleError(io.writeForecasts(forecasts));

    } else if (file_type == "Observations") {

        if (verbose >= 3) cout << GREEN << "Processing forecasts file ..." << RESET << endl;

        Observations_array observations;
        io.handleError(io.readObservations(observations));

        auto & parameters = observations.getParameters();

        parameters.push_back(windSpeed);
        parameters.push_back(windDir);

        i_U = parameters.getParameterIndex(
                parameters.getParameterByName(var_U_name).getID());
        i_V = parameters.getParameterIndex(
                parameters.getParameterByName(var_V_name).getID());
        i_speed = parameters.getParameterIndex(
                parameters.getParameterByName(var_speed_name).getID());
        i_dir = parameters.getParameterIndex(
                parameters.getParameterByName(var_dir_name).getID());

        num_parameters = observations.getParametersSize();
        num_stations = observations.getStationsSize();
        num_times = observations.getTimesSize();

        auto & data = observations.data();

        // Because we are enlarging the size, the original values are kept.
        if (verbose >= 3) cout << GREEN << "Resizing data ..." << RESET << endl;
        observations.updateDataDims(false);

        if (verbose >= 3) cout << GREEN << "Computing ..." << RESET << endl;
#ifdef _OPENMP
#pragma omp parallel for default(none) schedule(static) collapse(2) \
shared(num_stations, num_times, data, i_speed, i_U, i_V, i_dir, \
func_dir, func_speed)
#endif
        for (size_t j = 0; j < num_stations; j++) {
            for (size_t k = 0; k < num_times; k++) {
                data[i_speed][j][k] = func_speed(data[i_U][j][k], data[i_V][j][k]);
                data[i_dir][j][k] = func_dir(data[i_U][j][k], data[i_V][j][k]);
            }
        }

        if (verbose >= 3) cout << GREEN << "Writing file " << file_out << " ... " << RESET << endl;
        io.setMode("Write", file_out);
        io.handleError(io.writeObservations(observations));

    } else {
        stringstream ss;
        ss << BOLDRED << "Error: Unsupported file type " << file_type << RESET;
        throw runtime_error(ss.str());
    }

    return;
}

int main(int argc, char** argv) {

    namespace po = boost::program_options;
    
    // Required variables
    string file_in, file_type, file_out,
            var_U_name, var_V_name;    
    
    // Optional variables
    string var_dir_name, var_speed_name, config_file;
    int verbose = 0;
    
    try {
        po::options_description desc("Available options");
        desc.add_options()
                ("help,h", "Print help information for options.")
                ("config,c", po::value<string>(&config_file), "Set the configuration file path. Command line options overwrite options in configuration file. ")
                
                ("file_in", po::value<string>(&file_in)->required(), "Set the file to read.")
                ("file_type", po::value<string>(&file_type)->required(), "Set the file type. Currently supports Forecasts and Observations.")
                ("file_out", po::value<string>(&file_out)->required(), "Set the file to write.")
                ("U_name,U", po::value<string>(&var_U_name)->required(), "Set the name of the parameter to read for U component of wind.")
                ("V_name,V", po::value<string>(&var_V_name)->required(), "Set the name of the parameter to read for V component of wind.")
                
                ("dir_name", po::value<string>(&var_dir_name)->default_value("windDirection"), "Set the name of the parameter to write for wind direction.")
                ("speed_name", po::value<string>(&var_speed_name)->default_value("windSpeed"), "Set the name of the parameter to write for wind speed.")
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
            cout << BOLDGREEN << "Analog Ensemble program --- Similarity Calculator"
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
            << "file_type: " << file_type << endl
            << "file_out: " << file_out << endl
            << "var_U_name: " << var_U_name << endl
            << "var_V_name: " << var_V_name << endl
            << "var_dir_name: " << var_dir_name << endl
            << "var_speed_name: " << var_speed_name << endl
            << "config_file: " << config_file << endl
            << "verbose: " << verbose << endl;
    }

    try {
        runWindFieldCalculator(file_in, file_type, file_out,
                var_U_name, var_V_name, var_dir_name, var_speed_name, verbose);
    } catch (...) {
        handle_exception(current_exception());
        return 1;
    }
    
    return 0;
}
