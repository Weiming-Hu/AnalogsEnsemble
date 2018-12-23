/* 
 * File:   analogSelector.cpp
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <weiming@psu.edu>
 *
 * Created on April 17, 2018, 11:10 PM
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
#include "boost/multi_array.hpp"
#include "boost/filesystem.hpp"

#include <iostream>
#include <sstream>

using namespace std;

void runAnalogSelector(const string & file_sim, const string & file_obs,
        const vector<size_t> & obs_start, const vector<size_t> & obs_count,
        const string & file_mapping, const string & file_analogs, size_t observation_id,
        size_t num_members, bool quick, bool extend_observations,
        bool preserve_real_time, int verbose) {
    
#if defined(_CODE_PROFILING)
    clock_t time_start = clock();
#endif

    /**************************************************************************
     *                                Prepare Input                           *
     **************************************************************************/
    if (verbose >= 3) cout << GREEN << "Start selecting analogs ... " << RESET << endl;

    Analogs analogs;
    AnEn anen(verbose);
    
    AnEnIO io("Read", file_sim, "Similarity", verbose);
    SimilarityMatrices sims;
    io.handleError(io.readSimilarityMatrices(sims));
    
    Observations_array search_observations;
    io.setFileType("Observations");
    io.setFilePath(file_obs);
    if (obs_start.empty() || obs_count.empty()) {
        io.handleError(io.readObservations(search_observations));
    } else {
        io.handleError(io.readObservations(search_observations,
                obs_start, obs_count));
    }
    
    AnEn::TimeMapMatrix mapping;
    if (file_mapping.empty()) {
        stringstream ss;
        ss << BOLDRED << "Error: Recomputing the time mapping matrix is "
                << "currently not supported." << RESET << endl;
        throw runtime_error(ss.str());
    } else {
        io.setMode("Read", file_mapping);
        io.setFileType("Matrix");
        io.handleError(io.readTextMatrix(mapping));
    }
    
#if defined(_CODE_PROFILING)
    clock_t time_end_of_reading = clock();
#endif

    
    /**************************************************************************
     *                                Check Input                             *
     **************************************************************************/
    if (mapping.size2() != sims.getTargets().getFLTsSize()) {
        stringstream ss;
        ss << BOLDRED << "Error: Number of columns in mapping (" << mapping.size2()
            << ") should match number of forecast forecast lead times ("
            << sims.getTargets().getFLTsSize() << ")!" << RESET << endl;
        throw runtime_error(ss.str());
    }
    
    
    /**************************************************************************
     *                             Select Analogs                             *
     **************************************************************************/
    anen.handleError(anen.selectAnalogs(analogs, sims, search_observations,
            mapping, observation_id, num_members, quick,
            extend_observations, preserve_real_time));
    
#if defined(_CODE_PROFILING)
    clock_t time_end_of_select = clock();
#endif

    
    /**************************************************************************
     *                             Write Analogs                              *
     **************************************************************************/
    io.setMode("Write", file_analogs);
    io.setFileType("Analogs");
    io.handleError(io.writeAnalogs(analogs));
    
    if (verbose >= 3) cout << GREEN << "Done!" << RESET << endl;

#if defined(_CODE_PROFILING)
    clock_t time_end_of_write = clock();
    float duration_full = (float) (time_end_of_write - time_start) / CLOCKS_PER_SEC,
          duration_reading = (float) (time_end_of_reading - time_start) / CLOCKS_PER_SEC,
          duration_select = (float) (time_end_of_select - time_end_of_reading) / CLOCKS_PER_SEC,
          duration_write = (float) (time_end_of_write - time_end_of_select) / CLOCKS_PER_SEC;
    cout << "-----------------------------------------------------" << endl
        << "Time profiling for Analog Selector:" << endl
        << "Total time: " << duration_full << " seconds (100%)" << endl
        << "Reading data: " << duration_reading << " seconds (" << duration_reading / duration_full * 100 << "%)" << endl
        << "Selection: " << duration_select << " seconds (" << duration_select / duration_full * 100 << "%)" << endl
        << "Writing data: " << duration_write << " seconds (" << duration_write / duration_full * 100 << "%)" << endl
        << "-----------------------------------------------------" << endl;
#endif

    return;
}

int main(int argc, char** argv) {
    
    namespace po = boost::program_options;
    
    // Required variables
    string file_sim, file_obs, file_mapping, file_analogs;
    size_t num_members;

    // Optional variables
    int verbose = 0;
    string config_file;
    size_t observation_id = 0;
    vector<size_t>  obs_start, obs_count;
    bool quick = false, extend_observations = false,
         preserve_real_time = false;
    
    try {
        po::options_description desc("Avaialble options");
        desc.add_options()
                ("help,h", "Print help information for options.")
                ("config,c", po::value<string>(&config_file), "Set the configuration file path. Command line options overwrite options in configuration file. ")
                
                ("similarity-nc", po::value<string>(&file_sim)->required(), "Set the input file for the similarity matrix.")
                ("observation-nc", po::value<string>(&file_obs)->required(), "Set the input file for search observations.")
                ("mapping-txt", po::value<string>(&file_mapping)->required(), "Set the input file for time mapping matrix")
                ("analog-nc", po::value<string>(&file_analogs)->required(), "Set the output file for analogs.")
                ("members", po::value<size_t>(&num_members)->required(), "Set the number of analog members to keep in an ensemble.")
                
                ("verbose,v", po::value<int>(&verbose)->default_value(2), "Set the verbose level.")
                ("observation-id", po::value<size_t>(&observation_id)->default_value(0), "Set the index of the observation variable that will be used.")
                ("obs-start", po::value< vector<size_t> >(&obs_start)->multitoken(), "Set the start indices in the search observation NetCDF where the program starts reading.")
                ("obs-count", po::value< vector<size_t> >(&obs_count)->multitoken(), "Set the count numbers for each dimension in the search observation NetCDF.")
                ("quick", po::bool_switch(&quick)->default_value(false), "Use quick sort when selecting analog members.")
                ("extend-obs", po::bool_switch(&extend_observations)->default_value(false), "After getting the most similar forecast indices, take the corresponding observations from the search station.")
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

        if (vm.count("config")) {
            // If configuration file is specified, read it first.
            // The variable won't be written until we call notify.
            //
            config_file = vm["config"].as<string>();
        }
        
        // Then parse the configuration file
        if (!config_file.empty()) {
            ifstream ifs(config_file.c_str());
            if (!ifs) {
                cout << BOLDRED << "Error: Can't open configuration file " << config_file << RESET << endl;
                return 1;
            } else {
                auto parsed_config = parse_config_file(ifs, desc, true);

                auto unregistered_keys = po::collect_unrecognized(parsed_config.options, po::exclude_positional);
                if (unregistered_keys.size() != 0) {
                    guess_arguments(unregistered_keys, available_options);
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
            << "file_sim: " << file_sim << endl
            << "file_obs: " << file_obs << endl
            << "file_mapping: " << file_mapping << endl
            << "file_analogs: " << file_analogs << endl
            << "num_members: " << num_members << endl
            << "verbose: " << verbose << endl
            << "config_file: " << config_file << endl
            << "observation_id: " << observation_id << endl
            << "obs_start: " << obs_start << endl
            << "obs_count: " << obs_count << endl
            << "quick: " << quick << endl
            << "extend_observations: " << extend_observations << endl
            << "preserve_real_time: " << preserve_real_time << endl;
    }
    
    try {
        runAnalogSelector(file_sim, file_obs, obs_start, obs_count,
                file_mapping, file_analogs, observation_id, num_members,
                quick, extend_observations, preserve_real_time, verbose);
    } catch (...) {
        handle_exception(current_exception());
        return 1;
    }
    
    return (0);
}
