/* 
 * File:   AnEnSSE.tpp
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <cervone@psu.edu>
 *
 * Created on February 1, 2020, 12:37 PM
 */

template <std::size_t len>
void
AnEnSSE::saveAnalogs_(const SimsVec<len> & sims_arr, const Observations & observations,
        std::size_t station_i, std::size_t test_time_i, std::size_t flt_i) {

    for (std::size_t analog_i = 0; analog_i < num_analogs_; ++analog_i) {

        // Skip assigning values if the similarity metric is NAN
        if (std::isnan(sims_arr[analog_i][_SIM_VALUE_INDEX])) continue;

        double obs_time_index = sims_arr[analog_i][_SIM_OBS_TIME_INDEX];
        if (std::isnan(obs_time_index)) continue;

        size_t obs_station_index;
        if (extend_obs_) {
            obs_station_index = sims_arr[analog_i][_SIM_STATION_INDEX];
            if (std::isnan(obs_station_index)) continue;
        } else {
            obs_station_index = station_i;
        }

        double obs_value = observations.getValue(
                obs_var_index_, obs_station_index, obs_time_index);
        analogs_value_.setValue(obs_value,
                station_i, test_time_i, flt_i, analog_i);
    }
    return;
}

template <std::size_t len>
void
AnEnSSE::saveSimsStationIndex_(const SimsVec<len> & sims_arr,
        std::size_t station_i, std::size_t test_time_i, std::size_t flt_i) {

    for (std::size_t sim_i = 0; sim_i < num_sims_; ++sim_i) {

        // Skip assigning values if the similarity metric is NAN
        if (std::isnan(sims_arr[sim_i][_SIM_VALUE_INDEX])) continue;

        sims_station_index_.setValue(sims_arr[sim_i][_SIM_STATION_INDEX],
                station_i, test_time_i, flt_i, sim_i);
    }
    return;
}
