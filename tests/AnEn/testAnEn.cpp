/*
 * File:   testAnEn.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on Aug 4, 2018, 4:09:20 PM
 */

#include "testAnEn.h"
#include "../../CAnEn/include/colorTexts.h"

#include <numeric>
#include <cppunit/TestAssert.h>
#include <boost/numeric/ublas/io.hpp>

#if defined(_OPENMP)
#include <omp.h>
#endif

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION(testAnEn);

testAnEn::testAnEn() {
}

testAnEn::~testAnEn() {
}

void testAnEn::testComputeSimilarity() {

    /**
     * Test the function computeSimilarity().
     * 
     * The data contains no NAN values. This test is designed for correctness.
     */

    // tick is created for assigning values
    double tick;

    // Construct meta information
    anenSta::Station s1;

    anenPar::Parameter p1;
    anenPar::Parameters parameters;
    parameters.insert(parameters.end(),{p1});

    anenTime::FLTs flts;
    flts.insert(flts.end(),{1, 2, 3, 4});

    vector<double> values;

    // Construct test forecasts
    anenSta::Stations test_stations;
    test_stations.insert(test_stations.end(),{s1});
    anenTime::Times test_times;
    test_times.insert(test_times.end(),{800, 900});
    values.resize(parameters.size() * test_stations.size() *
            test_times.size() * flts.size());
    tick = 0.0;
    for (auto & value : values) {
        value = tick * 10;
        tick++;
    }
    Forecasts_array test_forecasts(parameters,
            test_stations, test_times, flts, values);

    // Construct search forecasts
    anenSta::Stations search_stations;
    search_stations.insert(search_stations.end(),{s1});
    anenTime::Times search_times;
    search_times.insert(search_times.end(),{
        100, 200, 300, 400, 500, 600, 700
    });
    values.resize(parameters.size() * search_stations.size() *
            search_times.size() * flts.size());
    tick = 200.0;
    for (auto & value : values) {
        value = tick / 10;
        tick -= 2;
    }
    Forecasts_array search_forecasts(parameters,
            search_stations, search_times, flts, values);

    // Construct search observations
    anenTime::Times search_observation_times;
    search_observation_times.insert(search_observation_times.end(),{
        101, 102, 103, 104, 201, 202, 203, 204, 301, 302, 303, 304,
        401, 402, 403, 404, 501, 502, 503, 504, 601, 602, 603, 604,
        701, 702, 703, 704
    });
    tick = 20.0;
    for (auto & value : values) {
        value = tick / 2;
        tick += 2;
    }
    values.resize(parameters.size() * search_stations.size() *
            search_observation_times.size());
    Observations_array search_observations(parameters,
            search_stations, search_observation_times, values);

    // Construct AnEn object
    AnEn anen(2);
    Functions functions(2);
    anen.setMethod(AnEn::simMethod::OneToOne);

    // Construct SimilarityMatrices
    SimilarityMatrices sims(test_forecasts);
    
    // Construct standard deviation
    StandardDeviation sds(parameters.size(),
            search_stations.size(), flts.size());
    functions.computeStandardDeviation(search_forecasts, sds);
    
    // Pre compute the time mapping from forecasts to observations
    Functions::TimeMapMatrix mapping;
    handleError(functions.computeObservationsTimeIndices(
            search_forecasts.getTimes(), search_forecasts.getFLTs(),
            search_observations.getTimes(), mapping));
    
    // Compute search stations for each test station
    AnEn::SearchStationMatrix i_search_stations;

    handleError(anen.computeSearchStations(
            test_forecasts.getStations(), search_forecasts.getStations(),
            i_search_stations));
    
    // Compute similarity
    anen.computeSimilarity(test_forecasts, search_forecasts, sds, sims,
            search_observations, mapping, i_search_stations);
    
    vector<double> results{
        32.81278, 32.51099, 32.21302, 31.91898, 31.62899, 31.34315, 31.06157,
        40.57181, 40.61140, 40.65622, 40.70626, 40.76150, 40.82191, 40.88748,
        66.48630, 66.85432, 67.22351, 67.59385, 67.96533, 68.33792, 68.71161,
        81.39652, 81.83760, 82.27892, 82.72048, 83.16228, 83.60430, 84.04654,
        24.81863, 24.85314, 24.89621, 24.94780, 25.00786, 25.07631, 25.15310,
        48.28857, 48.54306, 48.80061, 49.06119, 49.32473, 49.59119, 49.86052,
        86.01744, 86.42627, 86.83564, 87.24555, 87.65598, 88.06694, 88.47841,
        103.6841, 104.1337, 104.5834, 105.0332, 105.4831, 105.9331, 106.3832};

    size_t index = 0;
    for (size_t i_station = 0; i_station < sims.shape()[0]; i_station++) {
        for (size_t i_time = 0; i_time < sims.shape()[1]; i_time++) {
            for (size_t i_flt = 0; i_flt < sims.shape()[2]; i_flt++) {
                for (size_t i_search_time = 0;
                        i_search_time < search_times.size();
                        i_search_time++, index++) {
                    CPPUNIT_ASSERT((int) (sims[i_station]
                            [i_time][i_flt][i_search_time]
                            [SimilarityMatrices::COL_TAG::VALUE] * 100)
                            == (int) (results[index] * 100));
                }
            }
        }
    }
}

void testAnEn::testSelectAnalogs() {

    /**
     * Test the function selectAnalogs().
     * 
     * The data contains no NAN values. This test is designed for correctness.
     */
    // tick is created for assigning values
    double tick;

    // Construct meta information
    anenSta::Station s1;

    anenPar::Parameter p1;
    anenPar::Parameters parameters;
    parameters.insert(parameters.end(),{p1});

    anenTime::FLTs flts;
    flts.insert(flts.end(),{1, 2, 3, 4});

    vector<double> values;
    // Construct test forecasts
    anenSta::Stations test_stations;
    test_stations.insert(test_stations.end(),{s1});
    anenTime::Times test_times;
    test_times.insert(test_times.end(),{800, 900});
    values.resize(parameters.size() * test_stations.size() *
            test_times.size() * flts.size());
    tick = 0.0;
    for (auto & value : values) {
        value = tick * 10;
        tick++;
    }
    Forecasts_array test_forecasts(parameters,
            test_stations, test_times, flts, values);
    // Construct search forecasts
    anenSta::Stations search_stations;
    search_stations.insert(search_stations.end(),{s1});
    anenTime::Times search_times;
    search_times.insert(search_times.end(),{
        100, 200, 300, 400, 500, 600, 700
    });
    values.resize(parameters.size() * search_stations.size() *
            search_times.size() * flts.size());
    tick = 200.0;
    for (auto & value : values) {
        value = tick / 10;
        tick -= 2;
    }
    Forecasts_array search_forecasts(parameters,
            search_stations, search_times, flts, values);
    // Construct search observations
    anenTime::Times search_observation_times;
    search_observation_times.insert(search_observation_times.end(),{
        101, 102, 103, 104, 201, 202, 203, 204, 301, 302, 303, 304,
        401, 402, 403, 404, 501, 502, 503, 504, 601, 602, 603, 604,
        701, 702, 703, 704
    });
    tick = 20.0;
    for (auto & value : values) {
        value = tick / 2;
        tick += 2;
    }
    values.resize(parameters.size() * search_stations.size() *
            search_observation_times.size());
    Observations_array search_observations(parameters,
            search_stations, search_observation_times, values);
    // Construct AnEn object
    AnEn anen(2);
    anen.setMethod(AnEn::simMethod::OneToOne);
    Functions functions(2);

    // Construct SimilarityMatrices
    SimilarityMatrices sims(test_forecasts);

    // Construct standard deviation
    StandardDeviation sds(parameters.size(),
            search_stations.size(), flts.size());
    handleError(functions.computeStandardDeviation(search_forecasts, sds));
    
    // Pre compute the time mapping from forecasts to observations
    Functions::TimeMapMatrix mapping;
    handleError(functions.computeObservationsTimeIndices(
            search_forecasts.getTimes(), search_forecasts.getFLTs(),
            search_observations.getTimes(), mapping));

    // Compute search stations for each test station
    AnEn::SearchStationMatrix i_search_stations;

    handleError(anen.computeSearchStations(
            test_forecasts.getStations(), search_forecasts.getStations(),
            i_search_stations));
    
    // Compute similarity
    handleError(anen.computeSimilarity(
            test_forecasts, search_forecasts, sds, sims,
            search_observations, mapping, i_search_stations));
    
    // Select analogs
    Analogs analogs1, analogs2;
    anen.selectAnalogs(analogs1, sims, test_stations, search_observations, mapping,
            0, // I know there is only one parameter
            4, // I only want 4 members
            false, false);
    anen.selectAnalogs(analogs2, sims, test_stations, search_observations, mapping,
            0, // I know there is only one parameter
            4, // I only want 4 members
            false, true);

    vector<double> results{34, 10, 11, 11, 12, 12, 13, 13, 30,
        14, 15, 15, 16, 16, 17, 17, 26, 18, 19, 19, 20, 20, 21, 21,
        22, 22, 23, 23, 24, 24, 25, 25, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 701, 101, 102, 102, 103, 103, 104, 104, 601, 201,
        202, 202, 203, 203, 204, 204, 501, 301, 302, 302, 303, 303,
        304, 304, 401, 401, 402, 402, 403, 403, 404, 404};

    size_t dim1 = analogs1.shape()[0];
    size_t dim2 = analogs1.shape()[1];
    size_t dim3 = analogs1.shape()[2];
    size_t dim4 = analogs1.shape()[3];
    size_t dim5 = analogs1.shape()[4];
    
    const auto & search_observation_times_by_insert = search_observation_times.get<anenTime::by_insert>();

    size_t i = 0;
    for (size_t i_dim5 = 0; i_dim5 < dim5; i_dim5++) {
        for (size_t i_dim4 = 0; i_dim4 < dim4; i_dim4++) {
            for (size_t i_dim3 = 0; i_dim3 < dim3; i_dim3++) {
                for (size_t i_dim2 = 0; i_dim2 < dim2; i_dim2++) {
                    for (size_t i_dim1 = 0; i_dim1 < dim1; i_dim1++, i++) {
                        if (i_dim5 == Analogs::COL_TAG::TIME) {
                            CPPUNIT_ASSERT(results[i] == 
                                    search_observation_times_by_insert[analogs1[i_dim1][i_dim2][i_dim3][i_dim4][i_dim5]]);
                            CPPUNIT_ASSERT(results[i] == 
                                    search_observation_times_by_insert[analogs2[i_dim1][i_dim2][i_dim3][i_dim4][i_dim5]]);
                        } else {
                            CPPUNIT_ASSERT(results[i] == analogs1[i_dim1][i_dim2][i_dim3][i_dim4][i_dim5]);
                            CPPUNIT_ASSERT(results[i] == analogs2[i_dim1][i_dim2][i_dim3][i_dim4][i_dim5]);
                        }
                    }
                }
            }
        }
    }
}

void testAnEn::testComputeSearchStations() {

    /**
     * Tests function computeSearchStations;
     * 
     * The spatial distribution of stations looks like below. The numbers
     * shown on the diagram are the ID of stations.
     * 
     *           (Y)  ^
     *              8 |             14
     *              7 |       12          13
     *              6 |    11
     *              5 |     8  9 10
     *              4 |        6        7
     *              3 |  3        4           5
     *              2 |
     *              1 |  1              2
     *                .-------------------------->
     *                0  1  2  3  4  5  6  7  8  (X)
     */

    // Create an example forecast object
    anenSta::Station s1("1", 1, 1), s2("2", 6, 1), s3("3", 1, 3),
            s4("4", 4, 3), s5("5", 8, 3), s6("6", 3, 4),
            s7("7", 6, 4), s8("8", 2, 5), s9("9", 3, 5),
            s10("10", 4, 5), s11("11", 2, 6), s12("12", 3, 7),
            s13("13", 7, 7), s14("14", 5, 8);

    anenSta::Stations test_stations, search_stations;
    search_stations.insert(search_stations.end(),{s1, s2, s3, s4, s5, s6,
        s7, s8, s9, s10, s11, s12, s13, s14});
    test_stations.insert(test_stations.end(),{s3, s6, s7});

    AnEn::SearchStationMatrix i_search_stations;
    AnEn anen(2);
    anen.setMethod(AnEn::simMethod::OneToMany);

    vector<size_t> results;

    // Test getting stations based on distance
    double distance = 1.5;
    anen.computeSearchStations(test_stations, search_stations,
            i_search_stations, 5, distance);

    results = {2, 3, 5, 7, 8, 9, 6};
    for (auto row = i_search_stations.begin1();
            row != i_search_stations.end1(); row++) {
        for (const auto & val : row) {
            if (!std::isnan(val)) results.erase(find(results.begin(), results.end(), val));
        }
    }
    CPPUNIT_ASSERT(results.size() == 0);

    // Test getting KNN stations
    size_t num_stations = 4;
    anen.computeSearchStations(test_stations, search_stations,
            i_search_stations, num_stations, 0, num_stations);

    results = {5, 7, 2, 0, 3, 7, 5, 8, 4, 9, 3, 6};
    for (auto row = i_search_stations.begin1();
            row != i_search_stations.end1(); row++) {
        for (const auto & val : row) {
            results.erase(find(results.begin(), results.end(), val));
        }
    }
    CPPUNIT_ASSERT(results.size() == 0);

    // Test getting KNN stations limited by threshold distance
    double threshold = 2;
    anen.computeSearchStations(test_stations, search_stations,
            i_search_stations, num_stations, threshold, num_stations);

    results = {2, 7, 8, 3, 5, 6};
    for (auto row = i_search_stations.begin1();
            row != i_search_stations.end1(); row++) {
        for (const auto & val : row) {
            if (!std::isnan(val)) results.erase(find(results.begin(), results.end(), val));
        }
    }
    CPPUNIT_ASSERT(results.size() == 0);
}

void testAnEn::testOpenMP() {
    
    /*
     * Test whether OpenMP is supported and how many threads are created.
     */
    
#if defined(_OPENMP)
    cout << GREEN << "OpenMP is supported." << RESET << endl;
    
    int num_threads;

#pragma omp parallel for default(none) schedule(static) shared(num_threads)
    for (size_t i = 0; i < 100; i++) {
        double tmp = 42 * 3.14;
        if (0 == omp_get_thread_num()) num_threads = omp_get_num_threads();
        tmp *= 2;
    }
    
    cout << "There are " << GREEN;
    if (num_threads < 50) cout << num_threads;
    else cout << "> 50";
    cout << RESET << " threads created.";
    
#else
    cout << RED << "Warning: OpenMP is not supported." << RESET << endl;
#endif

}
