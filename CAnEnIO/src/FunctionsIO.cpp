/* 
 * File:   FunctionsIO.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 * 
 * Created on February 7, 2020, 5:57 PM
 */


#include <set>
#include <array>
#include <cmath>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <algorithm>

#include "FunctionsIO.h"

#include "boost/filesystem.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/filesystem/operations.hpp"

using namespace std;
using namespace boost::gregorian;
using namespace boost::xpressive;
namespace fs = boost::filesystem;

// These defines how to convert string-like times to double value times
static const size_t _SECONDS_IN_DAY = 24 * 60 * 60;
static const size_t _SECONDS_IN_HOUR = 60 * 60;

void
FunctionsIO::toParameterVector(
        vector<ParameterGrib> & grib_parameters,
        const vector<string> & parameters_name,
        const vector<long> & parameters_id,
        const vector<long> & parameters_level,
        const vector<string> & parameters_level_type,
        const vector<bool> & parameters_circular,
        Verbose verbose) {

    // Convert parameter settings to grib parameters
    bool has_circular = false;

    // Make sure all vectors have the same length
    size_t num_parameters = parameters_id.size();
    if (num_parameters != parameters_level.size()) throw runtime_error("Different numbers of parameters ID and levels");
    if (num_parameters != parameters_name.size()) throw runtime_error("Different numbers of parameters ID and names");
    if (num_parameters != parameters_level_type.size()) throw runtime_error("Different numbers of parameters ID and level types");
    if (parameters_circular.size() != 0) {
        has_circular = true;
        if (num_parameters != parameters_circular.size()) throw runtime_error("Different numbers of parameters ID and circular flags");
    }

    // Clear existing values in the output vector
    grib_parameters.clear();

    for (size_t parameter_i = 0; parameter_i < num_parameters; ++parameter_i) {
        ParameterGrib grib_parameter(parameters_name[parameter_i],
                Config::_CIRCULAR, parameters_id[parameter_i],
                parameters_level[parameter_i], parameters_level_type[parameter_i]);

        if (has_circular) grib_parameter.setCircular(parameters_circular[parameter_i]);

        grib_parameters.push_back(grib_parameter);

        if (verbose >= Verbose::Debug) cout << grib_parameter << endl;
    }

    return;
}

bool
FunctionsIO::parseFilename(Time & time, Time & flt,
        const string & file, const date & start_day,
        const sregex & rex,
        size_t unit_in_seconds,
        bool delimited) {

    date current_day;
    smatch what;

    // Apply the regular expression
    bool success = regex_match(file, what, rex);
    if (!success) return false;

    // Determine the current date
    if (delimited) current_day = from_string(string(what["day"]));
    else current_day = from_undelimited_string(string(what["day"]));

    // Calculate the associated time stamp
    size_t timestamp = (current_day - start_day).days() * _SECONDS_IN_DAY;
    time.timestamp = timestamp;

    // Calculate the date time offset by cycle time
    time.timestamp += stoi(what["cycle"]) * unit_in_seconds;
    
    // Calculate the forecast lead time
    flt.timestamp = stoi(what["flt"]) * unit_in_seconds;

    return true;
}

void
FunctionsIO::parseFilenames(Times & times, Times & flts,
        const vector<string> & files,
        const string & regex_str,
        size_t unit_in_seconds,
        bool delimited) {
    
    // File names must be sorted because times should be in order
    // and time will be extracted from file names
    //
    if (!is_sorted(files.begin(), files.end())) throw runtime_error("Filenames should be odered in ascension order");

    // Convert regular expression string to a regular expression object
    sregex rex = sregex::compile(regex_str);

    // Start with clean repositories
    times.clear();
    flts.clear();

    bool ret;
    Time time, flt;

    // Determine our start day
    date start_day(from_string(Time::_origin));

    // Loop through each file from the file list
    auto it_end = files.end();
    for (auto it = files.begin(); it != it_end; ++it) {

        // Try to parse time information from the file name
        ret = parseFilename(time, flt, *it, start_day, rex, unit_in_seconds, delimited);

        // If information is successfully parsed, record the information;
        // otherwise, this file is removed from further process.
        //
        if (ret) {
            times.push_back(time);
            flts.push_back(flt);
        }
    }

    return;
}

void
FunctionsIO::listFiles(vector<string> & files,
        const string & folder, const string & regex_str) {

    // Get the total number of files in this folder
    size_t total_files = totalFiles(folder);

    // Allocate memory
    files.resize(total_files);

    // Compile the regular expression to make sure it is valid
    sregex rex = sregex::compile(regex_str);

    // Convert string to boost filesystem path
    fs::path folder_path(fs::system_complete(fs::path(folder.c_str())));

    // Extract filenames that matches the regular expression
    fs::directory_iterator it(folder), endit;

    size_t pos = 0;
    fs::path filepath;

    while (it != endit) {

        // If this is a regular file
        if (fs::is_regular_file(it->status())) {
            filepath = it->path().filename();

            // Check regular expression
            if (regex_match(filepath.string(), rex)) {
                files[pos] = (folder_path/filepath).string();
                pos++;
            }
        }

        it++;
    }

    // Erase the empty slots if not all files are included
    if (pos != total_files) files.erase(files.begin() + pos, files.end());

    // Sort file names
    sort(files.begin(), files.end());

    return;
}

size_t
FunctionsIO::totalFiles(const string & folder) {

    // Expand the input path using the system rules
    if (folder.empty()) throw runtime_error("Specify folder");
    fs::path full_path(fs::system_complete(fs::path(folder.c_str())));

    // Sanity checks
    if (!fs::exists(full_path)) throw runtime_error("Input path does not exists");
    if (!fs::is_directory(full_path)) throw runtime_error("Input path is not a directory");

    size_t total_files = 0;
    fs::directory_iterator it(full_path);
    fs::directory_iterator end_iter;

    for (; it != end_iter; ++it) {
        if (fs::is_regular_file(it->status())) total_files++;
    }

    return total_files;
}
