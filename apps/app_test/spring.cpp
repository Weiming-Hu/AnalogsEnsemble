/* 
 * File:   analogGenerator.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *         Guido Cervone <cervone@psu.edu>
 *
 * Created on April 17, 2018, 11:10 PM
 */

/** @file */


#include "AnEnIS.h"
#include "AnEnReadNcdf.h"
#include "ForecastsArray.h"
#include "ObservationsArray.h"

#include <iterator>
#include <boost/multi_array/base.hpp>

using namespace std;
typedef Array4D::index_range range;

int main(int argc, char** argv) {

    string forecast_file = "forecasts_211.nc";
    string observation_file = "observations_211.nc";

    ForecastsArray forecasts;
    ObservationsArray observations;

    AnEnReadNcdf read_nc;
    read_nc.readForecasts(forecast_file, forecasts);
    read_nc.readObservations(observation_file, observations);
    
//    for (size_t i = 0; i < forecasts.getTimes().size(); ++i) {
//        cout << i <<",\t";
//        for (size_t j = 0; j < forecasts.getFLTs().size(); ++j) {
//            size_t fct_time = forecasts.getTimeStamp(i);
//            size_t flt_time = forecasts.getFltTimeStamp(j);
//            
//            size_t obs_time = fct_time + flt_time;
//            cout << observations.getValue(0,0,Time(obs_time)) << ",\t";
//            
//        }
//     cout << endl; 
//    }
    
    
    size_t num_search_fcsts = 51;
    size_t search_start = 0;
    size_t num_test_fcsts = 2;
    size_t test_start = 55;
    
    vector<size_t> fcsts_test_index(num_test_fcsts);
    vector<size_t> fcsts_search_index(num_search_fcsts);
    
    iota(fcsts_search_index.begin(), fcsts_search_index.end(), search_start);
    iota(fcsts_test_index.begin(), fcsts_test_index.end(), test_start);
    
    cout << "Test indices: " << Functions::format(fcsts_test_index);
    cout << endl << "Search indices: " << Functions::format(fcsts_search_index);
    cout << endl;

    size_t num_members = 5;
    bool operational = false;
    bool check_time_overlap = true;
    bool save_sims = true;
    size_t obs_id = 0;
    bool quick_sort = false;
    bool save_sims_index = true;
    bool save_analogs_index = false;
    size_t num_sims = 51;
    
    AnEnIS anen(num_members, operational, check_time_overlap, save_sims,
            AnEnDefaults::Verbose::Debug, obs_id, quick_sort, save_sims_index,
            save_analogs_index, num_sims);

    anen.compute(forecasts, observations, fcsts_test_index, fcsts_search_index);
    
    Array4D analogs = anen.getAnalogsValue(),
            sims = anen.getSimsValue(),
            sims_index = anen.getSimsIndex();
    
    Functions::print(cout, sims);
    
//    Array4D::array_view<2>::type analogs_view = analogs[
//            boost::indices[0][0][range()][range()] ];
//    Array4D::array_view<2>::type sims_view = sims[
//            boost::indices[0][0][range()][range()] ];
//    Array4D::array_view<2>::type sims_index_view = sims_index[
//            boost::indices[0][0][range()][range()] ];
//    
//    cout.precision(5);
//    cout << "Analogs: " << endl;
//    Functions::print(cout, analogs_view);
//    cout << "Similarity: " << endl;
//    Functions::print(cout, sims_view);
//    cout << "Similarity Index: " << endl;
//    Functions::print(cout, sims_index_view);
    
    return 0;
}
