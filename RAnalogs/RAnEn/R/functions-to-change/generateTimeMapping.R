# "`-''-/").___..--''"`-._
#  (`6_ 6  )   `-.  (     ).`-.__.`)   WE ARE ...
#  (_Y_.)'  ._   )  `._ `. ``-..-'    PENN STATE!
#    _ ..`--'_..-_/  /--'_.' ,'
#  (il),-''  (li),'  ((!.-'
# 
# Author: Weiming Hu <weiming@psu.edu>
#         Geoinformatics and Earth Observation Laboratory (http://geolab.psu.edu)
#         Department of Geography and Institute for CyberScience
#         The Pennsylvania State University
#

#' RAnEn::generateTimeMapping
#' 
#' RAnEn::generateTimeMapping returns a matrix that maps each combination of
#' forecast times and flts to observation times. The rows are forecast FLTs 
#' and columns are forecast times, the values are the index of the corresponding
#' observation times. Same results can be generated from the function
#' \code{\link{generateConfiguration}} if \bold{preserve_mapping} is selected.
#' 
#' @author Weiming Hu \email{weiming@@psu.edu}
#' 
#' @param forecast_times A vector of forecast times.
#' @param forecast_flts A vector of forecast FLTs.
#' @param observation_times A vector of observation times.
#' @param time_match_mode This is the method used to computed the mapping matrix
#' between forecast times/flts and observation times. 0 for strict search (return
#' error when matching observation times cannot be found) and 1 for loose search.
#' @param verbose Verbose level. You can find settings for different values in the
#' [C++ documentation verbose](https://weiming-hu.github.io/AnalogsEnsemble/CXX/class_an_en.html#a25984b953516a987e2e9eb23048e5d60).
#' 
#' @return A matrix with rows for forecast FLTs and columns for forecast times.
#' 
#' @import Rcpp
#' 
#' @useDynLib RAnEn
#' 
#' @export
generateTimeMapping <- function(
  forecast_times, forecast_flts, observation_times, time_match_mode = 0, verbose = 2) {
  
  forecast_times <- as.vector(forecast_times)
  forecast_flts <- as.vector(forecast_flts)
  observation_times <- as.vector(observation_times)
  
  if (inherits(forecast_times, 'POSIXt')) {
    forecast_times <- as.numeric(forecast_times)
  }
  if (inherits(forecast_flts, 'POSIXt')) {
    forecast_flts <- as.numeric(forecast_flts)
  }
  if (inherits(observation_times, 'POSIXt')) {
    observation_times <- as.numeric(observation_times)
  }
  
  ret <- .generateTimeMapping(forecast_times, forecast_flts, observation_times, time_match_mode, verbose)
  
  # Convert from C++ index to R index
  ret <- ret + 1
  
  return(ret)
}
