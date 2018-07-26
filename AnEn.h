/* 
 * File:   AnEn.h
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on July 23, 2018, 10:56 PM
 */

#ifndef ANEN_H
#define ANEN_H

#include "AnEnIO.h"

class AnEn {
public:
    AnEn();
    AnEn(const AnEn& orig);
    virtual ~AnEn();

    // TODO
    void compute_similarity();

    // TODO
    void select_analogs();

private:

    /*
     * Specifies the mode of the processes. It can be
     * - memory
     * - file
     */
    std::string mode;


};

#endif /* ANEN_H */

