/* 
 * File:   similarityCalculator.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on September 17, 2018, 5:20 PM
 */

/** @file */

#include "gribConverterFunctions.h"

using namespace gribConverter;

int main(int argc, char** argv) {

    namespace po = boost::program_options;

    // Required variables
    string folder, file_out, regex_time_str, regex_flt_str, output_type;
    vector<long> pars_id, levels;
    vector<string> level_types;
    double unit_flt = 0;

    // Optional variables
    int verbose = 0;
    bool delimited = false, overwrite_output = false, skip_data = false;
    string ext, config_file, par_key, level_key, type_key, val_key;
    vector<long> crcl_pars_id;
    vector<string> pars_new_name;

    try {
        po::options_description desc("Available options");
        desc.add_options()
                ("help,h", "Print help information for options.")
                ("config,c", po::value<string>(), "Set the configuration file path. Command line options overwrite options in configuration file.")

                ("output-type", po::value<string>(&output_type)->required(), "Set the output type. Currently it supports 'Forecasts' and 'Observations'.")
                ("folder", po::value<string>(&folder)->required(), "Set the data folder.")
                ("output,o", po::value<string>(&file_out)->required(), "Set the output file path.")
                ("regex-time", po::value<string>(&regex_time_str)->required(), "Set the regular expression for time. Time information will be extracted from the file name.")

                // This is required for Forecasts but not for Observations. I will deal with this separately.
                ("regex-flt", po::value<string>(&regex_flt_str), "Set the regular expression for FLT. FLT information will be extracted from the file name.")
                ("flt-interval", po::value<double>(&unit_flt), "Set the interval in seconds for FLT.")

                ("pars-id", po::value< vector<long> >(&pars_id)->multitoken()->required(), "Set the parameters ID that will be read from the file.")
                ("levels", po::value< vector<long> >(&levels)->multitoken()->required(), "Set the levels for each parameters.")
                ("level-types", po::value< vector<string> >(&level_types)->multitoken()->required(), "Set the types of levels for each parameters.")
                
                ("delimited", po::bool_switch(&delimited)->default_value(false), "The extracted time message is delimited by ambiguous character (1990-01-01).")
                ("ext", po::value<string>(&ext)->default_value(".grb2"), "Set the file extension.")
                ("par-key", po::value<string>(&par_key)->default_value("paramId"), "Set the parameter ID key name in GRB file.")
                ("level-key", po::value<string>(&level_key)->default_value("level"), "Set the level key name in GRB file.")
                ("type-key", po::value<string>(&type_key)->default_value("typeOfLevel"), "Set the type of level key name in GRB file.")
                ("value-key", po::value<string>(&val_key)->default_value("values"), "Set the value key name in GRB file.")
                ("circulars-id", po::value< vector<long> >(&crcl_pars_id)->multitoken(), "Set the IDs of circular parameters.")
                ("parameters-new-name", po::value< vector<string> >(&pars_new_name)->multitoken(), "Set the new names of each parameter.")
                ("overwrite", po::bool_switch(&overwrite_output)->default_value(false), "Overwrite the output file.")
                ("skip-data", po::bool_switch(&skip_data)->default_value(false), "Skip writing data to the file. Use this option to only write header information to the file.")
                ("verbose,v", po::value<int>(&verbose)->default_value(0), "Set the verbose level.");

        // process unregistered keys and notify users about my guesses
        vector<string> available_options;
        auto lambda = [&available_options](const boost::shared_ptr<boost::program_options::option_description> option) {
            available_options.push_back("--" + option->long_name());
        };
        for_each(desc.options().begin(), desc.options().end(), lambda);

        // Parse the command line first
        po::variables_map vm;
        po::parsed_options parsed = po::command_line_parser(argc, argv).options(desc).allow_unregistered().run();
        store(parsed, vm);

        cout << BOLDGREEN << "Parallel Ensemble Forecasts --- GRIB Converter "
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
            // If configuration file is specfied, read it first.
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

        if (vm.count("parameters-new-name")) {
            if (pars_new_name.size() != pars_id.size()) {
                throw runtime_error("Error: the number of new names and parameters do not match!");
            }
        } else {
            pars_new_name.clear();
            for (const auto & i : pars_id) {
                pars_new_name.push_back(to_string(i));
            }
        }

        if (output_type == "Forecasts") {
            if (!vm.count("flt-interval")) {
                cout << BOLDRED << "Error: flt-interval option is required for Forecasts." << RESET << endl;
                return 1;
            }
            
            if (!vm.count("regex-flt")) {
                cout << BOLDRED << "Error: regex-flt option is required for Forecasts." << RESET << endl;
                return 1;
            }
            
        } else if (output_type != "Observations") {
            cout << BOLDRED << "Error: Specified output type (" << output_type << ") is not supported." << RESET << endl;
            return 1;
        }

    } catch (...) {
        handle_exception(current_exception());
        return 1;
    }

    // List all files in a folder
    fs::recursive_directory_iterator it(folder), endit;
    vector<string> files_in;
    while (it != endit) {
        if (fs::is_regular_file(*it) && it->path().extension() == ext)
            files_in.push_back(folder + it->path().filename().string());
        it++;
    }

    // Sort the file list so that files are in incremental order
    sort(files_in.begin(), files_in.end());

    if (fs::exists(fs::path(file_out))) {
        if (overwrite_output) {
            fs::remove(fs::path(file_out));
            if (verbose >= 3) cout << GREEN << "Remove existing output file " << file_out << RESET << endl;
        } else {
            cout << BOLDRED << "Error: Output file exists!" << RESET << endl;
            return 1;
        }
    }

    if (verbose >= 5) {
        cout << "folder: " << folder << endl
                << "file_out: " << file_out << endl
                << "regex_time_str: " << regex_time_str << endl
                << "regex_flt_str: " << regex_flt_str << endl
                << "output_type: " << output_type << endl
                << "pars_id: " << pars_id << endl
                << "levels: " << levels << endl
                << "verbose: " << verbose << endl
                << "delimited: " << delimited << endl
                << "ext: " << ext << endl
                << "unit_flt: " << unit_flt << endl
                << "level_types: " << level_types << endl
                << "par_key: " << par_key << endl
                << "level_key: " << level_key << endl
                << "type_key: " << type_key << endl
                << "val_key: " << val_key << endl
                << "config_file: " << config_file << endl
                << "crcl_pars_id: " << crcl_pars_id << endl
                << "pars_new_name: " << pars_new_name << endl
                << "overwrite_output: " << overwrite_output << endl
                << "skip_data: " << skip_data << endl;
        
    }

    // Basic checks
    if (files_in.size() == 0) throw runtime_error("Error: No files input specified!");
    if (pars_id.size() == 0) throw runtime_error("Error: No parameter Id specified!");
    if (pars_id.size() != pars_new_name.size())
        throw runtime_error("Error: The numbers of parameter Ids and new names!");
    if (pars_id.size() != levels.size())
        throw runtime_error("Error: The numbers of parameter Ids and levels do not match!");    
    if (pars_id.size() != level_types.size())
        throw runtime_error("Error: The numbers of parameter Ids and level types do not match!");
    
    // Call function to convert GRIB
    if (output_type == "Forecasts") {
        toForecasts(files_in, file_out, pars_id, pars_new_name, crcl_pars_id,
                levels, level_types, par_key, level_key, type_key, val_key, 
                regex_time_str, regex_flt_str, unit_flt, delimited, skip_data, verbose);
    } else if (output_type == "Observations") {
        toObservations(files_in, file_out, pars_id, pars_new_name, crcl_pars_id,
                levels, level_types, par_key, level_key, type_key, val_key,
                regex_time_str, delimited, skip_data, verbose);
    } else {
        cout << BOLDRED << "Error: Output type " << output_type << " is not supported!"
                << RESET << endl;
        return 1;
    }

    return 0;
}
