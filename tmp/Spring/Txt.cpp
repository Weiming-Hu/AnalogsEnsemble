/* 
 * File:   ReadTxt.tpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on January 1, 2020, 6:06 PM
 */

#include "Txt.h"
#include "boost/filesystem.hpp"

namespace filesys = boost::filesystem;
using namespace std;

// TODO: This can be a helper function that is shared by other IO processes.
void
Txt::checkPath(const string & file_path, Mode mode, const std::string & extension) {
    
    // Validate the file path
    bool file_exists = filesys::exists(file_path);
    
    if (mode == Mode::Read && !file_exists) {
        ostringstream msg;
        msg << BOLDRED << "File not found: " << file_path << RESET;
        throw invalid_argument(msg.str());
    }
    
    if (mode == Mode::Write && file_exists) {
        ostringstream msg;
        msg << BOLDRED << "File exists: " << file_path << RESET;
        throw invalid_argument(msg.str());
    }

    filesys::path boost_path(file_path);

    // Succeed if the file path has right extension (Default is .txt)
    if (boost_path.has_extension()) {
        if (boost_path.extension().string() == extension) return;
    }

    // Fail if the file path does not have a different extension
    ostringstream msg;
    msg << BOLDRED << "Unknown extension: " << file_path << RESET;
    throw invalid_argument(msg.str());
}