/* 
 * File:   analogGenerator.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *         Guido Cervone <cervone@psu.edu>
 *
 * Created on April 17, 2018, 11:10 PM
 */

/** @file */

#include "AnEnIO.h"
#include "AnEn.h"
#include "CommonExeFunctions.h"

#include "boost/program_options.hpp"
#include "boost/multi_array.hpp"
#include "boost/filesystem.hpp"
#include "boost/exception/all.hpp"

using namespace std;

void runAnalogGenerator(
        const string & file_test_forecasts,
        const vector<size_t> & test_start,
        const vector<size_t> & test_count,

        const string & file_search_forecasts,
        const vector<size_t> & search_start,
        const vector<size_t> & search_count,

        const string & file_observations,
        const vector<size_t> & obs_start,
        const vector<size_t> & obs_count,

        const string & file_mapping,
        const string & file_analog,

        size_t observation_id,

        bool searchExtension, size_t max_neighbors,
        size_t num_neighbors, double distance, 

        size_t num_members, bool quick,
        bool preserve_real_time, int verbose) {

    /************************************************************************
     *                         Read Input Data                              *
     ************************************************************************/
    Forecasts_array test_forecasts, search_forecasts;
    Observations_array search_observations;

    AnEnIO io("Read", file_test_forecasts, "Forecasts", verbose);

    if (test_start.empty() || test_count.empty()) {
        io.handleError(io.readForecasts(test_forecasts));
    } else {
        io.handleError(io.readForecasts(test_forecasts,
                test_start, test_count));
    }

    io.setFilePath(file_search_forecasts);
    if (search_start.empty() || search_count.empty()) {
        io.handleError(io.readForecasts(search_forecasts));
    } else {
        io.handleError(io.readForecasts(search_forecasts,
                search_start, search_count));
    }

    io.setFileType("Observations");
    io.setFilePath(file_observations);
    if (obs_start.empty() || obs_count.empty()) {
        io.handleError(io.readObservations(search_observations));
    } else {
        io.handleError(io.readObservations(search_observations,
                obs_start, obs_count));
    }


    /************************************************************************
     *                           Analog Generation                          *
     ************************************************************************/
    AnEn anen(verbose);

    SimilarityMatrices sims(test_forecasts);

    StandardDeviation sds;
    anen.handleError(anen.computeStandardDeviation(search_forecasts, sds));

    AnEn::TimeMapMatrix mapping;
    anen.handleError(anen.computeObservationsTimeIndices(
                search_forecasts.getTimes(), search_forecasts.getFLTs(),
                search_observations.getTimes(), mapping));
    if (!file_mapping.empty()) {
        io.setFileType("Matrix");
        io.setMode("Write", file_mapping);
        io.handleError(io.writeTextMatrix(mapping));
    }

    if (searchExtension) {
        anen.setMethod(AnEn::simMethod::ONE_TO_MANY);

        AnEn::SearchStationMatrix i_search_stations;
        
        anen.handleError(anen.computeSearchStations(
                test_forecasts.getStations(),
                search_forecasts.getStations(),
                i_search_stations, max_neighbors,
                distance, num_neighbors, true));
        anen.handleError(anen.computeSimilarity(
                search_forecasts, sds, sims, search_observations, mapping,
                i_search_stations, observation_id));

    } else {
        anen.setMethod(AnEn::simMethod::ONE_TO_ONE);

        anen.handleError(anen.computeSimilarity(
                search_forecasts, sds, sims, search_observations,
                mapping, observation_id));
    }

    Analogs analogs;
    anen.handleError(anen.selectAnalogs(analogs, sims, search_observations,
            mapping, observation_id, num_members, quick, preserve_real_time));


    /************************************************************************
     *                           Write Analogs                              *
     ************************************************************************/
    io.setMode("Write", file_analog);
    io.handleError(io.writeAnalogs(analogs));
    
    return;
}

int main(int argc, char** argv) {
    cout << "Analog Generator underdevelopment ..." << endl;
    return (0);
}
