/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   newsimpletest.cpp
 * Author: guido
 *
 * Created on April 17, 2018, 11:07 PM
 */

#include <stdlib.h>
#include <iostream>
#include "Stations.h"

/*
 * Simple C++ Test Suite
 */

int main(int argc, char** argv) {

    Station s1;
    std::cout << s1 << std::endl;
    
    Station s2(10,20,30);
    std::cout << s2 << std::endl;
    
    Station s3 = s2;
    std::cout << s3 << std::endl;
    
    s1 = s2;
    std::cout << s1 << std::endl;

    
    Stations stations;
    
    stations.push_back(s1);
    std::cout<<stations<<std::endl;
    
    
    stations.push_back(s2);
    std::cout<<stations<<std::endl;
    
    
}

