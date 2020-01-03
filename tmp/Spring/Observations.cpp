/* 
 * File:   Observations.cpp
 * Author: Weiming Hu (weiming@psu.edu)
 * 
 * Created on May 4, 2018, 11:27 AM
 */

#include "Observations.h"
#include "colorTexts.h"

#include <stdexcept>
#include <algorithm>

using namespace std;

/*******************************************************************************
 *                               Observations                                  *
 ******************************************************************************/
Observations::Observations() {
}

Observations::Observations(const Parameters & parameters,
        const Stations & stations, const Times & times) : BasicData(parameters, stations,times) {
}

Observations::Observations(const Observations & orig) : BasicData(orig) {
}


Observations::~Observations() {
}

void
Observations::print(ostream& os) const {
    os << "[Observations] size: [" <<
            parameters_.size() << ", " <<
            stations_.size() << ", " <<
            times_.size() << "]" << endl << endl;
    os << parameters_ << endl;
    os << stations_ << endl;
    os << times_ << endl << endl;
}

ostream &
operator<<(ostream& os, const Observations & obj) {
    obj.print(os);
    return os;
}