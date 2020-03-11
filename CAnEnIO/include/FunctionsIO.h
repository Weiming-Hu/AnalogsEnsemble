/* 
 * File:   FunctionsIO.h
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on February 7, 2020, 5:57 PM
 */

#ifndef FUNCTIONSIO_H
#define FUNCTIONSIO_H

#include <boost/xpressive/xpressive.hpp>
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
            const boost::xpressive::sregex & rex,
            size_t unit_in_seconds,
            bool delimited);

    void parseFilenames(Times&, Times&,
            const std::vector<std::string> & files,
            const std::string & regex_str,
            size_t unit_in_seconds,
            bool delimited);

    void listFiles(std::vector<std::string> & files,
            const std::string & folder,
            const std::string & regex_str);

    size_t totalFiles(const std::string & folder);
}

#endif /* FUNCTIONSIO_H */

