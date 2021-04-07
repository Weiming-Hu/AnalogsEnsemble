/*
 * File:   GridQuery.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on April 3, 2021, 11:15 AM
 */

#include "GridQuery.h"


/*************
 * GridQuery *
**************/

void
GridQuery::rectangle(const Matrix & mask, size_t time_index, long lead_time_index, long lead_time_radius,
        const Forecasts & forecasts, Array4D & arr_to) {

    // Initialization
    size_t H = mask.size1();
    size_t W = mask.size2();
    size_t C = forecasts.getParameters().size();

    long num_lead_times = forecasts.getFLTs().size();
    long lead_time_left = lead_time_index - lead_time_radius;
    long lead_time_right = lead_time_index + lead_time_radius;

    if (lead_time_left < 0) lead_time_left = 0;
    if (lead_time_right >= num_lead_times) lead_time_right = num_lead_times - 1;
    size_t T = lead_time_right - lead_time_left + 1;

    // Allocate memory
    arr_to.resize(T, C, H, W);

    // Copy values from forecasts to the array
    for (size_t i_T = 0; i_T < T; ++i_T) {
        size_t lead_time_index = lead_time_left + i_T;

        for (size_t i_H = 0; i_H < H; ++i_H) {
            for (size_t i_W = 0; i_W < W; ++i_W) {
                size_t station_index = mask(i_H, i_W);

                    for (size_t i_C = 0; i_C < C; ++i_C) {
                        double v = forecasts.getValue(i_C, station_index, time_index, lead_time_index);
                        arr_to.setValue(v, i_T, i_C, i_H, i_W);
                }
            }
        }
    }

    return;
}

