/* 
 * File:   analogGenerator.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *         Guido Cervone <cervone@psu.edu>
 *
 * Created on April 17, 2018, 11:10 PM
 */

/** @file */

#if defined(_CODE_PROFILING)
#include <ctime>
#if defined(_OPENMP)
#include <omp.h>
#endif
#endif

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
        const string & file_sds,
        const vector<size_t> & sds_start,
        const vector<size_t> & sds_count,

        const string & file_similarity,
        const string & file_analogs,

        size_t observation_id,

        bool searchExtension, size_t max_neighbors,
        size_t num_neighbors, double distance, 

        size_t num_members, bool quick,
        bool extend_observations,
        int time_match_mode, double max_par_nan,
        double max_flt_nan, 
        
        vector<size_t> test_times_index,
        vector<size_t> search_times_index,
        const vector<double> & weights,
        
        bool operational, int verbose) {

#if defined(_CODE_PROFILING)
    clock_t time_start = clock();
#if defined(_OPENMP)
    double wtime_start = omp_get_wtime();
#endif
#endif

    /************************************************************************
     *                         Read Input Data                              *
     ************************************************************************/
    if (verbose >=3) cout << GREEN << "Start generating analogs ... " << RESET << endl;
    
    Forecasts_array test_forecasts, search_forecasts;
    Observations_array search_observations;

    AnEnIO io("Read", file_test_forecasts, "Forecasts", verbose),
           io_out("Write", file_analogs, "Analogs", verbose);

    if (test_start.empty() || test_count.empty()) {
        handleError(io.readForecasts(test_forecasts));
    } else {
        handleError(io.readForecasts(test_forecasts,
                test_start, test_count));
    }

    io.setFilePath(file_search_forecasts);
    if (search_start.empty() || search_count.empty()) {
        handleError(io.readForecasts(search_forecasts));
    } else {
        handleError(io.readForecasts(search_forecasts,
                search_start, search_count));
    }

    io.setFileType("Observations");
    io.setFilePath(file_observations);
    if (obs_start.empty() || obs_count.empty()) {
        handleError(io.readObservations(search_observations));
    } else {
        handleError(io.readObservations(search_observations,
                obs_start, obs_count));
    }

    // Assign weights if weights are provided properly
    if (weights.size() != 0) {

        // Weights of test forecasts will be used.
        if (weights.size() == test_forecasts.getParametersSize()) {
            
            anenPar::Parameters parameters_new;
            auto & parameters_by_insert =
                test_forecasts.getParameters().get<anenPar::by_insert>();

            for (size_t i = 0; i < weights.size(); i++) {
                auto parameter = parameters_by_insert[i];
                parameter.setWeight(weights[i]);
                parameters_new.push_back(parameter);
            }

            test_forecasts.setParameters(parameters_new);

            if (verbose >= 4) cout << test_forecasts.getParameters();

        } else {
            if (verbose >= 1) {
                cerr << BOLDRED << "Error: " << weights.size() << " weights provided. But forecasts have "
                    << test_forecasts.getParametersSize() << " parameters." << RESET << endl;
                return;
            }
        }
    }

#if defined(_CODE_PROFILING)
    clock_t time_end_of_reading = clock();
#if defined(_OPENMP)
    double wtime_end_of_reading = omp_get_wtime();
#endif
#endif

    /************************************************************************
     *                           Analog Generation                          *
     ************************************************************************/
    AnEn anen(verbose);
    Functions functions(verbose);

    SimilarityMatrices sims(test_forecasts);

    StandardDeviation sds;
    if (!operational) {
        if (!file_sds.empty()) {
            // If the standard deviation file is provided, read the file
            AnEnIO io_sds("Read", file_sds, "StandardDeviation", verbose);
            if (sds_start.empty() || sds_count.empty()) {
                handleError(io_sds.readStandardDeviation(sds));
            } else {
                handleError(io_sds.readStandardDeviation(sds, sds_start, sds_count));
            }
        } else {
            handleError(functions.computeStandardDeviation(
                        search_forecasts, sds, search_times_index));
        }
    }

#if defined(_CODE_PROFILING)
    clock_t time_end_of_sd = clock();
#if defined(_OPENMP)
    double wtime_end_of_sd = omp_get_wtime();
#endif
#endif

    AnEn::TimeMapMatrix mapping;
    handleError(functions.computeObservationsTimeIndices(
                search_forecasts.getTimes(), search_forecasts.getFLTs(),
                search_observations.getTimes(), mapping, time_match_mode));
    if (!file_mapping.empty()) {
        io.setMode("Write", file_mapping);
        io.setFileType("Matrix");
        handleError(io.writeTextMatrix(mapping));
    }

#if defined(_CODE_PROFILING)
    clock_t time_end_of_mapping = clock();
#if defined(_OPENMP)
    double wtime_end_of_mapping = omp_get_wtime();
#endif
#endif

    if (searchExtension) {
        anen.setMethod(AnEn::simMethod::OneToMany);
    } else {
        anen.setMethod(AnEn::simMethod::OneToOne);
    }
    
    AnEn::SearchStationMatrix i_search_stations;

    handleError(anen.computeSearchStations(
            test_forecasts.getStations(),
            search_forecasts.getStations(),
            i_search_stations, max_neighbors,
            distance, num_neighbors));
    
    anenTime::Times test_times, search_times;
    
    const auto & container_test = test_forecasts.getTimes().get<anenTime::by_insert>();
    for (size_t i : test_times_index) test_times.push_back(container_test[i]);
    
    const auto & container_search = search_forecasts.getTimes().get<anenTime::by_insert>();
    for (size_t i : search_times_index) search_times.push_back(container_search[i]);
    
    handleError(anen.computeSimilarity(
            test_forecasts, search_forecasts, sds, sims, search_observations,
            mapping, i_search_stations, observation_id, extend_observations,
            max_par_nan, max_flt_nan, test_times, search_times, operational));

    if (!file_similarity.empty()) {
        AnEnIO io_sim("Write", file_similarity, "Similarity");
        handleError(io_sim.writeSimilarityMatrices(
                sims, test_forecasts.getParameters(),
                test_forecasts.getStations(),
                test_forecasts.getTimes(),
                test_forecasts.getFLTs(),
                search_forecasts.getStations(),
                search_forecasts.getTimes()));
    }

#if defined(_CODE_PROFILING)
    clock_t time_end_of_sim = clock();
#if defined(_OPENMP)
    double wtime_end_of_sim = omp_get_wtime();
#endif
#endif

    Analogs analogs(test_forecasts, num_members);
    
    handleError(anen.selectAnalogs(analogs, sims,
            test_forecasts.getStations(), search_observations,
            mapping, observation_id, num_members, quick,
            extend_observations));

#if defined(_CODE_PROFILING)
    clock_t time_end_of_select = clock();
#if defined(_OPENMP)
    double wtime_end_of_select = omp_get_wtime();
#endif
#endif

    /************************************************************************
     *                           Write Analogs                              *
     ************************************************************************/
    handleError(io_out.writeAnalogs(
            analogs, test_forecasts.getStations(),
            test_forecasts.getTimes(),
            test_forecasts.getFLTs(),
            search_observations.getStations(),
            search_observations.getTimes()));

    if (verbose >= 3) cout << GREEN << "Done!" << RESET << endl;

#if defined(_CODE_PROFILING)
    clock_t time_end_of_write = clock();
#if defined(_OPENMP)
    double wtime_end_of_write = omp_get_wtime();

    double wduration_full = wtime_end_of_write - wtime_start,
           wduration_reading = wtime_end_of_reading - wtime_start,
           wduration_sd = wtime_end_of_sd - wtime_end_of_reading,
           wduration_mapping = wtime_end_of_mapping - wtime_end_of_sd,
           wduration_sim = wtime_end_of_sim - wtime_end_of_mapping,
           wduration_select = wtime_end_of_select - wtime_end_of_sim,
           wduration_write = wtime_end_of_write - wtime_end_of_select;
    double wduration_computation = wduration_sd + wduration_mapping + wduration_sim + wduration_select;
#endif

    float duration_full = (float) (time_end_of_write - time_start) / CLOCKS_PER_SEC,
          duration_reading = (float) (time_end_of_reading - time_start) / CLOCKS_PER_SEC,
          duration_sd = (float) (time_end_of_sd - time_end_of_reading) / CLOCKS_PER_SEC,
          duration_mapping = (float) (time_end_of_mapping - time_end_of_sd) / CLOCKS_PER_SEC,
          duration_sim = (float) (time_end_of_sim - time_end_of_mapping) / CLOCKS_PER_SEC,
          duration_select = (float) (time_end_of_select - time_end_of_sim) / CLOCKS_PER_SEC,
          duration_write = (float) (time_end_of_write - time_end_of_select) / CLOCKS_PER_SEC;
    float duration_computation = duration_sd + duration_mapping + duration_sim + duration_select;

    cout << "-----------------------------------------------------" << endl
        << "User Time for Analog Generator:" << endl
        << "Total: " << duration_full << " seconds (100%)" << endl
        << "Reading data: " << duration_reading << " seconds (" << duration_reading / duration_full * 100 << "%)" << endl
        << "Computation: " << duration_computation << " seconds (" << duration_computation / duration_full * 100 << "%)" << endl
        << " -- SD: " << duration_sd << " seconds (" << duration_sd / duration_full * 100 << "%)" << endl
        << " -- Mapping: " << duration_mapping << " seconds (" << duration_mapping / duration_full * 100 << "%)" << endl
        << " -- Similarity: " << duration_sim << " seconds (" << duration_sim / duration_full * 100 << "%)" << endl
        << " -- Selection: " << duration_select << " seconds (" << duration_select / duration_full * 100 << "%)" << endl
        << "Writing data: " << duration_write << " seconds (" << duration_write / duration_full * 100 << "%)" << endl
        << "-----------------------------------------------------" << endl;
#if defined(_OPENMP)
    cout << "-----------------------------------------------------" << endl
        << "Real Time for Analog Generator:" << endl
        << "Total: " << wduration_full << " seconds (100%)" << endl
        << "Reading data: " << wduration_reading << " seconds (" << wduration_reading / wduration_full * 100 << "%)" << endl
        << "Computation: " << wduration_computation << " seconds (" << wduration_computation / wduration_full * 100 << "%)" << endl
        << " -- SD: " << wduration_sd << " seconds (" << wduration_sd / wduration_full * 100 << "%)" << endl
        << " -- Mapping: " << wduration_mapping << " seconds (" << wduration_mapping / wduration_full * 100 << "%)" << endl
        << " -- Similarity: " << wduration_sim << " seconds (" << wduration_sim / wduration_full * 100 << "%)" << endl
        << " -- Selection: " << wduration_select << " seconds (" << wduration_select / wduration_full * 100 << "%)" << endl
        << "Writing data: " << wduration_write << " seconds (" << wduration_write / wduration_full * 100 << "%)" << endl
        << "-----------------------------------------------------" << endl;
#endif
#endif

    return;
}

int main(int argc, char** argv) {

#if defined(_ENABLE_MPI)
    AnEnIO::handle_MPI_Init();
#endif

    namespace po = boost::program_options;
    
    // Required variables
    string file_test_forecasts, file_search_forecasts, file_observations, file_analogs;
    size_t num_members = 0;

    // Optional variables
    int verbose = 0, time_match_mode = 1;
    size_t observation_id = 0;
    string config_file, file_mapping, file_similarity, file_sds;
    bool quick = false, searchExtension = false, extend_observations = false,
            operational = false, continuous_time_index = false;

    vector<double> weights;
    double distance = 0.0, max_par_nan = 0.0, max_flt_nan = 0.0;
    size_t max_neighbors = 0, num_neighbors = 0;
    vector<size_t> test_start, test_count, search_start, search_count,
            obs_start, obs_count, sds_start, sds_count, test_times_index,
            search_times_index;

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
                ("time-match-mode", po::value<int>(&time_match_mode)->default_value(1), "Set the match mode for generating TimeMapMatrix. 0 for strict and 1 for loose search.")
                ("max-par-nan", po::value<double>(&max_par_nan)->default_value(0), "The number of NAN values allowed when computing similarity across different parameters. Set it to a negative number (will be automatically converted to NAN) to allow any number of NAN values.")
                ("max-flt-nan", po::value<double>(&max_flt_nan)->default_value(0), "The number of NAN values allowed when computing FLT window averages. Set it to a negative number (will be automatically converted to NAN) to allow any number of NAN values.")
                ("similarity-nc", po::value<string>(&file_similarity), "Set the output file path for similarity NetCDF.")
                ("sds-nc", po::value<string>(&file_sds), "Set the file path to read for standard deviation.")
                ("mapping-txt", po::value<string>(&file_mapping), "Set the output file path for time mapping matrix.")
                ("observation-id", po::value<size_t>(&observation_id)->default_value(0), "Set the index of the observation variable that will be used.")
                ("searchExtension", po::bool_switch(&searchExtension)->default_value(false), "Use search extension.")
                ("distance", po::value<double>(&distance)->default_value(0.0), "Set the radius for selecting neighbors.")
                ("max-neighbors", po::value<size_t>(&max_neighbors)->default_value(0), "Set the maximum neighbors allowed to keep.")
                ("num-neighbors", po::value<size_t>(&num_neighbors)->default_value(0), "Set the number of neighbors to find.")
                ("test-start", po::value< vector<size_t> >(&test_start)->multitoken(), "(File I/O) Set the start indices in the test forecast NetCDF where the program starts reading.")
                ("test-count", po::value< vector<size_t> >(&test_count)->multitoken(), "(File I/O) Set the count numbers for each dimension in the test forecast NetCDF.")
                ("search-start", po::value< vector<size_t> >(&search_start)->multitoken(), "(File I/O) Set the start indices in the search forecast NetCDF where the program starts reading.")
                ("search-count", po::value< vector<size_t> >(&search_count)->multitoken(), "(File I/O) Set the count numbers for each dimension in the search forecast NetCDF.")
                ("obs-start", po::value< vector<size_t> >(&obs_start)->multitoken(), "(File I/O) Set the start indices in the search observation NetCDF where the program starts reading.")
                ("obs-count", po::value< vector<size_t> >(&obs_count)->multitoken(), "(File I/O) Set the count numbers for each dimension in the search observation NetCDF.")
                ("sds-start", po::value< vector<size_t> >(&sds_start)->multitoken(), "(File I/O) Set the start indices in the standard deviation NetCDF where the program starts reading.")
                ("sds-count", po::value< vector<size_t> >(&sds_count)->multitoken(), "(File I/O) Set the count numbers for each dimension in the standard deviation NetCDF.")
                ("test-times-index", po::value< vector<size_t> >(&test_times_index)->multitoken(), "Set the indices or the index range (with --continuous-time) of test times in the actual forecasts after the reading.")
                ("search-times-index", po::value< vector<size_t> >(&search_times_index)->multitoken(), "Set the indices or the index range (with --continuous-time) of search times in the actual forecasts after the reading.")
                ("operational", po::bool_switch(&operational)->default_value(false), "Use operational search. This feature uses all the times from search times that are historical to each test time during comparison.")
                ("continuous-time", po::bool_switch(&continuous_time_index)->default_value(false), "Whether to generate a sequence for test-times-index and search-times-index. If used, an inclusive sequence is generated when only 2 indices (start and end) are provided in test or search times index.")
                ("weights", po::value<vector<double> >(&weights)->multitoken(), "Specify the weight for each parameter in forecasts. The order of weights should be the same as the order of forecast parameters.")
                ("quick", po::bool_switch(&quick)->default_value(false), "Use quick sort when selecting analog members.")
                ("extend-obs", po::bool_switch(&extend_observations)->default_value(false), "After getting the most similar forecast indices, take the corresponding observations from the search station.");
        
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
        
        cout << BOLDGREEN << "Parallel Ensemble Forecasts --- Analog Generator "
#if defined(_CODE_PROFILING)
            <<" (with code profiling) "
#endif
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
    
    if (continuous_time_index) {
        bool succeed = false;
        
        if (test_times_index.size() == 2) {
            test_times_index.resize(test_times_index[1] - test_times_index[0] + 1);
            iota(test_times_index.begin(), test_times_index.end(), test_times_index[0]);
            succeed = true;
            
            if (verbose >= 4) {
                cout << "A sequence of test times are generated: " << test_times_index << endl;
            }
        }
        
        if (search_times_index.size() == 2) {
            search_times_index.resize(search_times_index[1] - search_times_index[0] + 1);
            iota(search_times_index.begin(), search_times_index.end(), search_times_index[0]);
            succeed = true;

            if (verbose >= 4) {
                cout << "A sequence of search times are generated: " << search_times_index << endl;
            }
        }
        
        if (!succeed) {
            cerr << BOLDRED << "Error: You have used --continuous-time, but "
                    << "both --test-times-index and --search-times-index cannot "
                    << "be converted to a range." << RESET << endl;
            return 1;
        }
    }

    if (verbose >= 5) {
        cout << "Input parameters:" << endl
                << "file_test_forecasts: " << file_test_forecasts << endl
                << "file_search_forecasts: " << file_search_forecasts << endl
                << "file_observations: " << file_observations << endl
                << "file_similarity: " << file_similarity << endl
                << "file_mapping: " << file_mapping << endl
                << "file_sds: " << file_sds << endl
                << "file_analogs: " << file_analogs << endl
                << "config_file: " << config_file << endl
                << "num_members: " << num_members << endl
                << "observation_id: " << observation_id << endl
                << "searchExtension: " << searchExtension << endl
                << "quick: " << quick << endl
                << "extend_observations: " << extend_observations << endl
                << "time_match_mode: " << time_match_mode << endl
                << "max_par_nan: " << max_par_nan << endl
                << "max_flt_nan: " << max_flt_nan << endl
                << "verbose: " << verbose << endl
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
                << "sds_count: " << sds_count << endl
                << "weights: " << weights << endl
                << "test_times_index: " << test_times_index << endl
                << "search_times_index: " << search_times_index << endl
                << "operational: " << operational << endl;
    }

    try {
        runAnalogGenerator(
                file_test_forecasts, test_start, test_count,
                file_search_forecasts, search_start, search_count,
                file_observations, obs_start, obs_count,
                file_mapping, file_sds, sds_start, sds_count,
                file_similarity, file_analogs,
                observation_id, searchExtension, max_neighbors,
                num_neighbors, distance, num_members, quick,
                extend_observations, time_match_mode, max_par_nan,
                max_flt_nan, test_times_index, search_times_index,
                weights, operational, verbose);
    } catch (...) {
        handle_exception(current_exception());
        return 1;
    }

#if defined(_ENABLE_MPI)
    AnEnIO::handle_MPI_Finalize();
#endif
    return (0);
}
