/* 
 * File:   similarityCalculator.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on September 11, 2018, 5:40 PM
 */

/** @file */

#if defined(_CODE_PROFILING)
#include <ctime>
#include <iomanip>
#endif

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

        const string & file_sds,
        const vector<size_t> & sds_start,
        const vector<size_t> & sds_count,

        size_t observation_id,
        bool extend_observations, 
        
        bool searchExtension, size_t max_neighbors,
        size_t num_neighbors, double distance,
        int time_match_mode, double max_par_nan,
        double max_flt_nan, int verbose) {

    
#if defined(_CODE_PROFILING)
    clock_t time_start = clock();
#endif

    /************************************************************************
     *                         Read Input Data                              *
     ************************************************************************/
    if (verbose >= 3) cout << GREEN << "Start computing similarity ... " << RESET << endl;

    Forecasts_array test_forecasts, search_forecasts;
    Observations_array observations;

    AnEnIO io("Read", file_test_forecasts, "Forecasts", verbose),
           io_out("Write", file_similarity, "Similarity", verbose);

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

#if defined(_CODE_PROFILING)
    clock_t time_end_of_reading = clock();
#endif


    /************************************************************************
     *                     Similarity Computation                           *
     ************************************************************************/
    AnEn anen(verbose);

    SimilarityMatrices sims(test_forecasts);

    StandardDeviation sds;
    if (!file_sds.empty()) {
        // If the standard deviation file is provided, read the file
        AnEnIO io_sds("Read", file_sds, "StandardDeviation", verbose);

        if (sds_start.empty() || sds_count.empty()) {
            io_sds.handleError(io_sds.readStandardDeviation(sds));
        } else {
            io_sds.handleError(io_sds.readStandardDeviation(sds, sds_start, sds_count));
        }
    } else {
        anen.handleError(anen.computeStandardDeviation(search_forecasts, sds));
    }

#if defined(_CODE_PROFILING)
    clock_t time_end_of_sd = clock();
#endif

    AnEn::TimeMapMatrix mapping;
    anen.handleError(anen.computeObservationsTimeIndices(
            search_forecasts.getTimes(), search_forecasts.getFLTs(),
            observations.getTimes(), mapping, time_match_mode));
    if (!file_mapping.empty()) {
        io.setMode("Write", file_mapping);
        io.setFileType("Matrix");
        io.handleError(io.writeTextMatrix(mapping));
    }

#if defined(_CODE_PROFILING)
    clock_t time_end_of_mapping = clock();
#endif

    if (searchExtension) {
        anen.setMethod(AnEn::simMethod::ONE_TO_MANY);
    } else {
        anen.setMethod(AnEn::simMethod::ONE_TO_ONE);
    }
    
    AnEn::SearchStationMatrix i_search_stations;

    anen.handleError(anen.computeSearchStations(
            test_forecasts.getStations(),
            search_forecasts.getStations(),
            i_search_stations, max_neighbors,
            distance, num_neighbors));
    
    anen.handleError(anen.computeSimilarity(
            search_forecasts, sds, sims, observations, mapping,
            i_search_stations, observation_id, extend_observations,
            max_par_nan, max_flt_nan));

#if defined(_CODE_PROFILING)
    clock_t time_end_of_sim = clock();
#endif


    /************************************************************************
     *                         Write Similarity                             *
     ************************************************************************/
    io_out.handleError(io_out.writeSimilarityMatrices(sims));

    if (verbose >= 3) cout << GREEN << "Done!" << RESET << endl;

#if defined(_CODE_PROFILING)
    clock_t time_end_of_write = clock();
    float duration_full = (float) (time_end_of_write - time_start) / CLOCKS_PER_SEC,
          duration_reading = (float) (time_end_of_reading - time_start) / CLOCKS_PER_SEC,
          duration_sd = (float) (time_end_of_sd - time_end_of_reading) / CLOCKS_PER_SEC,
          duration_mapping = (float) (time_end_of_mapping - time_end_of_sd) / CLOCKS_PER_SEC,
          duration_sim = (float) (time_end_of_sim - time_end_of_mapping) / CLOCKS_PER_SEC,
          duration_write = (float) (time_end_of_write - time_end_of_sim) / CLOCKS_PER_SEC;
    float duration_computation = duration_sd + duration_mapping + duration_sim;
    cout << "-----------------------------------------------------" << endl
        << "Time profiling for Similarity Calculator:" << endl
        << "Total time: " << duration_full << " seconds (100%)" << endl
        << "Reading data: " << duration_reading << " seconds (" << duration_reading / duration_full * 100 << "%)" << endl
        << "Computation: " << duration_computation << " seconds (" << duration_computation / duration_full * 100 << "%)" << endl
        << " -- SD: " << duration_sd << " seconds (" << duration_sd / duration_full * 100 << "%)" << endl
        << " -- Mapping: " << duration_mapping << " seconds (" << duration_mapping / duration_full * 100 << "%)" << endl
        << " -- Similarity: " << duration_sim << " seconds (" << duration_sim / duration_full * 100 << "%)" << endl
        << "Writing data: " << duration_write << " seconds (" << duration_write / duration_full * 100 << "%)" << endl
        << "-----------------------------------------------------" << endl;
#endif

    return;
}

int main(int argc, char** argv) {

    namespace po = boost::program_options;
    
    // Required variables
    string file_test_forecasts, file_search_forecasts,
            file_observations, file_similarity;

    // Optional variables
    int verbose = 0, time_match_mode = 1;
    size_t observation_id = 0, max_neighbors = 0, num_neighbors = 0;
    string config_file, file_mapping, file_sds;
    bool searchExtension = false, extend_observations = false;
    double distance = 0.0, max_par_nan = 0.0, max_flt_nan = 0.0;
    vector<size_t> test_start, test_count, search_start, search_count,
            obs_start, obs_count, sds_start, sds_count;
    
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
                ("time-match-mode", po::value<int>(&time_match_mode)->default_value(1), "Set the match mode for generating TimeMapMatrix. 0 for strict and 1 for loose search.")
                ("max-par-nan", po::value<double>(&max_par_nan)->default_value(0), "The number of NAN values allowed when computing similarity across different parameters. Set it to a negative number (will be automatically converted to NAN) to allow any number of NAN values.")
                ("max-flt-nan", po::value<double>(&max_flt_nan)->default_value(0), "The number of NAN values allowed when computing FLT window averages. Set it to a negative number (will be automatically converted to NAN) to allow any number of NAN values.")
                ("sds-nc", po::value<string>(&file_sds), "Set the file path to read for standard deviation.")
                ("mapping-txt", po::value<string>(&file_mapping), "Set the output file path for time mapping matrix.")
                ("observation-id", po::value<size_t>(&observation_id)->default_value(0), "Set the index of the observation variable that will be used.")
                ("searchExtension", po::bool_switch(&searchExtension)->default_value(false), "Use search extension.")
                ("distance", po::value<double>(&distance)->default_value(0.0), "Set the radius for selecting neighbors.")
                ("extend-obs", po::bool_switch(&extend_observations)->default_value(false), "After getting the most similar forecast indices, take the corresponding observations from the search station.")
                ("max-neighbors", po::value<size_t>(&max_neighbors)->default_value(0), "Set the maximum neighbors allowed to keep.")
                ("num-neighbors", po::value<size_t>(&num_neighbors)->default_value(0), "Set the number of neighbors to find.")
                ("test-start", po::value< vector<size_t> >(&test_start)->multitoken(), "Set the start indices in the test forecast NetCDF where the program starts reading.")
                ("test-count", po::value< vector<size_t> >(&test_count)->multitoken(), "Set the count numbers for each dimension in the test forecast NetCDF.")
                ("search-start", po::value< vector<size_t> >(&search_start)->multitoken(), "Set the start indices in the search forecast NetCDF where the program starts reading.")
                ("search-count", po::value< vector<size_t> >(&search_count)->multitoken(), "Set the count numbers for each dimension in the search forecast NetCDF.")
                ("obs-start", po::value< vector<size_t> >(&obs_start)->multitoken(), "Set the start indices in the search observation NetCDF where the program starts reading.")
                ("obs-count", po::value< vector<size_t> >(&obs_count)->multitoken(), "Set the count numbers for each dimension in the search observation NetCDF.")
                ("sds-start", po::value< vector<size_t> >(&sds_start)->multitoken(), "Set the start indices in the standard deviation NetCDF where the program starts reading.")
                ("sds-count", po::value< vector<size_t> >(&sds_count)->multitoken(), "Set the count numbers for each dimension in the standard deviation NetCDF.");
        
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
#if defined(_CODE_PROFILING)
                << " (with code profiling)"
#endif
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

        if (vm.count("max-par-nan")) {
            if (vm["max-par-nan"].as<double>() < 0) {
                max_par_nan = NAN;
            }
        }

        if (vm.count("max-flt-nan")) {
            if (vm["max-flt-nan"].as<double>() < 0) {
                max_flt_nan = NAN;
            }
        }
        
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
                << "file_sds: " << file_sds << endl
                << "config_file: " << config_file << endl
                << "verbose: " << verbose << endl
                << "max_par_nan: " << max_par_nan << endl
                << "max_flt_nan: " << max_flt_nan << endl
                << "extend_observations: " << extend_observations << endl
                << "time_match_mode: " << time_match_mode << endl
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
                << "obs_count: " << obs_count << endl
                << "sds_start: " << sds_start << endl
                << "sds_count: " << sds_count << endl;
    }
    
    try {
        runSimilarityCalculator(
                file_test_forecasts, test_start, test_count,
                file_search_forecasts, search_start, search_count,
                file_observations, obs_start, obs_count, file_similarity,
                file_mapping, file_sds, sds_start, sds_count,
                observation_id, extend_observations, searchExtension,
                max_neighbors, num_neighbors, distance, time_match_mode,
                max_par_nan, max_flt_nan, verbose);
    } catch (...) {
        handle_exception(current_exception());
        return 1;
    }
    
    return 0;
}

