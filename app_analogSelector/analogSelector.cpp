/* 
 * File:   analogSelector.cpp
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <weiming@psu.edu>
 *
 * Created on April 17, 2018, 11:10 PM
 */

/** @file */

#include "AnEnIO.h"
#include "AnEn.h"

using namespace std;

void runAnalogSelector(const string & file_sims, const string & file_obs,
        const vector<size_t> & obs_start, const vector<size_t> & obs_count,
        const string & file_mapping, const string & file_out, size_t i_parameter,
        size_t num_numbers, bool quick, bool preserve_real_time, int verbose) {
    
    /**************************************************************************
     *                                Prepare Input                           *
     **************************************************************************/
    Analogs analogs;
    
    AnEn anen(verbose);
    AnEnIO io("Read");
    
    SimilarityMatrices sims;
    io.setFilePath(file_sims);
    io.setFileType("Similarity");
    io.readSimilarityMatrices(sims);
    
    Observations_array search_observations;
    io.setFileType("Observations");
    io.setFilePath(file_obs);
    io.readForecasts(search_observations, obs_start, obs_count);
    
    AnEn::TimeMapMatrix mapping;
    if (file_mapping.empty()) {
        // Calculate mapping
    } else {
        // Read mapping from file
    }
    throw runtime_error("How to deal with mapping!!!");
    
    
    /**************************************************************************
     *                                Check Input                             *
     **************************************************************************/
    if (mapping.size1() == sims.getTargets().getTimesSize()) {
        stringstream ss;
        ss << BOLDRED << "Error: Number of rows in mapping should match "
                << " number of forecast times!" << RESET << endl;
        throw runtime_error(ss.str());
    }
    
    if (mapping.size2() == sims.getTargets().getFLTsSize()) {
        stringstream ss;
        ss << BOLDRED << "Error: Number of columns in mapping should match "
                << " number of forecast forecast lead times!" << RESET << endl;
        throw runtime_error(ss.str());
    }
    
    
    /**************************************************************************
     *                             Select Analogs                             *
     **************************************************************************/
    anen.handleError(anen.selectAnalogs(analogs, sims, search_observations,
            mapping, i_parameter, num_numbers, quick, preserve_real_time));
    
    
    /**************************************************************************
     *                             Write Analogs                              *
     **************************************************************************/
    io.setMode("Write", file_out);
    io.handleError(io.writeAnalogs(analogs));
    
    return;
}

int main(int argc, char** argv) {
    cout << "Analog Selector ..." << endl;
    
    // Check files are valid and expected
    
    return (0);
}
