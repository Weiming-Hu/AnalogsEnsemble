/*
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on August 15, 2018
 *
 * Rinterface.cpp is the source file with the interface definition for
 * R functions to call C++ functions.
 */

// [[Rcpp::plugins(cpp11)]]
// [[Rcpp::plugins(openmp)]]
//
#include <Rcpp.h>
#include <iostream>

#include <boost/numeric/ublas/io.hpp>

#include "AnEn.h"

using namespace Rcpp;

#if defined(_OPENMP)
#include <omp.h>
#endif

// Rcpp::Rcout is the wrapper for std::cout in Rcpp
#define cout Rcpp::Rcout
#define cerr Rcpp::Rcerr
#define endl std::endl

// [[Rcpp::export("checkOpenMP")]]

bool checkOpenMP() {
#ifndef _OPENMP
    return false;
#else
    return true;
#endif
}

// [[Rcpp::export(".generateAnalogs")]]

List generateAnalogs(
        NumericVector R_test_forecasts, NumericVector R_test_forecasts_dims,
        NumericVector R_test_forecasts_station_x,
        NumericVector R_test_forecasts_station_y,
        NumericVector R_search_forecasts, NumericVector R_search_forecasts_dims,
        NumericVector R_search_forecasts_station_x,
        NumericVector R_search_forecasts_station_y,
        NumericVector R_search_times, NumericVector R_search_flts,
        NumericVector R_search_observations,
        NumericVector R_search_observations_dims,
        NumericVector R_observation_times,
        size_t num_members, size_t observation_parameter, bool quick,
        IntegerVector R_circulars, bool search_extension,
        bool preserve_real_time, bool preserve_similarity,
        bool preserve_mapping, bool preserve_search_stations,
        size_t max_num_search_stations, double distance,
        size_t num_nearest_stations, int verbose) {

    /***************************************************************************
     *                   Convert objects for AnEn computation                  *
     **************************************************************************/
    if (verbose >= 3)
        cout << "Convert R objects to C++ objects ..." << endl;
    anenTime::Times search_times;
    search_times.insert(search_times.end(),
            R_search_times.begin(), R_search_times.end());

    anenTime::FLTs flts;
    flts.insert(flts.end(), R_search_flts.begin(), R_search_flts.end());

    anenTime::Times observation_times;
    observation_times.insert(observation_times.end(),
            R_observation_times.begin(), R_observation_times.end());

    std::vector<anenPar::Parameter> parameters_vec(R_test_forecasts_dims[0]);
    for (auto & pos : R_circulars) {
        parameters_vec[pos - 1].setCircular(true);
    }
    anenPar::Parameters parameters;
    parameters.insert(parameters.end(),
            parameters_vec.begin(), parameters_vec.end());

    anenSta::Stations test_stations;
    if (search_extension) {
        for (size_t i_station = 0; i_station < R_test_forecasts_dims[1];
                i_station++) {
            anenSta::Station s(R_test_forecasts_station_x[i_station],
                    R_test_forecasts_station_y[i_station]);
            test_stations.push_back(s);
        }
    } else {
        test_stations.get<anenSta::by_insert>().resize(
                R_test_forecasts_dims[1]);
    }

    std::vector<double> test_times_vec(R_test_forecasts_dims[2], 0);
    iota(test_times_vec.begin(), test_times_vec.end(), 0);
    anenTime::Times test_times;
    test_times.insert(test_times.end(),
            test_times_vec.begin(), test_times_vec.end());

    anenSta::Stations search_stations;
    if (search_extension) {
        for (size_t i_station = 0; i_station < R_search_forecasts_dims[1];
                i_station++) {
            anenSta::Station s(R_search_forecasts_station_x[i_station],
                    R_search_forecasts_station_y[i_station]);
            search_stations.push_back(s);
        }
    } else {
        search_stations.get<anenSta::by_insert>().resize(
                R_search_forecasts_dims[1]);
    }

    anenPar::Parameters observation_parameters;
    observation_parameters.get<anenPar::by_insert>().resize(
            R_search_observations_dims[0]);

    Forecasts_array test_forecasts(
            parameters, test_stations, test_times, flts);
    test_forecasts.data().assign(
            R_test_forecasts.begin(), R_test_forecasts.end());

    Forecasts_array search_forecasts(
            parameters, search_stations, search_times, flts);
    search_forecasts.data().assign(
            R_search_forecasts.begin(), R_search_forecasts.end());

    Observations_array search_observations(
            observation_parameters, search_stations, observation_times);
    search_observations.data().assign(
            R_search_observations.begin(), R_search_observations.end());

    /***************************************************************************
     *                           AnEn Computation                              *
     **************************************************************************/
    AnEn anen(verbose);
    Analogs analogs;
    SimilarityMatrices sims(test_forecasts);
    boost::numeric::ublas::matrix<size_t> mapping;
    StandardDeviation sds(parameters.size(),
            search_stations.size(), flts.size());

    if (verbose >= 4) {
        cout << "A summary of test forecast parameters:" << endl
                << sims.getTargets().getParameters();
    }

    if (search_extension) anen.setMethod(AnEn::simMethod::ONE_TO_MANY);
    else anen.setMethod(AnEn::simMethod::ONE_TO_ONE);

    // Pre compute the standard deviation
    anen.handleError(anen.computeStandardDeviation(search_forecasts, sds));

    // Pre compute the time mapping from forecasts [Times, FLTs] 
    // to observations [Times]
    //
    anen.handleError(anen.computeObservationsTimeIndices(
            search_forecasts.getTimes(), search_forecasts.getFLTs(),
            search_observations.getTimes(), mapping));

    // Compute similarity
    boost::numeric::ublas::matrix<double> i_search_stations;
    if (search_extension) {
        bool return_index = true;

        anen.handleError(anen.computeSearchStations(
                test_forecasts.getStations(), search_forecasts.getStations(),
                i_search_stations, max_num_search_stations, distance,
                num_nearest_stations, return_index));

        anen.handleError(anen.computeSimilarity(search_forecasts, sds, sims,
                search_observations, mapping,
                i_search_stations, observation_parameter));

    } else {
        anen.handleError(anen.computeSimilarity(search_forecasts, sds, sims,
                search_observations, mapping, observation_parameter));
    }

    // Select analogs
    anen.handleError(anen.selectAnalogs(analogs, sims, search_observations,
            mapping, observation_parameter, num_members,
            quick, preserve_real_time));

    /***************************************************************************
     *                           Wrap Up Results                               *
     **************************************************************************/
    List ret;

    if (preserve_similarity) {
        if (verbose >= 3) cout << "Wraping C++ object similarity matrix ..." << endl;
        IntegerVector R_sims_dims = {
            static_cast<int> (sims.shape()[4]),
            static_cast<int> (sims.shape()[3]),
            static_cast<int> (sims.shape()[2]),
            static_cast<int> (sims.shape()[1]),
            static_cast<int> (sims.shape()[0])
        };
        NumericVector R_sims(sims.data(), sims.data() + sims.num_elements());
        R_sims.attr("dim") = R_sims_dims;
        ret["similarity"] = R_sims;
    }

    if (preserve_mapping) {
        if (verbose >= 3) cout << "Wraping C++ object mapping ..." << endl;
        IntegerVector R_mapping_dims{
            static_cast<int> (mapping.size2()),
            static_cast<int> (mapping.size1())};
        NumericVector R_mapping(mapping.data().begin(),
                mapping.data().end());
        R_mapping.attr("dim") = R_mapping_dims;
        ret["mapping"] = R_mapping;
    }

    if (preserve_search_stations) {
        if (verbose >= 3) cout << "Wraping C++ object search stations ..." << endl;
        IntegerVector R_search_stations_dims{
            static_cast<int> (i_search_stations.size2()),
            static_cast<int> (i_search_stations.size1())};
        NumericVector R_search_stations(i_search_stations.data().begin(),
                i_search_stations.data().end());
        R_search_stations.attr("dim") = R_search_stations_dims;
        ret["searchStations"] = R_search_stations;
    }

    if (verbose >= 3) cout << "Wraping C++ object analogs ..." << endl;
    IntegerVector R_analogs_dims = {
        static_cast<int> (analogs.shape()[0]),
        static_cast<int> (analogs.shape()[1]),
        static_cast<int> (analogs.shape()[2]),
        static_cast<int> (analogs.shape()[3]),
        static_cast<int> (analogs.shape()[4])
    };
    NumericVector R_analogs(analogs.data(),
            analogs.data() + analogs.num_elements());
    R_analogs.attr("dim") = R_analogs_dims;
    ret["analogs"] = R_analogs;

    ret.attr("class") = "AnEn";
    return (ret);
}
