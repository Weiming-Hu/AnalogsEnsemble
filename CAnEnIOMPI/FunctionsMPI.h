/* 
 * File:   FunctionsMPI.h
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on March, 2020, 11:10 AM
 */


#ifndef FUNCTIONSMPI_H
#define FUNCTIONSMPI_H

#include <vector>

namespace FunctionsMPI {
    int getStartIndex(int total, int num_procs, int rank);
    int getEndIndex(int total, int num_procs, int rank);
    int getSubTotal(int grand_total, int num_procs, int rank);
};

#endif /* FUNCTIONSMPI_H */
