/* 
 * File:   analogGenerator.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *         Guido Cervone <cervone@psu.edu>
 *
 * Created on April 17, 2018, 11:10 PM
 */

/** @file */

#include "AnEn.h"
#include "AnEnReadNcdf.h"

#include "ForecastsArray.h"
#include "ObservationsArray.h"

using namespace std;

//int main () {
////    bm_type myTimes;
////    Time mytime(1000);
////    
////    myTimes.push_back(bm_type::value_type(0, Time(100)));
////    myTimes.push_back(bm_type::value_type(1, Time(200)));
////    myTimes.push_back(bm_type::value_type(2, Time(300)));
////    myTimes.push_back(bm_type::value_type(3, Time(400)));
////    
////    
////    cout << myTimes.left[0].first << " Value: " << myTimes.left.at(0).second << endl;
////    cout << myTimes.left.at(1).first << " Value: " << myTimes.left.at(1).second << endl;
////    cout << myTimes.left.at(2).first << " Value: " << myTimes.left.at(2).second << endl;
////    
////    cout << myTimes.right.find(Time(200))->second << endl;
//    
//    
//    Times times;
//    
//    cout << "Original size of times: " << times.size() << endl;
//    
//    times.resize(10);
//    cout << "Size of times after resize(): " << times.size() << endl;
//    cout << "Capacity of times after resize(): " << times.capacity() << endl;
//    
//    cout << times;
//    
//    return 0;
//}

int main(int argc, char** argv) {

//    string forecast_file = "/home/graduate/wuh20/github/AnalogsEnsemble/tests/Data/forecasts.nc";
//    string observation_file = "/home/graduate/wuh20/github/AnalogsEnsemble/tests/Data/observations.nc";
    
    string forecast_file = "forecasts.nc";
    string observation_file = "observations.nc";

    ForecastsArray forecasts;
    ObservationsArray observations;

    AnEnReadNcdf read_nc;
    read_nc.readForecasts(forecast_file, forecasts);
    read_nc.readObservations(observation_file, observations);

    cout << forecasts;
    cout << observations;

    return 0;
}
