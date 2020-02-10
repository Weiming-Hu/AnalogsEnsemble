/* 
 * File:   FunctionsIO.h
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on February 7, 2020, 5:57 PM
 */

#ifndef FUNCTIONSIO_H
#define FUNCTIONSIO_H

#include <vector>
#include <string>
#include <regex>

#include "Forecasts.h"
#include "Observations.h"

#include "boost/date_time/gregorian/gregorian.hpp"

namespace FunctionsIO {

    bool parseFilename(Time & time, Time & flt,
            const std::string & file,
            const boost::gregorian::date & start_day,
            const std::regex & regex_day,
            const std::regex & regex_flt,
            const std::regex & regex_cycle,
            double unit_in_seconds,
            bool delimited);

    bool parseFilename(Time & time, Time & flt,
            const std::string & file,
            const boost::gregorian::date & start_day,
            const std::regex & regex_day,
            const std::regex & regex_flt,
            double unit_in_seconds,
            bool delimited);

    void parseFilenames(Times&, Times&,
            const std::vector<std::string> & files,
            const std::string & regex_day_str,
            const std::string & regex_flt_str,
            const std::string & regex_cycle_str,
            double unit_in_seconds,
            bool delimited);
    
    void collapseLeadTimes(Observations &, const Forecasts &);
};

#endif /* FUNCTIONSIO_H */

