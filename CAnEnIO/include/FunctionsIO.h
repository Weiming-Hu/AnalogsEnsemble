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

#include "Times.h"

namespace FunctionsIO {

    void parseFiles(Times&, Times&, std::vector<std::string> & files,
            const std::string & regex_time_str,
            const std::string & regex_flt_str,
            const std::string & regex_cycle_str,
            double flt_unit_in_seconds,
            bool delimited);

    void parseFile(Time&, Time&, const std::string & file,
            const std::string & regex_time_str,
            const std::string & regex_flt_str,
            const std::string & regex_cycle_str,
            double flt_unit_in_seconds, bool delimited);
};

#endif /* FUNCTIONSIO_H */

