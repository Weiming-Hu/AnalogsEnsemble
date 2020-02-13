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

void
Txt::checkPath(const string & file_path, Mode mode, const std::string & extension) {
    
    // Validate the file path
    bool file_exists = filesys::exists(file_path);
    
    if (mode == Mode::Read && !file_exists) {
        ostringstream msg;
        msg << "File not found: " << file_path;
        throw invalid_argument(msg.str());
    }
    
    if (mode == Mode::Write && file_exists) {
        ostringstream msg;
        msg << "File exists: " << file_path;
        throw invalid_argument(msg.str());
    }

    filesys::path boost_path(file_path);

    // Succeed if the file path has right extension (Default is .txt)
    if (boost_path.has_extension()) {
        if (boost_path.extension().string() == extension) return;
    }

    // Fail if the file path does not have a different extension
    ostringstream msg;
    msg << "Unknown extension: " << file_path;
    throw invalid_argument(msg.str());
}
