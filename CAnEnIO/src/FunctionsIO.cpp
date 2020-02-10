/* 
 * File:   FunctionsIO.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 * 
 * Created on February 7, 2020, 5:57 PM
 */


#include <set>
#include <cmath>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <algorithm>

#include "FunctionsIO.h"

using namespace std;
using namespace boost::gregorian;

static const size_t _SECONDS_IN_DAY = 24 * 60 * 60;
static const size_t _SECONDS_IN_HOUR = 60 * 60;

bool
FunctionsIO::parseFilename(Time & time, Time & flt,
        const string & file, const date & start_day,
        const regex & regex_day,
        const regex & regex_flt,
        double flt_unit_in_seconds,
        bool delimited) {

    date current_day;
    smatch match_day, match_flt, match_cycle;

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
    flt.timestamp = stoi(match_flt[1]) * flt_unit_in_seconds;

    return true;
}

bool
FunctionsIO::parseFilename(Time & time, Time & flt,
        const string & file, const date & start_day,
        const regex & regex_day,
        const regex & regex_flt,
        const regex & regex_cycle,
        double flt_unit_in_seconds,
        bool delimited) {

    // Match only the day and the lead time parts
    bool ret = parseFilename(time, flt, file,
            start_day, regex_day, regex_flt,
            flt_unit_in_seconds, delimited);

    // If the first step of matching has failed, return false
    if (!ret) return false;

    // If the first step of matching has gone through, match the cycle time
    smatch match_cycle;

    if (regex_search(file.begin(), file.end(), match_cycle, regex_cycle)) {
        time.timestamp += stoi(match_cycle[1]) * _SECONDS_IN_HOUR;
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
        double flt_unit_in_seconds,
        bool delimited) {

    // Convert regular expression string to a regular expression object
    regex regex_day = regex(regex_day_str);
    regex regex_flt = regex(regex_flt_str);
    regex regex_cycle = regex(regex_cycle_str);

    // Start with clean repositories
    times.clear();
    flts.clear();
    
    bool ret;
    string origin;
    Time time, flt;
    size_t timestamp, counter = 0;

    // Determine our start day
    date start_day(from_string(Time::_origin));

    // Loop through each file from the file list
    for (const auto file : files) {

        if (regex_cycle_str.empty()) ret = parseFilename(
                time, flt, file, start_day, regex_day, regex_flt,
                flt_unit_in_seconds, delimited);
        else  ret = parseFilename(
                time, flt, file, start_day, regex_day, regex_flt,
                regex_cycle, flt_unit_in_seconds, delimited);

        if (ret) {
            times.push_back(Times::value_type(counter, time));
            flts.push_back(Times::value_type(counter, flt));
            counter++;
        }
    }

    return;
}

