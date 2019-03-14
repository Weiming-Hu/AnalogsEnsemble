/* 
 * File: fileAggregate.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on November 9, 2018, 10:20 AM
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

#if defined(_OPENMP)
#include <omp.h>
#endif

using namespace std;

void runFileAggregate(const string & file_type, vector<string> & in_files, 
        const size_t & along, const string & out_file, const int & verbose,
        const vector<size_t> & starts, const vector<size_t> & counts) {

    // Check start and count
    bool partial_read = true;
    if (starts.size() == 0 || counts.size() == 0) {
        partial_read = false;
        
    }

    if (partial_read) {
        if (file_type == "Forecasts" || file_type == "Observations") {
            // Currently, the partial read functionality only supports Forecasts and observations.
        } else {
            cerr << BOLDRED << "Error: Currently, the partial read functionality only supports Forecasts and observations."
                << RESET << endl;
            return;
        }
    }

    // Sort the files
    sort(in_files.begin(), in_files.end());
    if (verbose >= 4) cout << "The sorted files are " << in_files << endl;

    AnEnIO io_out("Write", out_file, file_type, verbose);

    if (file_type == "Forecasts") {

        if (partial_read) {
            if (!(starts.size() == 4 * in_files.size() && counts.size() == 4 * in_files.size())) {
                cerr << BOLDRED << "Error: The count of start and count should be an integer multiplication of 4 for forecasts."
                    << RESET << endl;
                return;
            }
        }
       
        // Reshape data
        Forecasts_array forecasts;
        if (verbose >= 3) cout << GREEN << "Combining forecasts ..." << RESET << endl;
        auto ret = AnEnIO::combineForecastsArray(in_files, forecasts, along, verbose, starts, counts);
        if (ret != errorType::SUCCESS) {
            throw runtime_error("Error: Failed when combining forecasts.");
        }
        
        // Write combined forecasts
        if (verbose >= 3) cout << GREEN << "Writing forecasts ..." << RESET << endl;
        handleError(io_out.writeForecasts(forecasts));

    } else if (file_type == "Observations") {
        
        if (partial_read) {
            if (!(starts.size() == 3 * in_files.size() && counts.size() == 3 * in_files.size())) {
                cerr << BOLDRED << "Error: The count for start and count should be an integer multiplication of 3 for observations."
                    << RESET << endl;
                return;
            }
        }

       // Reshape data
        if (verbose >= 3) cout << GREEN << "Combining observations ..." << RESET << endl;
        Observations_array observations;
        auto ret = AnEnIO::combineObservationsArray(in_files, observations, along, verbose, starts, counts);
        if (ret != errorType::SUCCESS) {
            throw runtime_error("Error: Failed when combining observations.");
        }
        
        // Write combined forecasts
        if (verbose >= 3) cout << GREEN << "Writing observations ..." << RESET << endl;
        handleError(io_out.writeObservations(observations));
        
    } else if (file_type == "StandardDeviation") {
        
        // Reshape data
        if (verbose >= 3) cout << GREEN << "Combining standard deviation ..." << RESET << endl;

        StandardDeviation sds;
        anenPar::Parameters parameters;
        anenSta::Stations stations;
        anenTime::FLTs flts;

        auto ret = AnEnIO::combineStandardDeviation(
                in_files, sds, parameters, stations, flts, along, verbose);
        if (ret != errorType::SUCCESS) {
            throw runtime_error("Error: Failed when combining standard deviation.");
        }
        
        if (verbose >= 3) cout << GREEN << "Writing standard deviation ..." << RESET << endl;
        io_out.writeStandardDeviation(sds, parameters, stations, flts);
        
    } else if (file_type == "Similarity") {
        
        // Reshape data
        if (verbose >= 3) cout << GREEN << "Combining similarity matrices ..." << RESET << endl;
        
        SimilarityMatrices sims;
        anenPar::Parameters parameters;
        anenSta::Stations stations, search_stations;
        anenTime::Times times, search_times;
        anenTime::FLTs flts;
        
        AnEnIO io_par("Read", in_files[0], "Similarity", verbose);
        io_par.readParameters(parameters);
                    
        auto ret = AnEnIO::combineSimilarityMatrices(
                in_files, sims, stations, times, flts,
                search_stations, search_times, along, verbose);
        if (ret != errorType::SUCCESS) {
            throw runtime_error("Error: Failed when combining similarity matrices.");
        }
        
        // Write combined similarity matrices
        if (verbose >= 3) cout << GREEN << "Writing similarity matrices ..." << RESET << endl;
        handleError(io_out.writeSimilarityMatrices(
                sims, parameters, stations, times, flts, search_stations, search_times));
        
    } else if (file_type == "Analogs") {

        // Reshape data
        if (verbose >= 3) cout << GREEN << "Combining analogs ..." << RESET << endl;

        Analogs analogs;
        anenSta::Stations stations, member_stations;
        anenTime::Times times, member_times;
        anenTime::FLTs flts;

        auto ret = AnEnIO::combineAnalogs(
                in_files, analogs, stations, times, flts,
                member_stations, member_times, along, verbose);
        if (ret != errorType::SUCCESS) {
            throw runtime_error("Error: Failed when combining analogs.");
        }


        // Write combined analogs
        if (verbose >= 3) cout << GREEN << "Writing analogs ..." << RESET << endl;
        handleError(io_out.writeAnalogs(
                analogs, stations, times, flts, member_stations, member_times));
        
    } else {
        if (verbose >= 1) cerr << BOLDRED << "Error: File type " << file_type
                << " is not supported." << RESET << endl;
        return;
    }

    if (verbose >= 3) cout << GREEN << "Done!" << RESET << endl;
    return;
}

int main(int argc, char** argv) {

#if defined(_ENABLE_MPI)
    AnEnIO::handle_MPI_Init();
#endif

    namespace po = boost::program_options;

    // Required variables
    string file_type, out_file;
    vector<string> in_files;
    size_t along = 0;

    // Optional variables
    int verbose = 0;
    string config_file;
    vector<size_t> starts, counts;

    try {
        po::options_description desc("Available options");
        desc.add_options()
                ("help,h", "Print help information for options.")
                ("config,c", po::value<string>(&config_file), "Set the configuration file path. Command line options overwrite options in configuration file. ")

                ("type,t", po::value<string>(&file_type)->required(), "Set the type of the files to read. It can be either Forecasts ,Observations, Similarity, Analogs, or StandardDeviation.")
                ("in,i", po::value< vector<string> >(&in_files)->multitoken()->required(), "Set the file names to read separated by a space.")
                ("out,o", po::value<string>(&out_file)->required(), "Set the name of the output file.")
                ("along,a", po::value<size_t>(&along)->required(), "Set the dimension index to be appended. It counts from 0.")

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

        cout << BOLDGREEN << "Parallel Ensemble Forecasts --- File Aggregate "
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

        notify(vm);
    } catch (...) {
        handle_exception(current_exception());
        return 1;
    }

    if (verbose >= 5) {
        cout << "Input parameters: " << endl
                << "file_type: " << file_type << endl
                << "in_files: " << in_files << endl
                << "out_file: " << out_file << endl
                << "along: " << along << endl
                << "verbose: " << verbose << endl
                << "start: " << starts << endl
                << "count: " << counts << endl;
    }
    
    if (file_type == "Forecasts" && along >= 4) {
        if (verbose >= 1) cerr << BOLDRED 
                << "Error: Forecasts should only have 4 dimensions. "
                << "Invalid along parameter (" << along << ")!" << RESET << endl;
        return 1;
    } else if (file_type == "Observations" && along >= 3) {
        if (verbose >= 1) cerr << BOLDRED
                << "Error: Observations should only have 3 dimensions. "
                << "Invalid along parameter (" << along << ")!" << RESET << endl;
        return 1;
    }

    try {
        runFileAggregate(file_type, in_files, along, out_file, verbose, starts, counts);
    } catch (...) {
        handle_exception(current_exception());
        return 1;
    }

#if defined(_ENABLE_MPI)
    AnEnIO::handle_MPI_Finalize();
#endif

    return 0;
}
