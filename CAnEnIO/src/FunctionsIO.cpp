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

using namespace std;
using namespace boost::gregorian;

/*
 * This definition is used to record the mapping from observation times to
 * forecast times and lead times. This is the reverse of the observation time
 * index table which goes from forecast times and lead times to observation times.
 */
using time_arr = array<size_t, 3>;

// These defines the value type on each position in the time array.
static const size_t _VALUE_INDEX = 0;
static const size_t _TIME_INDEX = 1;
static const size_t _FLT_INDEX = 2;

// These defines how to convert string-like times to double value times
static const size_t _SECONDS_IN_DAY = 24 * 60 * 60;
static const size_t _SECONDS_IN_HOUR = 60 * 60;

/*
 * This defines an operator for comparing observation time entry which is based
 * on the first position, time stamp value.
 */
struct time_arr_compare {

    bool operator()(const time_arr & lhs, const time_arr & rhs) const {
        return lhs[_VALUE_INDEX] < rhs[_VALUE_INDEX];
    }
};

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

    // Convert regular expression string to a regular expression object
    regex regex_day = regex(regex_day_str);
    regex regex_flt = regex(regex_flt_str);
    regex regex_cycle = regex(regex_cycle_str);

    // Start with clean repositories
    times.clear();
    flts.clear();

    bool ret;
    Time time, flt;
    size_t counter = 0;

    // Determine our start day
    date start_day(from_string(Time::_origin));

    // Loop through each file from the file list
    for (const auto file : files) {

        if (regex_cycle_str.empty()) ret = parseFilename(
                time, flt, file, start_day, regex_day, regex_flt,
                unit_in_seconds, delimited);
        else ret = parseFilename(
                time, flt, file, start_day, regex_day, regex_flt,
                regex_cycle, unit_in_seconds, delimited);

        if (ret) {
            times.push_back(Times::value_type(counter, time));
            flts.push_back(Times::value_type(counter, flt));
            counter++;
        }
    }

    return;
}

void
FunctionsIO::collapseLeadTimes(
        Observations & observations,
        const Forecasts & forecasts) {

    // Calculate the unique time combination from forecast times and lead times
    time_arr time_entry;
    set<time_arr, time_arr_compare> unique_times;

    const auto & fcst_times = forecasts.getTimes();
    const auto & fcst_flts = forecasts.getFLTs();

    /*
     * Insert times from forecasts to observations.
     * 
     * Please note that I'm looping first on forecast lead times and then forecast
     * times because I'm giving priority to earlier forecast lead times compared
     * to later lead time that are further into the future. I will keep the values
     * that are close to the initialization time.
     */
    for (size_t flt_i = 0; flt_i < fcst_flts.size(); ++flt_i) {
        for (size_t time_i = 0; time_i < fcst_times.size(); ++time_i) {

            time_entry[_TIME_INDEX] = time_i;
            time_entry[_FLT_INDEX] = flt_i;
            time_entry[_VALUE_INDEX] = fcst_times.getTime(time_i).timestamp
                    + fcst_flts.getTime(flt_i).timestamp;

            unique_times.insert(time_entry);
        }
    }

    // Insert the sorted unique times into observation times
    Times obs_times;
    size_t time_i = 0;
    const auto & unique_times_end = unique_times.end();
    for (auto it = unique_times.begin(); it != unique_times_end; ++it, ++time_i) {
        obs_times.push_back(Times::value_type(time_i, (*it)[_VALUE_INDEX]));
    }

    // Set dimensions for observations
    observations.setDimensions(forecasts.getParameters(), forecasts.getStations(), obs_times);

    // Copy values from forecasts
    time_i = 0;
    double value;
    size_t num_parameters = observations.getParameters().size();
    size_t num_stations = observations.getStations().size();

    for (auto it = unique_times.begin(); it != unique_times_end; ++it, ++time_i) {
        for (size_t parameter_i = 0; parameter_i < num_parameters; ++parameter_i) {
            for (size_t station_i = 0; station_i < num_stations; ++station_i) {
                value = forecasts.getValue(parameter_i, station_i, (*it)[_TIME_INDEX], (*it)[_FLT_INDEX]);
                observations.setValue(value, parameter_i, station_i, time_i);
            }
        }
    }

    return;
}