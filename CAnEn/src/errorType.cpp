/* 
 * File:   AnEn.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 * 
 * Created on July 23, 2018, 10:56 PM
 */

#include "Functions.h"

#include <exception>
#include <string>

void handleError(const errorType & indicator) {

    using namespace std;

    if (indicator == SUCCESS || indicator == OPTIONAL_VARIABLE_MISSING) {
        return;
    } else {
        throw runtime_error("Error code: " + to_string((long long) indicator) + "\n");
    }
}