/*
 * Author: Weiming Hu
 *
 * Created on August 15, 2018
 *
 * Rinterface.cpp is the source file with the interface definition for
 * R functions to call C++ functions.
 */

// [[Rcpp::plugins(cpp11)]]
// [[Rcpp::plugins(openmp)]]
//
#include <Rcpp.h>

#if defined(_OPENMP)
#include <omp.h>
#endif

// Rcpp::Rcout is the wrapper for std::cout in Rcpp
#define cout Rcpp::Rcout
#define cerr Rcpp::Rcerr

// [[Rcpp::export("check_openmp")]]
bool check_openmp() {
#ifndef _OPENMP
    return false;
#else
    return true;
#endif
}

// [[Rcpp::export(".computeSimilarity")]]
Rcpp::

// [[Rcpp::export(".compute_analogs_hidden")]]
Rcpp::List compute_analogs_hidden(
        Rcpp::NumericVector forecasts_R,
        Rcpp::NumericVector forecasts_R_dims,
        Rcpp::NumericVector observations_R,
        Rcpp::NumericVector observations_R_dims,
        Rcpp::NumericVector circulars_R,
        Rcpp::NumericVector weights_R,
        int observation_ID,
        Rcpp::NumericVector stations_ID_R,
        int test_ID_start, int test_ID_end,
        int train_ID_start, int train_ID_end,
        int members_size, int rolling,
        bool quick, int num_cores,
        bool search_extension,
        Rcpp::NumericVector xs_R,
        Rcpp::NumericVector ys_R,
        double distance, int num_nearest,
        bool observation_from_extended_station,
        bool recompute_sd_for_extended_station,
        bool output_search_stations,
        bool output_metric,
        int verbose) {
    
    // simple dimension check
    if (forecasts_R_dims.size() != 4 || observations_R_dims.size() != 4) {
        cerr << "Error: both observations and forecasts should be 4-dimensional!" << std::endl;
        return 0;
    }

    // convert variables from Rcpp structures to C++ structures
    std::vector<double> vec_observations = Rcpp::as< std::vector<double> >(observations_R);
    Array4D observations(vec_observations,
            observations_R_dims[0], observations_R_dims[1],
            observations_R_dims[2], observations_R_dims[3]);
    std::vector<double> vec_forecasts= Rcpp::as< std::vector<double> >(forecasts_R);
    Array4D forecasts(vec_forecasts,
            forecasts_R_dims[0], forecasts_R_dims[1],
            forecasts_R_dims[2], forecasts_R_dims[3]);

    std::vector<double> weights = Rcpp::as< std::vector<double> >(weights_R);
    std::vector<std::size_t> stations_ID = Rcpp::as< std::vector<std::size_t> >(stations_ID_R);

    if (search_extension) {
        std::vector<double> xs = Rcpp::as< std::vector<double> >(xs_R);
        std::vector<double> ys = Rcpp::as< std::vector<double> >(ys_R);
        observations.set_xs(xs);
        observations.set_ys(ys);
        forecasts.set_xs(xs);
        forecasts.set_ys(ys);
    }

    if (circulars_R.size() != 0) {
        // there are circular variable in the forecasts
        // set the property of these variables accordingly
        //
        std::vector<int> circulars = Rcpp::as< std::vector<int> >(circulars_R);
        for (auto index : circulars) {
            // R counts from 1 but C counts from 0
            // subtract 1 in indexing
            //
            if (verbose >= 1) {
                cout << "setting parameter " << index << " as circular." << endl;
            }
            forecasts.setCircular(index - 1);
        }
    }

    // print out session information to console
    if (verbose == 2) {
        print_session_info(forecasts, observations, weights, stations_ID,
                test_ID_start, test_ID_end, train_ID_start, train_ID_end,
                members_size, rolling, quick, num_cores);
    }

    if (observation_ID < 1) {
        cerr << "ERROR: observation_ID (" << observation_ID << ") < 1!" << endl;
        return 0;
    }
    if (test_ID_start < 1) {
        cerr << "ERROR: test_ID_start (" << test_ID_start << ") < 1!" << endl;
        return 0;
    }
    if (test_ID_end < 1) {
        cerr << "ERROR: test_ID_end (" << test_ID_end << ") < 1!" << endl;
        return 0;
    }
    if (train_ID_start < 1) {
        cerr << "ERROR: train_ID_start (" << train_ID_start << ") < 1!" << endl;
        return 0;
    }
    if (train_ID_end < 1) {
        cerr << "ERROR: train_ID_end (" << train_ID_end << ") < 1!" << endl;
        return 0;
    }

    // convert indexing from R style (counting from 1) to 
    // C style (counting from 0)
    //
    for (auto& index : stations_ID) {
        index -= 1;
    }
    test_ID_start -= 1;
    test_ID_end -= 1;
    train_ID_start -= 1;
    train_ID_end -= 1;
    observation_ID -= 1;

    if (members_size < 0) {
        cerr << "ERROR: members_size (" << members_size << ") < 0!" << endl;
        return 0;
    }
    if (num_cores < 0) {
        cerr << "ERROR: num_cores (" << num_cores << ") < 0!" << endl;
        return 0;
    }

    AnEn AnEn_calc(weights, observation_ID, stations_ID,
            test_ID_start, test_ID_end,
            train_ID_start, train_ID_end,
            members_size, rolling, quick, num_cores,
            search_extension,
            observation_from_extended_station,
            recompute_sd_for_extended_station,
            "");

    AnEn_calc.set_distance(distance);
    AnEn_calc.set_num_nearest((size_t)num_nearest);
    if (output_metric) {
        AnEn_calc.set_output_metric("inMemory");
    }
    if (output_search_stations) {
        AnEn_calc.switch_save_search_stations(true);
    }
        
    if (verbose > 1) {
        cout << "Checking parameters ..." << endl;
    }
    
    if (!AnEn_calc.validate_parameters(forecasts, observations)) {
        cout << "failed!" << endl;
        return 1;
    }

    if (verbose > 0) {
        cout << "Start computing analogs ...";
        cout.flush();
    }
    
    Array5D analogs = AnEn_calc.computeAnalogs(
            forecasts, observations);
    
    if (verbose > 0) {
        cout << "Done!" << std::endl;
        cout.flush();

#ifndef _OPENMP
        cout << "*** Multi-thread is not supported by the compiler. Please refer to README if this concerns you. ***" << std::endl;
#endif
    }

    // data re-formatting and output
    //
    if (verbose > 0) {
        cout << "Transporting data from C++ to R ...";
        cout.flush();
    }

    Rcpp::List ret;

    // to return to Rcpp::NumericVector, the below data type conversion is carried out:
    // Array4D -> vector<double> -> Rcpp::NumericVector
    //
    // Pay attention to the indexing and for-loop
    // multidimensional arrays in C/C++ are stored in row-   major order
    //            while arrays in   R   are stored in column-major order
    //
    // Here are generally three cases, value, single_index, and double index
    // the first two has the same dimensions, so they can be dealt by two
    // processes
    //
    int num_stations, num_days, num_flts, num_members, num_indices;
    num_stations = analogs.shape()[0];
    num_days = analogs.shape()[1];
    num_flts = analogs.shape()[2];
    num_members = analogs.shape()[3];
    num_indices = analogs.shape()[4];

    vector<double> analogs_1D(num_stations * num_days * num_flts * num_members * num_indices);
    size_t index = 0;

    for (int k = 0; k < num_indices; k++)
        for (int i = 0; i < num_members; i++)
            for (int j = 0; j < num_flts; j++)
                for (int m = 0; m < num_days; m++)
                    for (int n = 0; n < num_stations; n++, index++) {

                        switch(k) {
                            case 0: // observation values are stored at the first position
                                    analogs_1D[index] = analogs[n][m][j][i][k];
                                    break;
                            default: // all the others should be index
                                    analogs_1D[index] = analogs[n][m][j][i][k] + 1;
                        }

                    }
    Rcpp::NumericVector analogs_R(analogs_1D.begin(), analogs_1D.end());

    // assign the dimension information
    vector<int> dim = {num_stations, num_days, num_flts, num_members, num_indices};
    Rcpp::IntegerVector dim_R(dim.begin(), dim.end());
    analogs_R.attr("dim") = dim_R;
    analogs_R.attr("class") = "array";
    ret["analogs"] = analogs_R;

    if (output_metric) {
        const boost::multi_array<Array2D, 3> & metrics =
            analogs.get_const_metrics();
        Rcpp::List metrics_R;
        for (int m = 0; m < num_stations; m++) {
            for (int n = 0; n < num_days; n++) {
                for (int k = 0; k < num_flts; k++) {
                    std::string ele_name = 
                        "station_" + std::to_string(stations_ID.at(m) + 1) + "_" +
                        "day_" + std::to_string(test_ID_start + n + 1) + "_" +
                        "flt_" + std::to_string(k + 1);
                    size_t d1 = metrics[m][n][k].sizeM();
                    size_t d2 = metrics[m][n][k].sizeN();
                    std::vector<double> metric(d1*d2);
                    index = 0;
                    for (int i = 0; i < d2; i++) {
                        for (int j = 0; j < d1; j++, index++) {
                            switch(i) {
                                case 0: // observation values are stored at the first position
                                    metric[index] = (metrics[m][n][k])[j][i];
                                    break;
                                default: // all the others should be index
                                    metric[index] = (metrics[m][n][k])[j][i] + 1;
                            }
                        }
                    }

                    Rcpp::NumericVector metric_R(metric.begin(), metric.end());

                    vector<size_t> dim_metric = {d1, d2};
                    Rcpp::IntegerVector dim_metric_R(dim_metric.begin(), dim_metric.end());
                    metric_R.attr("dim") = dim_metric_R;
                    Rcpp::colnames(metric_R) = Rcpp::CharacterVector::create(
                            "metric", "station", "day");

                    metrics_R[ele_name] = metric_R;
                }
            }
        }

        metrics_R.attr("class") = "metrics";
        ret["metrics"] = metrics_R;
    }

    if (output_search_stations) {
        const std::vector< std::vector<size_t> > & vec_search_stations =
            analogs.get_const_vec_search_stations();
        Rcpp::List search_stations_R_list;
        for (size_t i = 0; i < num_stations; i++) {
            vector<size_t> search_stations(vec_search_stations[i]);
            for(auto & val : search_stations) val += 1;
            Rcpp::IntegerVector search_stations_R(search_stations.begin(), search_stations.end());
            string ele_name = "station_" + std::to_string(stations_ID.at(i) + 1);
            search_stations_R_list[ele_name] = search_stations_R;
        }

        search_stations_R_list.attr("class") = "search_stations";
        ret["search_stations"] = search_stations_R_list;
    }
    
    if (verbose > 0) {
        cout << "Done!" << std::endl;
        cout.flush();
    }

    ret.attr("class") = "AnEn";
    return ret;
 }
