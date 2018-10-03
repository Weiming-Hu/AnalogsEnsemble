/* 
 * File:   analogGenerator.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *         Guido Cervone <cervone@psu.edu>
 *
 * Created on April 17, 2018, 11:10 PM
 */

/** @file */

#include "AnEnIO.h"
#include "AnEn.h"
#include "CommonExeFunctions.h"

#include "boost/program_options.hpp"
#include "boost/multi_array.hpp"
#include "boost/filesystem.hpp"
#include "boost/exception/all.hpp"

using namespace std;

void runAnalogGenerator(
        const string & file_test_forecasts,
        const vector<size_t> & test_start,
        const vector<size_t> & test_count,

        const string & file_search_forecasts,
        const vector<size_t> & search_start,
        const vector<size_t> & search_count,

        const string & file_observations,
        const vector<size_t> & obs_start,
        const vector<size_t> & obs_count,

        const string & file_mapping,
        const string & file_similarity,
        const string & file_analogs,

        size_t observation_id,

        bool searchExtension, size_t max_neighbors,
        size_t num_neighbors, double distance, 

        size_t num_members, bool quick,
        bool preserve_real_time, int verbose) {

    /************************************************************************
     *                         Read Input Data                              *
     ************************************************************************/
    Forecasts_array test_forecasts, search_forecasts;
    Observations_array search_observations;

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
        io.handleError(io.readObservations(search_observations));
    } else {
        io.handleError(io.readObservations(search_observations,
                obs_start, obs_count));
    }


    /************************************************************************
     *                           Analog Generation                          *
     ************************************************************************/
    AnEn anen(verbose);

    SimilarityMatrices sims(test_forecasts);

    StandardDeviation sds;
    anen.handleError(anen.computeStandardDeviation(search_forecasts, sds));

    AnEn::TimeMapMatrix mapping;
    anen.handleError(anen.computeObservationsTimeIndices(
                search_forecasts.getTimes(), search_forecasts.getFLTs(),
                search_observations.getTimes(), mapping));
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
                search_forecasts, sds, sims, search_observations, mapping,
                i_search_stations, observation_id));

    } else {
        anen.setMethod(AnEn::simMethod::ONE_TO_ONE);

        anen.handleError(anen.computeSimilarity(
                search_forecasts, sds, sims, search_observations,
                mapping, observation_id));
    }

    if (!file_similarity.empty()) {
        io.setMode("Write", file_similarity);
        io.setFileType("Similarity");
        io.handleError(io.writeSimilarityMatrices(sims));
    }

    Analogs analogs;
    anen.handleError(anen.selectAnalogs(analogs, sims, search_observations,
            mapping, observation_id, num_members, quick, preserve_real_time));


    /************************************************************************
     *                           Write Analogs                              *
     ************************************************************************/
    io.setMode("Write", file_analogs);
    io.setFileType("Analogs");
    io.handleError(io.writeAnalogs(analogs));
    
    return;
}

int main(int argc, char** argv) {

    namespace po = boost::program_options;
    
    // Required variables
    string file_test_forecasts, file_search_forecasts,
            file_observations, file_analogs;
    size_t num_members = 0;

    // Optional variables
    int verbose = 0;
    size_t observation_id = 0;
    string config_file, file_mapping, file_similarity;
    bool quick = false, preserve_real_time = false, searchExtension = false;

    double distance = 0;
    size_t max_neighbors, num_neighbors;
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
                ("analog-nc", po::value<string>(&file_analogs)->required(), "Set the output file for analogs.")
                ("members", po::value<size_t>(&num_members)->required(), "Set the number of analog members to keep in an ensemble.")

                ("verbose,v", po::value<int>(&verbose)->default_value(2), "Set the verbose level.")
                ("similarity-nc", po::value<string>(&file_similarity), "Set the output file path for similarity NetCDF.")
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
                ("obs-count", po::value< vector<size_t> >(&obs_count)->multitoken(), "Set the count numbers for each dimension in the search observation NetCDF.")
                ("quick", po::bool_switch(&quick)->default_value(false), "Use quick sort when selecting analog members.")
                ("real-time", po::bool_switch(&preserve_real_time)->default_value(false), "Convert observation time index to real time information.");
        
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
            cout << GREEN << "Analog Ensemble program --- Analog Generator"
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
            << "file_analogs: " << file_analogs << endl
            << "config_file: " << config_file << endl
            << "num_members: " << num_members << endl
            << "observation_id: " << observation_id << endl
            << "searchExtension: " << searchExtension << endl
            << "quick: " << quick << endl
            << "preserve_real_time: " << preserve_real_time << endl
            << "verbose: " << verbose << endl
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
        runAnalogGenerator(
                file_test_forecasts, test_start, test_count,
                file_search_forecasts, search_start, search_count,
                file_observations, obs_start, obs_count,
                file_mapping, file_similarity, file_analogs,
                observation_id, searchExtension, max_neighbors,
                num_neighbors, distance, num_members, quick,
                preserve_real_time, verbose);
    } catch (...) {
        handle_exception(current_exception());
        return 1;
    }

    return (0);
}
