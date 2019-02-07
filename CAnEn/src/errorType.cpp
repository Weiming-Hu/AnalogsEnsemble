/* 
 * File:   AnEn.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 * 
 * Created on July 23, 2018, 10:56 PM
 */

#include "MathFunctions.h"

#include <exception>
#include <string>

void handleError(const errorType & indicator) {

    using namespace std;

    if (indicator == SUCCESS || indicator == OPTIONAL_VARIABLE_MISSING) {
        return;
    } else {
        throw runtime_error("Error code " + to_string((long long) indicator)
                + "\nCode reference: https://weiming-hu.github.io/AnalogsEnsemble/CXX/class_an_en_i_o.html#aa56bc1ec6610b86db4349bce20f9ead0");
    }
}