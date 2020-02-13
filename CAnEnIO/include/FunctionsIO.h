/* 
 * File:   FunctionsIO.h
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on February 7, 2020, 5:57 PM
 */

#ifndef FUNCTIONSIO_H
#define FUNCTIONSIO_H

#include <regex>
#include <vector>
#include <string>

#include "Times.h"
#include "boost/date_time/gregorian/gregorian.hpp"

namespace FunctionsIO {

    bool parseFilename(Time & time, Time & flt,
            const std::string & file,
            const boost::gregorian::date & start_day,
            const std::regex & regex_day,
            const std::regex & regex_flt,
            const std::regex & regex_cycle,
            size_t unit_in_seconds,
            bool delimited);

    bool parseFilename(Time & time, Time & flt,
            const std::string & file,
            const boost::gregorian::date & start_day,
            const std::regex & regex_day,
            const std::regex & regex_flt,
            size_t unit_in_seconds,
            bool delimited);

    void parseFilenames(Times&, Times&,
            std::vector<std::string> files,
            const std::string & regex_day_str,
            const std::string & regex_flt_str,
            const std::string & regex_cycle_str,
            size_t unit_in_seconds,
            bool delimited);

    void listFiles(std::vector<std::string> & files, std::string & folder,
            const std::string & ext = ".grb2");
};

#endif /* FUNCTIONSIO_H */
