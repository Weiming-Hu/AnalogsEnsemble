/* 
 * File:   ReadTxt.tpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on January 1, 2020, 6:06 PM
 */

#include "ReadTxt.h"
#include "boost/filesystem.hpp"

namespace filesys = boost::filesystem;
using namespace std;

void
ReadTxt::checkPath(const string & file_path) {
    
    // Check whether the file exists
    bool file_exists = filesys::exists(file_path);
    if (!file_exists) {
        ostringstream msg;
        msg << BOLDRED << "File not found " << file_path << RESET;
        throw invalid_argument(msg.str());
    }

    filesys::path boost_path(file_path);

    // Succeed if the file path has .txt as its extension
    if (boost_path.has_extension()) {
        if (boost_path.extension().string() == ".txt") return;
    }

    // Fail if the file path does not have a .nc extension
    ostringstream msg;
    msg << BOLDRED << "Unknown extension " << file_path << RESET;
    throw invalid_argument(msg.str());
}

