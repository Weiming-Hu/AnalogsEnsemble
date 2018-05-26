/* 
 * File:   canalogs.cpp
 * Author: guido
 *
 * Created on April 17, 2018, 11:10 PM
 */

#include <cstdlib>
#include <string>

#include "Stations.h"
#include "Parameters.h"
#include "Times.h"
#include "Forecasts.h"
#include "AnEnIO.h"

#include <iostream>

using namespace std;

/*
 * 
 */

int main(int argc, char** argv) {
    string file(argv[argc - 1]);
    AnEnIO::checkForecasts("happy");
    return (0);
}

int main_old(int argc, char** argv) {

    Station s1("S1", 10, 5);
    Station s2("S2", 12, 7);
    Station s3("S3", 30, 30);

    Stations stations;
    stations.insert(s1);
    stations.insert(s2);
    stations.insert(s3);


    Parameter p1("Temperature", .1, false);
    Parameter p2 = p1;
    Parameter p3;
    Parameter p4("Wind Direction", 1, true);

    Parameters parameters;
    parameters.push_back(p1);
    parameters.push_back(p2);
    parameters.push_back(p3);
    parameters.push_back(p4);

    Times ts;
    ts.insert(1000);
    ts.insert(2000);
    ts.insert(3000);
    ts.insert(4000);

    FLTs flt;
    flt.insert(0);
    flt.insert(100);
    flt.insert(200);



    // polymorphism
    Forecasts_array forecasts(parameters, stations, ts, flt);
    //Forecasts &forecasts = forecasts_array;

    forecasts.setValue(99.99, 1, 1, 1, 1);

    std::cout << "TEST -- Print the forecast." << endl
            << "There should be only one value" << endl;

    std::cout << forecasts << endl;


    std::cout << "TEST -- Reference station s1." << endl;
    cout << stations[s1] << endl;
    cout << endl;
    
    std::cout << "TEST -- Reference station 1." << endl;
    cout << stations[1] << endl;
    cout << endl;
    std::cout << "TEST -- Reference station 10.  This should be true" << endl;
    cout << (stations[10] == *stations.end()) << endl << endl;
    cout << endl;


    std::cout << "TEST -- Print the nearby stations for s1." << endl;
    vector<int> ret = stations.getNearbyStations(s1);
    copy(ret.begin(), ret.end(), ostream_iterator<int>(cout, "\n"));
    cout << stations[ ret.front() ] << endl;
    cout << endl;


    std::cout << "TEST -- Print the nearby stations for s2." << endl;
    ret = stations.getNearbyStations(2);
    copy(ret.begin(), ret.end(), ostream_iterator<int>(cout, "\n"));

    cout << stations[ret.front()] << endl;
    cout << endl;

    Station sn1 = stations[2];
    Station sn2 = stations[s3];

    cout << "TEST -- Is " << endl
            << sn1 << endl
            << "smaller than " << endl
            << sn2 << endl
            << ((sn1 < sn2) ? "YES" : "NO" ) << endl;
    cout << endl;



    return 0;
}

