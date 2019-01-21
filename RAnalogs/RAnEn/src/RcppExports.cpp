// Generated by using Rcpp::compileAttributes() -> do not edit by hand
// Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#include <Rcpp.h>

using namespace Rcpp;

// checkOpenMP
bool checkOpenMP();
RcppExport SEXP _RAnEn_checkOpenMP() {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    rcpp_result_gen = Rcpp::wrap(checkOpenMP());
    return rcpp_result_gen;
END_RCPP
}
// generateAnalogs
List generateAnalogs(NumericVector R_test_forecasts, NumericVector R_test_forecasts_dims, NumericVector R_test_forecasts_station_x, NumericVector R_test_forecasts_station_y, NumericVector R_search_forecasts, NumericVector R_search_forecasts_dims, NumericVector R_search_forecasts_station_x, NumericVector R_search_forecasts_station_y, NumericVector R_search_times, NumericVector R_search_flts, NumericVector R_search_observations, NumericVector R_search_observations_dims, NumericVector R_observation_times, size_t num_members, size_t observation_parameter, bool quick, IntegerVector R_circulars, bool search_extension, bool extend_observations, bool preserve_similarity, bool preserve_mapping, bool preserve_search_stations, size_t max_num_search_stations, double distance, size_t num_nearest_stations, int time_match_mode, double max_par_nan, double max_flt_nan, int verbose);
RcppExport SEXP _RAnEn_generateAnalogs(SEXP R_test_forecastsSEXP, SEXP R_test_forecasts_dimsSEXP, SEXP R_test_forecasts_station_xSEXP, SEXP R_test_forecasts_station_ySEXP, SEXP R_search_forecastsSEXP, SEXP R_search_forecasts_dimsSEXP, SEXP R_search_forecasts_station_xSEXP, SEXP R_search_forecasts_station_ySEXP, SEXP R_search_timesSEXP, SEXP R_search_fltsSEXP, SEXP R_search_observationsSEXP, SEXP R_search_observations_dimsSEXP, SEXP R_observation_timesSEXP, SEXP num_membersSEXP, SEXP observation_parameterSEXP, SEXP quickSEXP, SEXP R_circularsSEXP, SEXP search_extensionSEXP, SEXP extend_observationsSEXP, SEXP preserve_similaritySEXP, SEXP preserve_mappingSEXP, SEXP preserve_search_stationsSEXP, SEXP max_num_search_stationsSEXP, SEXP distanceSEXP, SEXP num_nearest_stationsSEXP, SEXP time_match_modeSEXP, SEXP max_par_nanSEXP, SEXP max_flt_nanSEXP, SEXP verboseSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< NumericVector >::type R_test_forecasts(R_test_forecastsSEXP);
    Rcpp::traits::input_parameter< NumericVector >::type R_test_forecasts_dims(R_test_forecasts_dimsSEXP);
    Rcpp::traits::input_parameter< NumericVector >::type R_test_forecasts_station_x(R_test_forecasts_station_xSEXP);
    Rcpp::traits::input_parameter< NumericVector >::type R_test_forecasts_station_y(R_test_forecasts_station_ySEXP);
    Rcpp::traits::input_parameter< NumericVector >::type R_search_forecasts(R_search_forecastsSEXP);
    Rcpp::traits::input_parameter< NumericVector >::type R_search_forecasts_dims(R_search_forecasts_dimsSEXP);
    Rcpp::traits::input_parameter< NumericVector >::type R_search_forecasts_station_x(R_search_forecasts_station_xSEXP);
    Rcpp::traits::input_parameter< NumericVector >::type R_search_forecasts_station_y(R_search_forecasts_station_ySEXP);
    Rcpp::traits::input_parameter< NumericVector >::type R_search_times(R_search_timesSEXP);
    Rcpp::traits::input_parameter< NumericVector >::type R_search_flts(R_search_fltsSEXP);
    Rcpp::traits::input_parameter< NumericVector >::type R_search_observations(R_search_observationsSEXP);
    Rcpp::traits::input_parameter< NumericVector >::type R_search_observations_dims(R_search_observations_dimsSEXP);
    Rcpp::traits::input_parameter< NumericVector >::type R_observation_times(R_observation_timesSEXP);
    Rcpp::traits::input_parameter< size_t >::type num_members(num_membersSEXP);
    Rcpp::traits::input_parameter< size_t >::type observation_parameter(observation_parameterSEXP);
    Rcpp::traits::input_parameter< bool >::type quick(quickSEXP);
    Rcpp::traits::input_parameter< IntegerVector >::type R_circulars(R_circularsSEXP);
    Rcpp::traits::input_parameter< bool >::type search_extension(search_extensionSEXP);
    Rcpp::traits::input_parameter< bool >::type extend_observations(extend_observationsSEXP);
    Rcpp::traits::input_parameter< bool >::type preserve_similarity(preserve_similaritySEXP);
    Rcpp::traits::input_parameter< bool >::type preserve_mapping(preserve_mappingSEXP);
    Rcpp::traits::input_parameter< bool >::type preserve_search_stations(preserve_search_stationsSEXP);
    Rcpp::traits::input_parameter< size_t >::type max_num_search_stations(max_num_search_stationsSEXP);
    Rcpp::traits::input_parameter< double >::type distance(distanceSEXP);
    Rcpp::traits::input_parameter< size_t >::type num_nearest_stations(num_nearest_stationsSEXP);
    Rcpp::traits::input_parameter< int >::type time_match_mode(time_match_modeSEXP);
    Rcpp::traits::input_parameter< double >::type max_par_nan(max_par_nanSEXP);
    Rcpp::traits::input_parameter< double >::type max_flt_nan(max_flt_nanSEXP);
    Rcpp::traits::input_parameter< int >::type verbose(verboseSEXP);
    rcpp_result_gen = Rcpp::wrap(generateAnalogs(R_test_forecasts, R_test_forecasts_dims, R_test_forecasts_station_x, R_test_forecasts_station_y, R_search_forecasts, R_search_forecasts_dims, R_search_forecasts_station_x, R_search_forecasts_station_y, R_search_times, R_search_flts, R_search_observations, R_search_observations_dims, R_observation_times, num_members, observation_parameter, quick, R_circulars, search_extension, extend_observations, preserve_similarity, preserve_mapping, preserve_search_stations, max_num_search_stations, distance, num_nearest_stations, time_match_mode, max_par_nan, max_flt_nan, verbose));
    return rcpp_result_gen;
END_RCPP
}

static const R_CallMethodDef CallEntries[] = {
    {"_RAnEn_checkOpenMP", (DL_FUNC) &_RAnEn_checkOpenMP, 0},
    {"_RAnEn_generateAnalogs", (DL_FUNC) &_RAnEn_generateAnalogs, 29},
    {NULL, NULL, 0}
};

RcppExport void R_init_RAnEn(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
