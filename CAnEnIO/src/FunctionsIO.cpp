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

using namespace std;
using namespace boost::gregorian;
namespace fs = boost::filesystem;

// These defines how to convert string-like times to double value times
static const size_t _SECONDS_IN_DAY = 24 * 60 * 60;
static const size_t _SECONDS_IN_HOUR = 60 * 60;

bool
FunctionsIO::parseFilename(Time & time, Time & flt,
        const string & file, const date & start_day,
        const regex & regex_day,
        const regex & regex_flt,
        size_t unit_in_seconds,
        bool delimited) {

    date current_day;
    smatch match_day, match_flt;

    // Apply the regular expression
    bool has_day = regex_search(file.begin(), file.end(), match_day, regex_day);
    if (!has_day) return false;

    bool has_flt = regex_search(file.begin(), file.end(), match_flt, regex_flt);
    if (!has_flt) return false;

    // Determine the current time
    if (delimited) current_day = from_string(string(match_day[1]));
    else current_day = from_undelimited_string(string(match_day[1]));

    // Calculate the associated time stamp
    size_t timestamp = (current_day - start_day).days() * _SECONDS_IN_DAY;

    time.timestamp = timestamp;
    flt.timestamp = stoi(match_flt[1]) * unit_in_seconds;

    return true;
}

bool
FunctionsIO::parseFilename(Time & time, Time & flt,
        const string & file, const date & start_day,
        const regex & regex_day,
        const regex & regex_flt,
        const regex & regex_cycle,
        size_t unit_in_seconds,
        bool delimited) {

    // Match only the day and the lead time parts
    bool ret = parseFilename(time, flt, file,
            start_day, regex_day, regex_flt,
            unit_in_seconds, delimited);

    // If the first step of matching has failed, return false
    if (!ret) return false;

    // If the first step of matching has gone through, match the cycle time
    smatch match_cycle;

    if (regex_search(file.begin(), file.end(), match_cycle, regex_cycle)) {
        time.timestamp += stoi(match_cycle[1]) * unit_in_seconds;
    } else {
        throw runtime_error("Cannot find cycle time using the regular expression");
    }

    return true;
}

void
FunctionsIO::parseFilenames(Times & times, Times & flts,
        const vector<string> & files,
        const string & regex_day_str,
        const string & regex_flt_str,
        const string & regex_cycle_str,
        size_t unit_in_seconds,
        bool delimited) {
    
    // File names must be sorted because times should be in order
    // and time will be extracted from file names
    //
    if (!is_sorted(files.begin(), files.end())) throw runtime_error("Filenames should be odered in ascension order");

    // Convert regular expression string to a regular expression object
    regex regex_day = regex(regex_day_str);
    regex regex_flt = regex(regex_flt_str);
    regex regex_cycle = regex(regex_cycle_str);

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
        if (regex_cycle_str.empty()) ret = parseFilename(
                time, flt, *it, start_day, regex_day, regex_flt,
                unit_in_seconds, delimited);
        else ret = parseFilename(
                time, flt, *it, start_day, regex_day, regex_flt,
                regex_cycle, unit_in_seconds, delimited);

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
FunctionsIO::listFiles(vector<string> & files, string& folder,
        const string& ext) {

    if (folder.empty()) throw runtime_error("Specify folder");

    files.clear();

    // Append a trailing slash if there is none
    if (folder.back() != '/') folder += '/';

    // List all files in a folder
    fs::recursive_directory_iterator it(folder), endit;
    while (it != endit) {

        // Check file extension
        if (fs::is_regular_file(*it) && it->path().extension() == ext) {
            files.push_back(folder + it->path().filename().string());
        }

        it++;
    }

    // Sort file names
    sort(files.begin(), files.end());

    return;
}
