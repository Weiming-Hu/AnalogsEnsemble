/* 
 * File:   FunctionsIO.h
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on February 7, 2020, 5:57 PM
 */

#ifndef FUNCTIONSIO_H
#define FUNCTIONSIO_H

#include <boost/regex.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

#include <vector>
#include <string>

#include "Times.h"
#include "Config.h"
#include "ParameterGrib.h"

namespace FunctionsIO {

    void toParameterVector(std::vector<ParameterGrib> &,
        const std::vector<std::string> & parameters_name,
        const std::vector<long> & parameters_id,
        const std::vector<long> & parameters_level,
        const std::vector<std::string> & parameters_level_type,
        const std::vector<bool> & parameters_circular = {},
        Verbose verbose = Verbose::Warning);

    bool parseFilename(Time & time, Time & flt,
            const std::string & file,
            const boost::gregorian::date & start_day,
            const boost::regex & regex_day,
            const boost::regex & regex_flt,
            const boost::regex & regex_cycle,
            size_t unit_in_seconds,
            bool delimited);

    bool parseFilename(Time & time, Time & flt,
            const std::string & file,
            const boost::gregorian::date & start_day,
            const boost::regex & regex_day,
            const boost::regex & regex_flt,
            size_t unit_in_seconds,
            bool delimited);

    void parseFilenames(Times&, Times&,
            const std::vector<std::string> & files,
            const std::string & regex_day_str,
            const std::string & regex_flt_str,
            const std::string & regex_cycle_str,
            size_t unit_in_seconds,
            bool delimited);

    void listFiles(std::vector<std::string> & files,
            std::string & folder,
            const std::string & ext = ".grb2");
};

#endif /* FUNCTIONSIO_H */

