/* 
 * File:   similarityCalculator.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on September 11, 2018, 5:40 PM
 */

/** @file */

#include "AnEnIO.h"
#include "AnEn.h"
#include "CommonExeFunctions.h"

#include "boost/program_options.hpp"
#include "boost/filesystem.hpp"
#include "boost/exception/all.hpp"

#include <iostream>
#include <fstream>

using namespace std;

void runSimilarityCalculator(
        const string & file_test_forecasts,
        const vector<size_t> & test_start,
        const vector<size_t> & test_count,

        const string & file_search_forecasts,
        const vector<size_t> & search_start,
        const vector<size_t> & search_count,

        const string & file_observations,
        const vector<size_t> & obs_start,
        const vector<size_t> & obs_count,

        const string & file_similarity,
        const string & file_mapping,

        size_t observation_id,

        bool searchExtension, size_t max_neighbors,
        size_t num_neighbors, double distance, int verbose) {

    
    /************************************************************************
     *                         Read Input Data                              *
     ************************************************************************/
    Forecasts_array test_forecasts, search_forecasts;
    Observations_array observations;

    AnEnIO io("Read", file_test_forecasts, "Forecasts", verbose);

    if (test_start.empty() || test_count.empty()) {
        io.handleError(io.readForecasts(test_forecasts));
    } else {
        io.handleError(io.readForecasts(test_forecasts,
                test_start, test_count));
    }

    io.setFilePath(file_search_forecasts);
    if (search_start.empty() || search_count.empty()) {
        io.handleError(io.readForecasts(search_forecasts));
    } else {
        io.handleError(io.readForecasts(search_forecasts,
                search_start, search_count));
    }

    io.setFileType("Observations");
    io.setFilePath(file_observations);
    if (obs_start.empty() || obs_count.empty()) {
        io.handleError(io.readObservations(observations));
    } else {
        io.handleError(io.readObservations(observations,
                obs_start, obs_count));
    }


    /************************************************************************
     *                     Similarity Computation                           *
     ************************************************************************/
    AnEn anen(verbose);

    SimilarityMatrices sims(test_forecasts);

    StandardDeviation sds;
    anen.handleError(anen.computeStandardDeviation(search_forecasts, sds));

    AnEn::TimeMapMatrix mapping;
    anen.handleError(anen.computeObservationsTimeIndices(
            search_forecasts.getTimes(), search_forecasts.getFLTs(),
            observations.getTimes(), mapping));
    if (!file_mapping.empty()) {
        io.setMode("Write", file_mapping);
        io.setFileType("Matrix");
        io.handleError(io.writeTextMatrix(mapping));
    }

    if (searchExtension) {
        anen.setMethod(AnEn::simMethod::ONE_TO_MANY);

        AnEn::SearchStationMatrix i_search_stations;
        
        anen.handleError(anen.computeSearchStations(
                test_forecasts.getStations(),
                search_forecasts.getStations(),
                i_search_stations, max_neighbors,
                distance, num_neighbors, true));
        anen.handleError(anen.computeSimilarity(
                search_forecasts, sds, sims, observations, mapping,
                i_search_stations, observation_id));

    } else {
        anen.setMethod(AnEn::simMethod::ONE_TO_ONE);

        anen.handleError(anen.computeSimilarity(
                search_forecasts, sds, sims, observations,
                mapping, observation_id));
    }


    /************************************************************************
     *                         Write Similarity                             *
     ************************************************************************/
    io.setMode("Write", file_similarity);
    io.setFileType("Similarity");
    io.handleError(io.writeSimilarityMatrices(sims));
}

int main(int argc, char** argv) {

    namespace po = boost::program_options;
    
    // Required variables
    string file_test_forecasts, file_search_forecasts,
            file_observations, file_similarity;

    // Optional variables
    int verbose = 0;
    size_t observation_id = 0, max_neighbors = 0, num_neighbors = 0;
    string config_file, file_mapping;
    bool searchExtension = false;
    double distance = 0;
    vector<size_t> test_start, test_count, search_start, search_count,
            obs_start, obs_count;
    
    try {
        po::options_description desc("Available options");
        desc.add_options()
                ("help,h", "Print help information for options.")
                ("config,c", po::value<string>(&config_file), "Set the configuration file path. Command line options overwrite options in configuration file. ")

                ("test-forecast-nc", po::value<string>(&file_test_forecasts)->required(), "Set the input file path for test forecast NetCDF.")
                ("search-forecast-nc", po::value<string>(&file_search_forecasts)->required(), "Set input the file path for search forecast NetCDF.")
                ("observation-nc", po::value<string>(&file_observations)->required(), "Set the input file path for search observation NetCDF.")
                ("similarity-nc", po::value<string>(&file_similarity)->required(), "Set the output file path for similarity NetCDF.")

                ("verbose,v", po::value<int>(&verbose)->default_value(2), "Set the verbose level.")
                ("mapping-txt", po::value<string>(&file_mapping), "Set the output file path for time mapping matrix.")
                ("observation-id", po::value<size_t>(&observation_id)->default_value(0), "Set the index of the observation variable that will be used.")
                ("searchExtension", po::bool_switch(&searchExtension)->default_value(false), "Use search extension.")
                ("distance", po::value<double>(&distance)->default_value(0.0), "Set the radius for selecting neighbors.")
                ("max-neighbors", po::value<size_t>(&max_neighbors)->default_value(0), "Set the maximum neighbors allowed to keep.")
                ("num-neighbors", po::value<size_t>(&num_neighbors)->default_value(0), "Set the number of neighbors to find.")
                ("test-start", po::value< vector<size_t> >(&test_start)->multitoken(), "Set the start indices in the test forecast NetCDF where the program starts reading.")
                ("test-count", po::value< vector<size_t> >(&test_count)->multitoken(), "Set the count numbers for each dimension in the test forecast NetCDF.")
                ("search-start", po::value< vector<size_t> >(&search_start)->multitoken(), "Set the start indices in the search forecast NetCDF where the program starts reading.")
                ("search-count", po::value< vector<size_t> >(&search_count)->multitoken(), "Set the count numbers for each dimension in the search forecast NetCDF.")
                ("obs-start", po::value< vector<size_t> >(&obs_start)->multitoken(), "Set the start indices in the search observation NetCDF where the program starts reading.")
                ("obs-count", po::value< vector<size_t> >(&obs_count)->multitoken(), "Set the count numbers for each dimension in the search observation NetCDF.");
        
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
            cout << GREEN << "Analog Ensemble program --- Similarity Calculator"
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
        cout << "Input parameters:" << endl
                << "file_test_forecasts: " << file_test_forecasts << endl
                << "file_search_forecasts: " << file_search_forecasts << endl
                << "file_observations: " << file_observations << endl
                << "file_similarity: " << file_similarity << endl
                << "file_mapping: " << file_mapping << endl
                << "config_file: " << config_file << endl
                << "verbose: " << verbose << endl
                << "observation_id: " << observation_id << endl
                << "searchExtension: " << searchExtension << endl
                << "distance: " << distance << endl
                << "max_neighbors: " << max_neighbors << endl
                << "num_neighbors: " << num_neighbors << endl
                << "test_start: " << test_start << endl
                << "test_count: " << test_count << endl
                << "search_start: " << search_start << endl
                << "search_count: " << search_count << endl
                << "obs_start: " << obs_start << endl
                << "obs_count: " << obs_count << endl;
    }
    
    try {
        runSimilarityCalculator(
                file_test_forecasts, test_start, test_count,
                file_search_forecasts, search_start, search_count,
                file_observations, obs_start, obs_count, file_similarity,
                file_mapping, observation_id, searchExtension, max_neighbors,
                num_neighbors, distance, verbose);
    } catch (...) {
        handle_exception(current_exception());
        return 1;
    }
    
    return 0;
}

