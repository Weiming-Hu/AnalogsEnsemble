/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   canalogs.cpp
 * Author: guido
 *
 * Created on April 17, 2018, 11:10 PM
 */

#include <cstdlib>
#include "Stations.h"
#include "Parameters.h"
#include "Time.h"
#include "Forecasts.h"
#include <iostream>

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {

    Station s1("S1",-2, 3);
    Station s2("S2",40,30);
    Station s3("S3",40,30);
    Station s4("S4",40,30);
    Station s5("S5",40,30);
        
    Stations stations;
    stations.insert(s1);
    stations.insert(s2);
    stations.insert(s3);
    stations.insert(s4);
    stations.insert(s5);
    
    
    Parameter p1("Temperature", .1, false);
    Parameter p2 = p1;
    Parameter p3;
    Parameter p4("Wind Direction",1,true);
    Parameter p5("Wind Speed",1,false);
    Parameter p6("Wind Gust",1,false);
    
    Parameters parameters;
    parameters.push_back(p1);
    parameters.push_back(p2);
    parameters.push_back(p3);
    parameters.push_back(p4);
    //parameters.push_back(p5);
    //parameters.push_back(p6);
    
    
    Time ts;
    ts.push_back(1000);
    ts.push_back(2000);
    ts.push_back(3000);
    ts.push_back(4000);
    ts.push_back(5000);
    ts.push_back(6000);
       
    FLT flt;
    flt.push_back(0);
    flt.push_back(100);
    flt.push_back(200);
    flt.push_back(300);
    
    
    
    
    Forecasts_array forecasts(parameters, stations, ts, flt);
    
    forecasts.setValue(99.99,1,1,1,1);
    std::cout << forecasts << endl;
        
        
    vector<int> ret = stations.getNearbyStations(s4) ;
    
    cout << ret[0] << endl;
    cout << ret[1] << endl;
    cout << ret[2] << endl;
    

    ret = stations.getNearbyStations(2) ;
    
    cout << ret[0] << endl;
    cout << ret[1] << endl;
    cout << ret[2] << endl;

    
    Station sn1 = stations[20];
    Station sn2 = stations[s4];
    
    cout << "Is " << sn1 << "smaller than " << sn2 << " == " << (sn1 < sn2) << endl;
    
    
    
    
    
    return 0;
}

