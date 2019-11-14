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
#include <exception>

#include "AnEn.h"
#include "colorTexts.h"

using namespace Rcpp;

#if defined(_OPENMP)
#include <omp.h>
#endif

// Rcpp::Rcout is the wrapper for std::cout in Rcpp
#define cout Rcpp::Rcout
#define cerr Rcpp::Rcerr
#define endl std::endl

// [[Rcpp::export(".checkOpenMP")]]

bool checkOpenMP() {
#ifndef _OPENMP
    return false;
#else
    return true;
#endif
}

// [[Rcpp::export(".generateSearchStations")]]

NumericVector generateSearchStations(
        NumericVector R_test_forecasts_station_x,
        NumericVector R_test_forecasts_station_y,
        NumericVector R_search_forecasts_station_x,
        NumericVector R_search_forecasts_station_y,
        size_t max_num_search_stations, double distance,
        size_t num_nearest_stations, 
        NumericVector R_test_station_tags,
        NumericVector R_search_station_tags,
        int verbose) {

    // Converting R data types to C++ data types
    if (verbose >= 3)
        cout << "Convert R objects to C++ objects ..." << endl;
    
    anenSta::Stations test_stations;
    for (size_t i_station = 0; i_station < R_test_forecasts_station_x.size(); i_station++) {
        anenSta::Station s(R_test_forecasts_station_x[i_station], R_test_forecasts_station_y[i_station]);
        test_stations.push_back(s);
    }

    anenSta::Stations search_stations;
    for (size_t i_station = 0; i_station < R_search_forecasts_station_x.size(); i_station++) {
        anenSta::Station s(R_search_forecasts_station_x[i_station], R_search_forecasts_station_y[i_station]);
        search_stations.push_back(s);
    }
    
    std::vector<size_t> test_station_tags(0), search_station_tags(0);
    test_station_tags.insert(test_station_tags.end(),
            R_test_station_tags.begin(), R_test_station_tags.end());
    search_station_tags.insert(search_station_tags.end(),
            R_search_station_tags.begin(), R_search_station_tags.end());
    
    // Compute search stations
    if (verbose >= 3) cout << "Calculating search stations ..." << endl;
    AnEn anen(verbose);
    boost::numeric::ublas::matrix<double> i_search_stations;
    anen.setMethod(AnEn::simMethod::OneToMany);
    
    handleError(anen.computeSearchStations(
            test_stations, search_stations,
            i_search_stations, max_num_search_stations, distance,
            num_nearest_stations, test_station_tags, search_station_tags));
    
    // Convert C++ data types to R data types
    if (verbose >= 3) cout << "Wrapping C++ object search stations ..." << endl;
    IntegerVector R_search_stations_dims{
        static_cast<int> (i_search_stations.size2()),
        static_cast<int> (i_search_stations.size1())};
    NumericVector R_search_stations(i_search_stations.data().begin(),
            i_search_stations.data().end());
    R_search_stations.attr("dim") = R_search_stations_dims;
    
    return (R_search_stations);
}


// [[Rcpp::export(".generateTimeMapping")]]

NumericVector generateTimeMapping(
        NumericVector R_times_forecasts,
        NumericVector R_flts_forecasts,
        NumericVector R_times_observations,
        int time_match_mode, int verbose) {

    /***************************************************************************
     *                   Convert objects for mapping computation               *
     **************************************************************************/

    if (verbose >= 3)
        cout << "Convert R objects to C++ objects ..." << endl;

    anenTime::Times times_forecasts, flts_forecasts, times_observations;
    Functions::TimeMapMatrix mapping;

    times_forecasts.insert(times_forecasts.end(),
            R_times_forecasts.begin(), R_times_forecasts.end());
    flts_forecasts.insert(flts_forecasts.end(),
            R_flts_forecasts.begin(), R_flts_forecasts.end());
    times_observations.insert(times_observations.end(),
            R_times_observations.begin(), R_times_observations.end());

    if (R_times_forecasts.size() != times_forecasts.size()) {
        if (verbose >= 1)
            cout << "Error: Forecast times are not unique. "
                << "Original: " << R_times_forecasts.size() << " Unique: " 
                << times_forecasts.size() << endl;
        throw std::runtime_error("Elements not unique.");
    }
    if (R_flts_forecasts.size() != flts_forecasts.size()) {
        if (verbose >= 1)
            cout << "Error: Forecast FLTs are not unique. "
                << "Original: " << R_flts_forecasts.size() << " Unique: " 
                << flts_forecasts.size() << endl;
        throw std::runtime_error("Elements not unique.");
    }
    if (R_times_observations.size() != times_observations.size()) {
        if (verbose >= 1)
            cout << "Error: Observation times are not unique. "
                << "Original: " << R_times_observations.size() << " Unique: " 
                << times_observations.size() << endl;
        throw std::runtime_error("Elements not unique.");
    }

    /***************************************************************************
     *                        Mapping Computation                              *
     **************************************************************************/
    if (verbose >= 3)
        cout << "Compute mapping from forecast times/flts to observation times ..." << endl;

    Functions functions(verbose);

    handleError(functions.computeObservationsTimeIndices(
            times_forecasts, flts_forecasts, times_observations,
            mapping, time_match_mode));

    /***************************************************************************
     *                           Wrap Up Results                               *
     **************************************************************************/
    if (verbose >= 3) cout << "Wrapping C++ object mapping ..." << endl;
    IntegerVector R_mapping_dims{
        static_cast<int> (mapping.size2()),
        static_cast<int> (mapping.size1())
    };

    NumericVector R_mapping(mapping.data().begin(), mapping.data().end());
    R_mapping.attr("dim") = R_mapping_dims;

    return (R_mapping);
}

// [[Rcpp::export(".generateAnalogs")]]

List generateAnalogs(
        NumericVector R_test_forecasts,
        NumericVector R_test_forecasts_dims,
        NumericVector R_test_forecasts_station_x,
        NumericVector R_test_forecasts_station_y,
        NumericVector R_test_times,
        NumericVector R_search_forecasts,
        NumericVector R_search_forecasts_dims,
        NumericVector R_search_forecasts_station_x,
        NumericVector R_search_forecasts_station_y,
        NumericVector R_search_times,
        NumericVector R_flts,
        NumericVector R_search_observations,
        NumericVector R_search_observations_dims,
        NumericVector R_observation_times,
        NumericVector R_weights,
        size_t num_members, size_t observation_id, bool quick,
        IntegerVector R_circulars,
        bool search_extension, bool extend_observations,
        bool preserve_similarity, bool preserve_mapping,
        bool preserve_search_stations, bool preserve_std,
        size_t max_num_search_stations, double distance,
        size_t num_nearest_stations, int time_match_mode,
        double max_par_nan, double max_flt_nan,
        NumericVector R_test_times_compare,
        NumericVector R_search_times_compare,
        bool operational, int max_sims_entries, int FLT_radius,
        NumericVector R_test_station_tags,
        NumericVector R_search_station_tags,
        int verbose,
        bool debug) {

    /***************************************************************************
     *                   Convert objects for AnEn computation                  *
     **************************************************************************/
    if (verbose >= 3)
        cout << "Convert R objects to C++ objects ..." << endl;

    std::vector<size_t> test_station_tags(0), search_station_tags(0);
    test_station_tags.insert(test_station_tags.end(),
            R_test_station_tags.begin(), R_test_station_tags.end());
    search_station_tags.insert(search_station_tags.end(),
            R_search_station_tags.begin(), R_search_station_tags.end());
    
    anenTime::Times test_times;
    test_times.insert(test_times.end(),
            R_test_times.begin(), R_test_times.end());

    anenTime::Times search_times;
    search_times.insert(search_times.end(),
            R_search_times.begin(), R_search_times.end());

    anenTime::FLTs flts;
    flts.insert(flts.end(), R_flts.begin(), R_flts.end());

    anenTime::Times observation_times;
    observation_times.insert(observation_times.end(),
            R_observation_times.begin(), R_observation_times.end());

    std::vector<anenPar::Parameter> parameters_vec(R_test_forecasts_dims[0]);
    for (auto & pos : R_circulars) {
        parameters_vec[pos - 1].setCircular(true);
    }
    for (size_t i = 0; i < R_weights.size(); i++) {
        parameters_vec[i].setWeight(R_weights[i]);
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

    anenSta::Stations search_stations;
    if (search_extension) {
        for (size_t i_station = 0; i_station < R_search_forecasts_dims[1]; i_station++) {
            anenSta::Station s(R_search_forecasts_station_x[i_station],
                    R_search_forecasts_station_y[i_station]);
            search_stations.push_back(s);
        }
    } else {
        search_stations.get<anenSta::by_insert>().resize(R_search_forecasts_dims[1]);
    }

    // There is an important assumption for observation stations that if the numbers
    // of observation stations and the search stations are the same, the stations are
    // the same. If they are not the same, since I haven't implemented passing
    // coordinates for observation stations, the C++ library will report an error due to
    // missing information for observation stations. 
    //
    anenSta::Stations observation_stations;
    if (R_search_observations_dims[1] == R_search_forecasts_dims[1]) {
        observation_stations = search_stations;
    } else {
        observation_stations.resize(R_search_observations_dims[1]);
    }

    anenPar::Parameters observation_ids;
    observation_ids.get<anenPar::by_insert>().resize(
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
            observation_ids, observation_stations, observation_times);
    search_observations.data().assign(
            R_search_observations.begin(), R_search_observations.end());

    anenTime::Times test_times_compare, search_times_compare;
    test_times_compare.insert(test_times_compare.end(), 
            R_test_times_compare.begin(), R_test_times_compare.end());
    search_times_compare.insert(search_times_compare.end(), 
            R_search_times_compare.begin(), R_search_times_compare.end());

    /***************************************************************************
     *                           AnEn Computation                              *
     **************************************************************************/
    AnEn anen(verbose);
    Functions functions(verbose);

    if (max_par_nan < 0) max_par_nan = NAN;
    if (max_flt_nan < 0) max_flt_nan = NAN;
    
    Analogs analogs;
    SimilarityMatrices sims(test_forecasts);
    AnEn::TimeMapMatrix mapping;
    StandardDeviation sds(parameters.size(),
            search_stations.size(), flts.size());

    if (verbose >= 4) {
        cout << "A summary of test forecast parameters:" << endl
                << test_forecasts.getParameters();
    }

    if (search_extension) anen.setMethod(AnEn::simMethod::OneToMany);
    else anen.setMethod(AnEn::simMethod::OneToOne);

    // Pre compute the standard deviation
    std::vector<size_t> search_times_index;
    functions.convertToIndex(search_times_compare, search_times, search_times_index);

    if (!operational) {
        handleError(functions.computeStandardDeviation(search_forecasts, sds, search_times_index));
    }

    // Pre compute the time mapping from forecasts [Times, FLTs] 
    // to observations [Times]
    //
    handleError(functions.computeObservationsTimeIndices(
            search_forecasts.getTimes(), search_forecasts.getFLTs(),
            search_observations.getTimes(), mapping, time_match_mode));

    // Compute similarity
    boost::numeric::ublas::matrix<double> i_search_stations;
    
    handleError(anen.computeSearchStations(
            test_forecasts.getStations(), search_forecasts.getStations(),
            i_search_stations, max_num_search_stations, distance,
            num_nearest_stations, test_station_tags, search_station_tags));

    handleError(anen.computeSimilarity(test_forecasts, search_forecasts,
            sds, sims, search_observations, mapping, i_search_stations,
            observation_id, extend_observations,
            max_par_nan, max_flt_nan, test_times_compare,
            search_times_compare, operational, max_sims_entries, FLT_radius, debug));

    // Select analogs
    handleError(anen.selectAnalogs(analogs, sims, 
            test_stations, search_observations,
            mapping, observation_id, num_members, quick,
            extend_observations, debug));

    /***************************************************************************
     *                           Wrap Up Results                               *
     **************************************************************************/
    List ret;

    if (preserve_similarity) {
        if (verbose >= 3) cout << "Wrapping C++ object similarity matrix ..." << endl;
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
        if (verbose >= 3) cout << "Wrapping C++ object mapping ..." << endl;
        IntegerVector R_mapping_dims{
            static_cast<int> (mapping.size2()),
            static_cast<int> (mapping.size1())};
        NumericVector R_mapping(mapping.data().begin(),
                mapping.data().end());
        R_mapping.attr("dim") = R_mapping_dims;
        ret["mapping"] = R_mapping;
    }

    if (preserve_search_stations) {
        if (verbose >= 3) cout << "Wrapping C++ object search stations ..." << endl;
        IntegerVector R_search_stations_dims{
            static_cast<int> (i_search_stations.size2()),
            static_cast<int> (i_search_stations.size1())};
        NumericVector R_search_stations(i_search_stations.data().begin(),
                i_search_stations.data().end());
        R_search_stations.attr("dim") = R_search_stations_dims;
        ret["searchStations"] = R_search_stations;
    }

    if (preserve_std) {
        if (operational) {
            // If operational search is used, the standard deviation will be constantly updated
            // because of the increase in the search forecasts for each test forecast. But the
            // changes are not reflected in the results. For consistency, std can not be preserve
            // when operational search is used.
            //
            if (verbose >= 2) cout << RED
                << "Warning: Standard deviation cannot be preserved when operational search is used."
                << RESET << endl;

        } else {
            if (verbose >= 3) cout << "Wrapping C++ object standard deviation matrices ..." << endl;
            IntegerVector R_sds_dims{
                static_cast<int> (sds.shape()[0]),
                    static_cast<int> (sds.shape()[1]),
                    static_cast<int> (sds.shape()[2])};
            NumericVector R_sds(sds.data(), sds.data() + sds.num_elements());
            R_sds.attr("dim") = R_sds_dims;
            ret["std"] = R_sds;
        }
    }

    if (verbose >= 3) cout << "Wrapping C++ object analogs ..." << endl;
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
