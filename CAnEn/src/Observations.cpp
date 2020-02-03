/* 
 * File:   Observations.cpp
 * Author: Weiming Hu (weiming@psu.edu)
 * 
 * Created on May 4, 2018, 11:27 AM
 */

#include "Observations.h"

using namespace std;

Observations::Observations() : BasicData() {
}

Observations::Observations(const Parameters & parameters,
        const Stations & stations, const Times & times) :
BasicData(parameters, stations,times) {
}

Observations::Observations(const Observations & orig) :
BasicData(orig) {
}

Observations::~Observations() {
}

void
Observations::print(ostream& os) const {
    os << "[Observations] size: [" <<
            parameters_.size() << ", " <<
            stations_.size() << ", " <<
            times_.size() << "]" << endl;
    BasicData::print(os);
    return;
}

ostream &
operator<<(ostream& os, const Observations & obj) {
    obj.print(os);
    return os;
}