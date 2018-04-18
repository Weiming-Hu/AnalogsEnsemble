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

    Station s1(10, -2, 3);
    cout << s1 << endl;
    
    Station s2(100,40,30);
    
    Stations stations;
    stations.push_back(s1);
    stations.push_back(s2);
    
    std::cout <<  stations << std::endl;
    
    
    Parameter p1("Temperature", .1, false);
    Parameter p2 = p1;
    Parameter p3;
    Parameter p4 = p2;
    
    Parameters parameters;
    parameters.push_back(p1);
    parameters.push_back(p2);
    parameters.push_back(p3);
    parameters.push_back(p4);
    
    std::cout <<  parameters << std::endl;

    Parameters ppp = parameters;
     
    std::cout <<  ppp << std::endl;
    
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
    
    std::cout << ts << std::endl;
    std::cout << flt << std::endl;
    
    Forecasts_array forecasts(parameters, stations, ts, flt);
    std::cout << forecasts << endl;
        
        
    return 0;
}

