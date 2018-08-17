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
List generateAnalogs(NumericVector R_test_forecasts, NumericVector R_test_forecasts_dims, NumericVector R_search_forecasts, NumericVector R_search_forecasts_dims, NumericVector R_search_times, NumericVector R_search_flts, NumericVector R_search_observations, NumericVector R_search_observations_dims, NumericVector R_observation_times, size_t num_members, size_t observation_parameter, bool quick, IntegerVector R_circulars, bool preserve_similarity, int verbose);
RcppExport SEXP _RAnEn_generateAnalogs(SEXP R_test_forecastsSEXP, SEXP R_test_forecasts_dimsSEXP, SEXP R_search_forecastsSEXP, SEXP R_search_forecasts_dimsSEXP, SEXP R_search_timesSEXP, SEXP R_search_fltsSEXP, SEXP R_search_observationsSEXP, SEXP R_search_observations_dimsSEXP, SEXP R_observation_timesSEXP, SEXP num_membersSEXP, SEXP observation_parameterSEXP, SEXP quickSEXP, SEXP R_circularsSEXP, SEXP preserve_similaritySEXP, SEXP verboseSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< NumericVector >::type R_test_forecasts(R_test_forecastsSEXP);
    Rcpp::traits::input_parameter< NumericVector >::type R_test_forecasts_dims(R_test_forecasts_dimsSEXP);
    Rcpp::traits::input_parameter< NumericVector >::type R_search_forecasts(R_search_forecastsSEXP);
    Rcpp::traits::input_parameter< NumericVector >::type R_search_forecasts_dims(R_search_forecasts_dimsSEXP);
    Rcpp::traits::input_parameter< NumericVector >::type R_search_times(R_search_timesSEXP);
    Rcpp::traits::input_parameter< NumericVector >::type R_search_flts(R_search_fltsSEXP);
    Rcpp::traits::input_parameter< NumericVector >::type R_search_observations(R_search_observationsSEXP);
    Rcpp::traits::input_parameter< NumericVector >::type R_search_observations_dims(R_search_observations_dimsSEXP);
    Rcpp::traits::input_parameter< NumericVector >::type R_observation_times(R_observation_timesSEXP);
    Rcpp::traits::input_parameter< size_t >::type num_members(num_membersSEXP);
    Rcpp::traits::input_parameter< size_t >::type observation_parameter(observation_parameterSEXP);
    Rcpp::traits::input_parameter< bool >::type quick(quickSEXP);
    Rcpp::traits::input_parameter< IntegerVector >::type R_circulars(R_circularsSEXP);
    Rcpp::traits::input_parameter< bool >::type preserve_similarity(preserve_similaritySEXP);
    Rcpp::traits::input_parameter< int >::type verbose(verboseSEXP);
    rcpp_result_gen = Rcpp::wrap(generateAnalogs(R_test_forecasts, R_test_forecasts_dims, R_search_forecasts, R_search_forecasts_dims, R_search_times, R_search_flts, R_search_observations, R_search_observations_dims, R_observation_times, num_members, observation_parameter, quick, R_circulars, preserve_similarity, verbose));
    return rcpp_result_gen;
END_RCPP
}

static const R_CallMethodDef CallEntries[] = {
    {"_RAnEn_checkOpenMP", (DL_FUNC) &_RAnEn_checkOpenMP, 0},
    {"_RAnEn_generateAnalogs", (DL_FUNC) &_RAnEn_generateAnalogs, 15},
    {NULL, NULL, 0}
};

RcppExport void R_init_RAnEn(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
