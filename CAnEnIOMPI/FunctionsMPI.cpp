/* 
 * File:   FunctionsMPI.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on March, 2020, 11:10 AM
 */

#include "FunctionsMPI.h"

#include <cmath>

using namespace std;

int
FunctionsMPI::getStartIndex(int total, int num_procs, int rank) {
    // Rank 0 is the master. Start with rank 1.
    return (int) ceil((rank - 1) * total / (float) (num_procs - 1));
}

int
FunctionsMPI::getEndIndex(int total, int num_procs, int rank) {
    // Rank 0 is the master. Start with rank 1.
    if (rank == num_procs - 1) return total;
    return (int) ceil(rank * total / (float) (num_procs - 1));
}

int
FunctionsMPI::getSubTotal(int total, int num_procs, int rank) {
    return getEndIndex(total, num_procs, rank) - getStartIndex(total, num_procs, rank);
}

