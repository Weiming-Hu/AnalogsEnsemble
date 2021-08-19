/*
 * File:   GridQuery.h
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on April 3, 2021, 10:54 AM
 */

#ifndef GRIDQUERY_H
#define GRIDQUERY_H

#include "Forecasts.h"
#include "Grid.h"

namespace GridQuery {
    void rectangle(const Matrix & mask, std::size_t time_index,
            long lead_time_index, long lead_time_radius,
            const Forecasts & forecasts, Array4D & arr_to);
}

#endif
