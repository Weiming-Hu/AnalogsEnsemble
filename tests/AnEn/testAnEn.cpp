/*
 * File:   testAnEn.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on Aug 4, 2018, 4:09:20 PM
 */

#include "AnEnIS.h"
#include "testAnEn.h"
#include "ForecastsArray.h"
#include "ObservationsArray.h"
#include "boost/assign/list_of.hpp"
#include "boost/assign/list_inserter.hpp"

#if defined(_OPENMP)
#include <omp.h>
#endif

using namespace std;
using namespace boost;
using namespace boost::bimaps;

CPPUNIT_TEST_SUITE_REGISTRATION(testAnEn);

testAnEn::testAnEn() {
}

testAnEn::~testAnEn() {
}

//void testAnEn::testCompute() {
//
//    /**
//     * Test the function compute().
//     * 
//     * The data contains no NAN values. This test is designed for correctness.
//     */
//
//    // tick is created for assigning values
//    double tick;
//    
//    // Create dimensions
//    Stations stations;
//    Parameters parameters;
//    Times fcst_times, obs_times, times_test, times_search, flts;
//    
//    assign::push_back(stations.left)(0, Station());
//    assign::push_back(parameters.left)(0, Parameter());
//    assign::push_back(flts.left)(0, Time(1))(1, Time(2))(2, Time(3))(3, Time(4));
//    assign::push_back(times_test.left)(7, Time(800))(8, Time(900));
//    assign::push_back(times_search.left)(0, Time(100))(1, Time(200))(2, Time(300))
//            (3, Time(400))(4, Time(500))(5, Time(600))(6, Time(700));
//    fcst_times.insert(fcst_times.end(), times_search.begin(), times_search.end());
//    fcst_times.insert(fcst_times.end(), times_test.begin(), times_test.end());
//    
//    vector<size_t> times = {
//        101, 102, 103, 104, 201, 202, 203, 204, 301, 302, 303, 304,
//        401, 402, 403, 404, 501, 502, 503, 504, 601, 602, 603, 604,
//        701, 702, 703, 704
//    };
//    
//    size_t index = 0;
//    for (auto e : times) obs_times.push_back(Times::value_type(index++, Time(e)));
//    
//    // Create forecasts
//    ForecastsArray forecasts_test(parameters, stations, times_test, flts);
//    double *ptr1 = forecasts_test.getValuesPtr();
//    tick = 0.0;
//    for (size_t pos = 0; pos < forecasts_test.num_elements(); ++pos) {
//        ptr1[pos] = tick * 10;
//        ++tick;
//    }
//
//    ForecastsArray forecasts_search(parameters, stations, times_search, flts);
//    tick = 200.0;
//    double *ptr2 = forecasts_search.getValuesPtr();
//    for (size_t pos = 0; pos < forecasts_search.num_elements(); ++pos) {
//        ptr2[pos] = tick / 10;
//        tick -= 2;
//    }
//    
//    ForecastsArray forecasts(parameters, stations, fcst_times, flts);
//    double *ptr3 = forecasts.getValuesPtr();
//    
//    memcpy(ptr3, ptr2, forecasts_search.num_elements() * sizeof(double));
//    memcpy(ptr3 + forecasts_search.num_elements(), ptr1,
//            forecasts_test.num_elements() * sizeof(double));
//    cout << forecasts;
//    
//    // Create observations
//    tick = 20.0;
//    ObservationsArray observations(parameters, stations, obs_times);
//    double *ptr = observations.getValuesPtr();
//    
//    for (size_t pos = 0; pos < observations.num_elements(); ++pos) {
//        ptr[pos] = tick / 2;
//        tick += 2;
//    }
//    cout << observations;
//    
//    // Create analog calculation device
//    bool save_dims = true;
//    AnEnIS anen(7, false, true, save_dims, AnEnDefaults::Verbose::Debug, 0, false);
//    anen.compute(forecasts, observations, {7,8}, {0,1,2,3,4,5,6});
//
//    const auto & sims = anen.getSimsValue();
//    const auto & analogs = anen.getAnalogsValue();
//    
//    Functions::print(cout, sims);
//    Functions::print(cout, analogs);
//}

//    
//    Times times;
//    for (size_t i = 1; i <= 9; ++i) {
//        times.push_back(Times::value_type(i - 1, Time(i * 100)));
//    }
//    
//    Times flts;
//    assign::push_back(flts.left)(0, Time(1))(1, Time(2))(2, Time(3))(3, Time(4));
//
//    vector<double> values;
//
//    // Construct forecasts
//    ForecastsArray forecasts(parameters, stations, times, flts);
//    double *ptr = forecasts.getValuesPtr();
//    size_t num_values = parameters.size() * stations.size() *
//            times.size() * flts.size();
//    tick = 0.0;
//    for (size_t i = 0; i < num_values; ++i) {
//        ptr[i] = tick * 10;
//        ++tick;
//    }
//
//    // Construct search forecasts
//    Stations search_stations;
//    search_stations.insert(search_stations.end(),{s1});
//    Times search_times;
//    search_times.insert(search_times.end(),{
//        100, 200, 300, 400, 500, 600, 700
//    });
//    values.resize(parameters.size() * search_stations.size() *
//            search_times.size() * flts.size());
//    tick = 200.0;
//    for (auto & value : values) {
//        value = tick / 10;
//        tick -= 2;
//    }
//    Forecasts_array search_forecasts(parameters,
//            search_stations, search_times, flts, values);
//
//    // Construct search observations
//    Times search_observation_times;
//    search_observation_times.insert(search_observation_times.end(),{
//        101, 102, 103, 104, 201, 202, 203, 204, 301, 302, 303, 304,
//        401, 402, 403, 404, 501, 502, 503, 504, 601, 602, 603, 604,
//        701, 702, 703, 704
//    });
//    tick = 20.0;
//    for (auto & value : values) {
//        value = tick / 2;
//        tick += 2;
//    }
//    values.resize(parameters.size() * search_stations.size() *
//            search_observation_times.size());
//    Observations_array search_observations(parameters,
//            search_stations, search_observation_times, values);
//
//    // Construct AnEn object
//    AnEn anen(2);
//    Functions functions(2);
//    simMethod::OneToOne);
//
//    // Construct SimilarityMatrices
//    SimilarityMatrices sims(test_forecasts);
//    
//    // Construct standard deviation
//    StandardDeviation sds(parameters.size(),
//            search_stations.size(), flts.size());
//    functions.computeStandardDeviation(search_forecasts, sds);
//    
//    // Pre compute the time mapping from forecasts to observations
//    Functions::TimeMapMatrix mapping;
//    handleError(functions.computeObservationsTimeIndices(
//            search_forecasts.getTimes(), search_forecasts.getFLTs(),
//            search_observations.getTimes(), mapping));
//    
//    // Compute search stations for each test station
//    AnEn::SearchStationMatrix i_search_stations;
//
//    handleError(anen.computeSearchStations(
//            test_forecasts.getStations(), search_forecasts.getStations(),
//            i_search_stations));
//    
//    // Compute similarity
//    anen.computeSimilarity(test_forecasts, search_forecasts, sds, sims,
//            search_observations, mapping, i_search_stations);
//    
//    vector<double> results{
//        32.81278, 32.51099, 32.21302, 31.91898, 31.62899, 31.34315, 31.06157,
//        40.57181, 40.61140, 40.65622, 40.70626, 40.76150, 40.82191, 40.88748,
//        66.48630, 66.85432, 67.22351, 67.59385, 67.96533, 68.33792, 68.71161,
//        81.39652, 81.83760, 82.27892, 82.72048, 83.16228, 83.60430, 84.04654,
//        24.81863, 24.85314, 24.89621, 24.94780, 25.00786, 25.07631, 25.15310,
//        48.28857, 48.54306, 48.80061, 49.06119, 49.32473, 49.59119, 49.86052,
//        86.01744, 86.42627, 86.83564, 87.24555, 87.65598, 88.06694, 88.47841,
//        103.6841, 104.1337, 104.5834, 105.0332, 105.4831, 105.9331, 106.3832};
//
//    size_t index = 0;
//    for (size_t i_station = 0; i_station < sims.shape()[0]; i_station++) {
//        for (size_t i_time = 0; i_time < sims.shape()[1]; i_time++) {
//            for (size_t i_flt = 0; i_flt < sims.shape()[2]; i_flt++) {
//                for (size_t i_search_time = 0;
//                        i_search_time < search_times.size();
//                        i_search_time++, index++) {
//                    CPPUNIT_ASSERT((int) (sims[i_station]
//                            [i_time][i_flt][i_search_time]
//                            [SimilarityMatrices::COL_TAG::VALUE] * 100)
//                            == (int) (results[index] * 100));
//                }
//            }
//        }
//    }
//}
//
//void testAnEn::testOperationalSearch() {
//    
//    /**
//     * Test the operational search functionality by using the normal search
//     * multiple times.
//     */
//    
//    // Construct data sets
//    Parameters parameters;
//    for (int i = 0; i < 3; i++) {
//        Parameter parameter;
//        parameters.push_back(parameter);
//    }
//    
//    Stations stations;
//    for (int i = 0; i < 3; i++) {
//        Station station((double)i, (double)i);
//        stations.push_back(station);
//    }
//    
//    Times times;
//    for (int i = 0; i < 8; i++) {
//        times.push_back(i * 10000);
//    }
//    
//    FLTs flts;
//    for (int i = 0; i < 2; i++) {
//        flts.push_back(i);
//    }
//    
//    Times obs_times;
//    for (const auto & time : times) {
//        for (const auto & flt : flts) {
//            obs_times.push_back(time + flt);
//        }
//    }
//    
//    Times test_times, search_times;
//    for (int i = 5; i < 8; i++) {
//        test_times.push_back(i * 10000);
//    }
//    
//    vector<double> values;
//
//    values.resize(parameters.size() * stations.size() * times.size() * flts.size());
//    for (auto & value : values) value = rand() % 10000;
//    Forecasts_array forecasts(parameters, stations, times, flts, values);
//    
//    values.resize(parameters.size() * stations.size() * obs_times.size());
//    for (auto & value : values) value = rand() % 100000;
//    Observations_array obs(parameters, stations, obs_times, values);
//    
//    // Case 1: Operational search
//    AnEn anen(5);
//    Functions functions(5);
//    SimilarityMatrices sims1;
//    AnEn::TimeMapMatrix mapping;
//    AnEn::SearchStationMatrix i_search_stations;
//    StandardDeviation sds(parameters.size(), stations.size(), flts.size());
//
//    functions.computeObservationsTimeIndices(
//            forecasts.getTimes(), forecasts.getFLTs(), obs.getTimes(), mapping, 0);
//    anen.computeSearchStations(
//            forecasts.getStations(), forecasts.getStations(), i_search_stations);
//    anen.computeSimilarity(forecasts, forecasts, sds, sims1, obs, mapping,
//            i_search_stations, 0, false, 0, 0, test_times, search_times, true);
//    
//    // Case 2: Manually compute similarity for the last test time
//    test_times.clear();
//    test_times.push_back(7 * 10000);
//    search_times.clear();
//    for (int i = 0; i < 7; i++) {
//        search_times.push_back(i * 10000);
//    }
//    
//    SimilarityMatrices sims2, sims3;
//    vector<size_t> i_search_times;
//    
//    functions.convertToIndex(search_times, forecasts.getTimes(), i_search_times);
//    functions.computeStandardDeviation(forecasts, sds, i_search_times);
//    functions.computeObservationsTimeIndices(
//            forecasts.getTimes(), forecasts.getFLTs(), obs.getTimes(), mapping, 0);
//    anen.computeSearchStations(
//            forecasts.getStations(), forecasts.getStations(), i_search_stations);
//    anen.computeSimilarity(forecasts, forecasts, sds, sims2, obs, mapping,
//            i_search_stations, 0, false, 0, 0, test_times, search_times, false);
//    anen.computeSimilarity(forecasts, forecasts, sds, sims3, obs, mapping,
//            i_search_stations, 0, false, 0, 0, test_times, search_times, false,
//            forecasts.getTimes().size());
//
////    cout << sims1 << endl << sims2;
//    
//    // Results should be the same
//    for (size_t i = 0; i < sims1.shape()[0]; i++) {
//        for (size_t j = 0; j < sims1.shape()[2]; j++) {
//            for (size_t l = 0; l < sims1.shape()[3]; l++) {
//                for (size_t m = 0; m < sims1.shape()[4]; m++) {
//                    CPPUNIT_ASSERT(sims1[i][2][j][l][m] == sims2[i][0][j][l][m]);
//                    CPPUNIT_ASSERT(sims3[i][0][j][l][m] == sims2[i][0][j][l][m]);
//                }
//            }
//        }
//    }
//}
//
//void testAnEn::testOpenMP() {
//    
//    /*
//     * Test whether OpenMP is supported and how many threads are created.
//     */
//    
//#if defined(_OPENMP)
//    cout << GREEN << "OpenMP is supported." << RESET << endl;
//    
//    int num_threads;
//
//#pragma omp parallel for default(none) schedule(static) shared(num_threads)
//    for (size_t i = 0; i < 100; i++) {
//        double tmp = 42 * 3.14;
//        if (0 == omp_get_thread_num()) num_threads = omp_get_num_threads();
//        tmp *= 2;
//    }
//    
//    cout << "There are " << GREEN;
//    if (num_threads < 50) cout << num_threads;
//    else cout << "> 50";
//    cout << RESET << " threads created.";
//    
//#else
//    cout << RED << "Warning: OpenMP is not supported." << RESET << endl;
//#endif
//
//}
//
//void testAnEn::testAutomaticDeleteOverlappingTimes() {
//
//    /*
//     * Test whether the overlapping times will be automatically removed
//     * during computation of similarity
//     */
//    
//    // Construct data sets
//    Parameters parameters;
//    for (int i = 0; i < 3; i++) {
//        Parameter parameter;
//        parameters.push_back(parameter);
//    }
//
//    Stations stations;
//    for (int i = 0; i < 3; i++) {
//        Station station((double) i, (double) i);
//        stations.push_back(station);
//    }
//
//    Times times;
//    for (int i = 0; i < 8; i++) {
//        times.push_back(i * 10);
//    }
//
//    FLTs flts;
//    for (int i = 0; i < 5; i++) {
//        flts.push_back(i * 3);
//    }
//
//    Times obs_times;
//    for (const auto & time : times) {
//        for (const auto & flt : flts) {
//            obs_times.push_back(time + flt);
//        }
//    }
//
//    Times test_times, search_times1, search_times2;
//    test_times.push_back(70);
//    
//    for (int i = 0; i < 7; i++) {
//        search_times1.push_back(i * 10);
//    }
//    
//    for (int i = 0; i < 6; i++) {
//        search_times2.push_back(i * 10);
//    }
//    
//    vector<double> values;
//
//    values.resize(parameters.size() * stations.size() * times.size() * flts.size());
//    for (auto & value : values) value = rand() % 10000;
//    Forecasts_array forecasts(parameters, stations, times, flts, values);
//
//    values.resize(parameters.size() * stations.size() * obs_times.size());
//    for (auto & value : values) value = rand() % 100000;
//    Observations_array obs(parameters, stations, obs_times, values);
//
//    // Case 1: let the program figure out what to delete
//    AnEn anen(5);
//    Functions functions(5);
//    SimilarityMatrices sims1;
//    AnEn::TimeMapMatrix mapping;
//    AnEn::SearchStationMatrix i_search_stations;
//    StandardDeviation sds(parameters.size(), stations.size(), flts.size());
//    vector<size_t> i_search_times;
//
//    functions.convertToIndex(search_times1, forecasts.getTimes(), i_search_times);
//    functions.computeStandardDeviation(forecasts, sds, i_search_times);
//    functions.computeObservationsTimeIndices(
//            forecasts.getTimes(), forecasts.getFLTs(), obs.getTimes(), mapping, 0);
//    anen.computeSearchStations(
//            forecasts.getStations(), forecasts.getStations(), i_search_stations);
//    anen.computeSimilarity(forecasts, forecasts, sds, sims1, obs, mapping,
//            i_search_stations, 0, false, 0, 0, test_times, search_times1, false,
//            -1, 1, true);
//
//    // Case 2: manually remove the overlapping time
//    SimilarityMatrices sims2;
//    
//    // Here I'm using the sds from search times 1 because the program would not
//    // know about the overlapping when computing sds.
//    functions.convertToIndex(search_times1, forecasts.getTimes(), i_search_times);
//    functions.computeStandardDeviation(forecasts, sds, i_search_times);
//    functions.computeObservationsTimeIndices(
//            forecasts.getTimes(), forecasts.getFLTs(), obs.getTimes(), mapping, 0);
//    anen.computeSearchStations(
//            forecasts.getStations(), forecasts.getStations(), i_search_stations);
//    anen.computeSimilarity(forecasts, forecasts, sds, sims2, obs, mapping,
//            i_search_stations, 0, false, 0, 0, test_times, search_times2, false);
//    
////        cout << sims1 << endl << sims2;
//
//    // Results should be the same
//    for (size_t i = 0; i < sims1.shape()[0]; i++) {
//        for (size_t j = 0; j < sims1.shape()[1]; j++) {
//            for (size_t l = 0; l < sims1.shape()[2]; l++) {
//                for (size_t m = 0; m < 6; m++) {
//                    for (size_t n = 0; n < sims1.shape()[4]; n++) {
//                        CPPUNIT_ASSERT(sims1[i][j][l][m][n] == sims2[i][j][l][m][n]);
//                    }
//                }
//
//                CPPUNIT_ASSERT(std::isnan(sims1[i][j][l][6][0]));
//                CPPUNIT_ASSERT(std::isnan(sims1[i][j][l][6][1]));
//                CPPUNIT_ASSERT(sims1[i][j][l][6][2] == -2);
//            }
//        }
//    }
//}
//
//void testAnEn::testLeaveOneOut() {
//    
//    /*
//     * Test the leave one out similarity computation
//     */
//    
//    // Construct data sets
//    Parameters parameters;
//    for (int i = 0; i < 3; i++) {
//        Parameter parameter;
//        parameters.push_back(parameter);
//    }
//
//    Stations stations;
//    for (int i = 0; i < 3; i++) {
//        Station station((double) i, (double) i);
//        stations.push_back(station);
//    }
//
//    Times times;
//    for (int i = 0; i < 8; i++) {
//        times.push_back(i * 10);
//    }
//
//    FLTs flts;
//    for (int i = 0; i < 5; i++) {
//        flts.push_back(i * 3);
//    }
//
//    Times obs_times;
//    for (const auto & time : times) {
//        for (const auto & flt : flts) {
//            obs_times.push_back(time + flt);
//        }
//    }
//
//    Times test_times, search_times;
//    test_times.push_back(50);
//
//    for (int i = 0; i < 4; i++) {
//        search_times.push_back(i * 10);
//    }
//
//    search_times.push_back(70);
//
//    vector<double> values;
//
//    values.resize(parameters.size() * stations.size() * times.size() * flts.size());
//    for (auto & value : values) value = rand() % 10000;
//    Forecasts_array forecasts(parameters, stations, times, flts, values);
//
//    values.resize(parameters.size() * stations.size() * obs_times.size());
//    for (auto & value : values) value = rand() % 100000;
//    Observations_array obs(parameters, stations, obs_times, values);
//
//    // Case 1: let the program figure out leave-one-out computation
//    AnEn anen(5);
//    Functions functions(5);
//    SimilarityMatrices sims1;
//    AnEn::TimeMapMatrix mapping;
//    AnEn::SearchStationMatrix i_search_stations;
//    StandardDeviation sds(parameters.size(), stations.size(), flts.size());
//
//    functions.computeStandardDeviation(forecasts, sds);
//    functions.computeObservationsTimeIndices(
//            forecasts.getTimes(), forecasts.getFLTs(), obs.getTimes(), mapping, 0);
//    anen.computeSearchStations(
//            forecasts.getStations(), forecasts.getStations(), i_search_stations);
//    anen.computeSimilarity(forecasts, forecasts, sds, sims1, obs, mapping,
//            i_search_stations, 0, false, 0, 0, test_times);
//
//    // Case 2: manually configure leave-one-out configuration
//    SimilarityMatrices sims2;
//
//    // Here I'm using the sds from search times 1 because the program would not
//    // know about the overlapping when computing sds.
//
//    functions.computeObservationsTimeIndices(
//            forecasts.getTimes(), forecasts.getFLTs(), obs.getTimes(), mapping, 0);
//    anen.computeSearchStations(
//            forecasts.getStations(), forecasts.getStations(), i_search_stations);
//    anen.computeSimilarity(forecasts, forecasts, sds, sims2, obs, mapping,
//            i_search_stations, 0, false, 0, 0, test_times, search_times);
//
////    cout << sims1 << endl << sims2;
//
//    // Results should be the same
//    for (size_t i = 0; i < sims2.shape()[0]; i++) {
//        for (size_t j = 0; j < sims2.shape()[1]; j++) {
//            for (size_t l = 0; l < sims2.shape()[2]; l++) {
//                for (size_t m = 0; m < 4; m++) {
//                    for (size_t n = 0; n < sims2.shape()[4]; n++) {
//                        CPPUNIT_ASSERT(sims1[i][j][l][m][n] == sims2[i][j][l][m][n]);
//                    }
//                }
//                
//                for (size_t n = 0; n < sims2.shape()[4]; n++) {
//                    CPPUNIT_ASSERT(sims1[i][j][l][7][n] == sims2[i][j][l][4][n]);
//                }
//            }
//        }
//    }
//}
