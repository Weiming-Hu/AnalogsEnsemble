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

void runFileAggregate(const string & file_type, const vector<string> & in_files, 
        const size_t & along, const string & out_file,
        const vector<size_t> & starts, const vector<size_t> & counts, const int & verbose) {

    // This variable determines whether we are reading the complete or partial
    // data from files
    //
    bool partial_read = true;
    
    AnEnIO io_out("Write", out_file, file_type, verbose);

    if (file_type == "Forecasts") {
        
        // Check input indices starts and counts
        if (starts.size() == 0 || counts.size() == 0) {
            partial_read = false;
        } else {
            if (starts.size() != in_files.size() * 4) {
                if (verbose >= 1) cout << BOLDRED << "Error: The length of start indices"
                        << " should be an integer multiplication of 4. "
                        << starts.size() << " indices found instead." << RESET << endl;
                return;
            }

            if (counts.size() != in_files.size() * 4) {
                if (verbose >= 1) cout << BOLDRED << "Error: The length of count indices"
                        << " should be an integer multiplication of 4. "
                        << counts.size() << " indices found instead." << RESET << endl;
                return;
            }
        }
        
        // Read files
        vector<Forecasts_array> forecasts_vec(in_files.size());
        vector<bool> flags(in_files.size(), false);
        if (verbose >= 3) cout << GREEN << "Reading files ... " << RESET << endl;
          
        for (size_t i = 0; i < in_files.size(); i++) {
            AnEnIO io("Read", in_files[i], file_type, verbose);

            if (partial_read) {
                io.handleError(io.readForecasts(forecasts_vec[i],
                            {starts[i*4], starts[i*4+1], starts[i*4+2], starts[i*4+3]},
                            {counts[i*4], counts[i*4+1], counts[i*4+2], counts[i*4+3]}));
            } else {
                io.handleError(io.readForecasts(forecasts_vec[i]));
            }
        }
        
        // Reshape data
        Forecasts_array forecasts;
        if (verbose >= 3) cout << GREEN << "Combining forecasts ..." << RESET << endl;
        auto ret = AnEnIO::combineForecastsArray(forecasts_vec, forecasts, along, verbose);
        if (ret != AnEnIO::errorType::SUCCESS) {
            throw runtime_error("Error: Failed when combining forecasts.");
        }
        
        // Write combined forecasts
        if (verbose >= 3) cout << GREEN << "Writing forecasts ..." << RESET << endl;
        io_out.handleError(io_out.writeForecasts(forecasts));
        if (verbose >= 3) cout << GREEN << "Done!" << RESET << endl;
        
        return;
    } else if (file_type == "Observations") {
        
        // Check input indices starts and counts
        if (starts.size() == 0 && counts.size() == 0) {
            partial_read = false;
        } else {
            if (starts.size() != in_files.size() * 3) {
                if (verbose >= 1) cout << BOLDRED << "Error: The length of start indices"
                        << " should be an integer multiplication of 3. "
                        << starts.size() << " indices found instead." << RESET << endl;
                return;
            }

            if (counts.size() != in_files.size() * 3) {
                if (verbose >= 1) cout << BOLDRED << "Error: The length of count indices"
                        << " should be an integer multiplication of 3. "
                        << counts.size() << " indices found instead." << RESET << endl;
                return;
            }
        }
        
        // Read files
        if (verbose >= 3) cout << GREEN << "Reading files ... " << RESET << endl;
        vector<Observations_array> observations_vec(in_files.size());
        vector<bool> flags(in_files.size(), false);
          
        for (size_t i = 0; i < in_files.size(); i++) {
            try{
                AnEnIO io("Read", in_files[i], file_type, verbose);
                
                if (partial_read) {
                    io.handleError(io.readObservations(observations_vec[i],
                            {starts[i*3], starts[i*3+1], starts[i*3+2]},
                            {counts[i*3], counts[i*3+1], counts[i*3+2]}));
                } else {
                    io.handleError(io.readObservations(observations_vec[i]));
                }
                
                flags[i] = true;
            } catch (...) {
                flags[i] = false;
            }
        }
        
        // Check if all elements in flags are true.
        for (size_t i = 0; i < flags.size(); i++) {
            if (!flags[i]) {
                if (verbose>=1) cout << BOLDRED << "Error: Error occurred when"
                        << " reading file " << in_files[i] << RESET << endl;
                return;
            }
        }

        // Reshape data
        if (verbose >= 3) cout << GREEN << "Combining observations ..." << RESET << endl;
        Observations_array observations;
        auto ret = AnEnIO::combineObservationsArray(observations_vec, observations, along, verbose);
        if (ret != AnEnIO::errorType::SUCCESS) {
            throw runtime_error("Error: Failed when combining observations.");
        }
        
        // Write combined forecasts
        if (verbose >= 3) cout << GREEN << "Writing observations ..." << RESET << endl;
        io_out.handleError(io_out.writeObservations(observations));
        if (verbose >= 3) cout << GREEN << "Done!" << RESET << endl;
        
        return;
        
    } else if (file_type == "StandardDeviation") {
        
        // Check input indices starts and counts
        if (starts.size() == 0 && counts.size() == 0) {
            partial_read = false;
        } else {
            if (starts.size() != in_files.size() * 4) {
                if (verbose >= 1) cout << BOLDRED << "Error: The length of start indices"
                        << " should be an integer multiplication of 3. "
                        << starts.size() << " indices found instead." << RESET << endl;
                return;
            }

            if (counts.size() != in_files.size() * 4) {
                if (verbose >= 1) cout << BOLDRED << "Error: The length of count indices"
                        << " should be an integer multiplication of 3. "
                        << counts.size() << " indices found instead." << RESET << endl;
                return;
            }
        }

        // Read files
        if (verbose >= 3) cout << GREEN << "Reading files ... " << RESET << endl;
        vector<StandardDeviation> sds_vec(in_files.size());
        vector<bool> flags(in_files.size(), false);

        for (size_t i = 0; i < in_files.size(); i++) {
            try {
                AnEnIO io("Read", in_files[i], file_type, verbose);

                if (partial_read) {
                    io.handleError(io.readStandardDeviation(sds_vec[i],
                        {starts[i * 3], starts[i * 3 + 1], starts[i * 3 + 2]},
                        {counts[i * 3], counts[i * 3 + 1], counts[i * 3 + 2]}));
                } else {
                    io.handleError(io.readStandardDeviation(sds_vec[i]));
                }

                flags[i] = true;
            } catch (...) {
                flags[i] = false;
            }
        }

        // Check if all elements in flags are true.
        for (size_t i = 0; i < flags.size(); i++) {
            if (!flags[i]) {
                if (verbose >= 1) cout << BOLDRED << "Error: Error occurred when"
                        << " reading file " << in_files[i] << RESET << endl;
                return;
            }
        }

        // Reshape data
        if (verbose >= 3) cout << GREEN << "Combining standard deviation ..." << RESET << endl;
        StandardDeviation sds;
        auto ret = AnEnIO::combineStandardDeviation(sds_vec, sds, along, verbose);
        if (ret != AnEnIO::errorType::SUCCESS) {
            throw runtime_error("Error: Failed when combining standard deviation.");
        }
        
        // Read parameters
        AnEnIO ioPar("Read", in_files[0], file_type, verbose);
        anenPar::Parameters parameters;
        if (partial_read) ioPar.readParameters(parameters, starts[0], counts[0]);
        else ioPar.readParameters(parameters);

        // Write combined standard deviation
        if (verbose >= 3) cout << GREEN << "Writing standard deviation ..." << RESET << endl;
        io_out.handleError(io_out.writeStandardDeviation(sds, parameters));
        
        // Write meta info from the first file in list
        AnEnIO io_meta("Read", in_files[0], file_type, verbose);
        if (io_meta.checkVariable("StationNames", true) != AnEnIO::errorType::SUCCESS) {
            anenSta::Stations stations;
            if (partial_read) io_meta.readStations(stations, starts[1], counts[1]);
            else io_meta.readStations(stations);
            io_out.handleError(io_out.writeStations(stations, false));
        }
        if (io_meta.checkVariable("FLTs", true) != AnEnIO::errorType::SUCCESS) {
            anenTime::FLTs flts;
            if (partial_read) io_meta.readFLTs(flts, starts[2], counts[2]);
            else io_meta.readFLTs(flts);
            io_out.handleError(io_out.writeFLTs(flts, false));
        }
        
        if (verbose >= 3) cout << GREEN << "Done!" << RESET << endl;

        return;
        
    } else if (file_type == "Similarity") {
        
        // Check input indices starts and counts
        if (starts.size() == 0 && counts.size() == 0) {
            // This is implemented. No further action needed.
        } else {
            throw runtime_error("Error: Function not implemented for partial IO of similarity files.");
        }
        
        // Read files
        if (verbose >= 3) cout << GREEN << "Reading files ... " << RESET << endl;
        vector<SimilarityMatrices> sims_vec(in_files.size());
        vector<bool> flags(in_files.size(), false);

        for (size_t i = 0; i < in_files.size(); i++) {
            try {
                AnEnIO io("Read", in_files[i], file_type, verbose);
                io.handleError(io.readSimilarityMatrices(sims_vec[i]));

                flags[i] = true;
            } catch (...) {
                flags[i] = false;
            }
        }

        // Check if all elements in flags are true.
        for (size_t i = 0; i < flags.size(); i++) {
            if (!flags[i]) {
                if (verbose >= 1) cout << BOLDRED << "Error: Error occurred when"
                        << " reading file " << in_files[i] << RESET << endl;
                return;
            }
        }

        // Reshape data
        if (verbose >= 3) cout << GREEN << "Combining similarity matrices ..." << RESET << endl;
        SimilarityMatrices sims;
        auto ret = AnEnIO::combineSimilarityMatrices(sims_vec, sims, along, verbose);
        if (ret != AnEnIO::errorType::SUCCESS) {
            throw runtime_error("Error: Failed when combining similarity matrices.");
        }

        // Write combined similarity matrices
        if (verbose >= 3) cout << GREEN << "Writing similarity matrices ..." << RESET << endl;
        io_out.handleError(io_out.writeSimilarityMatrices(sims));

        if (!sims.hasTargets() && along == 3) {
            // Write meta info from the first file in list
            AnEnIO io_meta("Read", in_files[0], file_type, verbose);
            if (io_meta.checkVariable("StationNames", true) == AnEnIO::errorType::SUCCESS) {
                anenSta::Stations stations;
                io_meta.readStations(stations);
                io_out.handleError(io_out.writeStations(stations, false));
            }
            if (io_meta.checkVariable("Times", true) == AnEnIO::errorType::SUCCESS) {
                anenTime::Times times;
                io_meta.readTimes(times);
                io_out.handleError(io_out.writeTimes(times, false));
            }
            if (io_meta.checkVariable("FLTs", true) == AnEnIO::errorType::SUCCESS) {
                anenTime::FLTs flts;
                io_meta.readFLTs(flts);
                io_out.handleError(io_out.writeFLTs(flts, false));
            }

        }
        
        if (verbose >= 3) cout << GREEN << "Done!" << RESET << endl;
        
        return;
    } else {
        if (verbose >= 1) cout << BOLDRED << "Error: File type " << file_type
                << " is not supported." << RESET << endl;
        return;
    }
}

int main(int argc, char** argv) {

    namespace po = boost::program_options;

    // Required variables
    string file_type, out_file;
    vector<string> in_files;
    size_t along = 0;

    // Optional variables
    int verbose = 0;
    string config_file;
    vector<size_t> starts(0), counts(0);

    try {
        po::options_description desc("Available options");
        desc.add_options()
                ("help,h", "Print help information for options.")
                ("config,c", po::value<string>(&config_file), "Set the configuration file path. Command line options overwrite options in configuration file. ")

                ("type,t", po::value<string>(&file_type)->required(), "Set the type of the files to read. It can be either Forecasts ,Observations, Similarity, or StandardDeviation.")
                ("in,i", po::value< vector<string> >(&in_files)->multitoken()->required(), "Set the file names to read separated by a space.")
                ("out,o", po::value<string>(&out_file)->required(), "Set the name of the output file.")
                ("along,a", po::value<size_t>(&along)->required(), "Set the dimension index to be appended. It counts from 0.")

                ("start", po::value<vector<size_t> >(&starts)->multitoken(), "Set the start indices for each files.")
                ("count", po::value<vector<size_t> >(&counts)->multitoken(), "Set the count numbers for each files.")
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
            cout << GREEN << "Analog Ensemble program --- File Aggregate"
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
                << "file_type: " << file_type << endl
                << "in_files: " << in_files << endl
                << "out_file: " << out_file << endl
                << "along: " << along << endl
                << "verbose: " << verbose << endl
                << "starts: " << starts << endl
                << "counts: " << counts << endl;
    }
    
    if (file_type == "Forecasts" && along >= 4) {
        if (verbose >= 1) cout << BOLDRED 
                << "Error: Forecasts should only have 4 dimensions. "
                << "Invalid along parameter (" << along << ")!" << RESET << endl;
        return 1;
    } else if (file_type == "Observations" && along >= 3) {
        if (verbose >= 1) cout << BOLDRED
                << "Error: Observations should only have 3 dimensions. "
                << "Invalid along parameter (" << along << ")!" << RESET << endl;
        return 1;
    }

    try {
        runFileAggregate(file_type, in_files, along, out_file, starts, counts, verbose);
    } catch (...) {
        handle_exception(current_exception());
        return 1;
    }

    return 0;
}


