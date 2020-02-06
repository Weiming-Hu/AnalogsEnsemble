#  "`-''-/").___..--''"`-._
# (`6_ 6  )   `-.  (     ).`-.__.`)   WE ARE ...
# (_Y_.)'  ._   )  `._ `. ``-..-'    PENN STATE!
#   _ ..`--'_..-_/  /--'_.' ,'
# (il),-''  (li),'  ((!.-'
#
#
# Author: Weiming Hu (weiming@psu.edu), Laura Clemente-Harding (laura@psu.edu)
#         Geoinformatics and Earth Observation Laboratory (http://geolab.psu.edu)
#         Department of Geography and Institute for CyberScience
#         The Pennsylvania State University
#

#' RAnEn::toMemberForecasts
#' 
#' RAnEn::toMemberForecasts finds the most similar historical forecasts
#' that are associated with each analog members.
#' 
#' @author Weiming Hu \email{weiming@@psu.edu}
#' @author Laura Clemente-Harding \email{laura@@psu.edu}
#' @param forecasts The Data from Forecasts. Usually it is a 4D array.
#' @param sims.time.index The similarity time index array from AnEn.
#' @param sims.stations.index The similarity station index array from AnEn.
#' @param stations.index The station index of the analogs.
#' @param test.times.index The test time index of the analogs.
#' @param flts.index The forecast lead time index of the analogs.
#' @param members.index The analog members index.
#' @param parameters.index The forecast parameters index.
#' @param verbose Whether to show progress and summary information
#' @return A 5 dimensional array with the shape `[parameters][stations][times][flts][members]`.
#'
#' @md
#' @export
toMemberForecasts <- function(
  forecasts, sims.time.index, sims.stations.index = NULL, 
  stations.index = 1, test.times.index = 1, flts.index = 1,
  members.index = NULL, parameters.index = NULL, verbose = T) {
  
  # Overflow check
  if (is.null(stations.index)) {
    stations.index <- 1:dim(sims.time.index)[1]
  } else {
    stopifnot(max(stations.index) <= dim(sims.time.index)[1])
  }
  
  if (is.null(test.times.index)) {
    test.times.index <- 1:dim(sims.time.index)[2]
  } else {
    stopifnot(max(test.times.index) <= dim(sims.time.index)[2])
  }
  
  if (is.null(flts.index)) {
    flts.index <- 1:dim(sims.time.index)[3]
  } else {
    stopifnot(max(flts.index) <= dim(sims.time.index)[3])
  }
  
  if (is.null(members.index)) {
    members.index <- 1:dim(sims.time.index)[4]
  } else {
    stopifnot(members.index <= dim(sims.time.index)[4])
  }
  
  if (is.null(parameters.index)) {
    parameters.index <- 1:dim(forecasts)[1]
  } else {
    stopifnot(parameters.index <= dim(forecasts)[1])
  }
  
  ###########################################################################
  #                                                                         #
  #  The most important step is to create the four dimensional indices for  #
  #  the historical forecasts associated with each ensemble member.         #
  #                                                                         #
  ###########################################################################
  #
  if (verbose) {
    cat("Finding the associated historical forecasts (this might take a while) ...\n")
  }
  
  # 
  # Since R array is in column major meaning that the internal vector storage
  # has the first dimension varies the fastest. We need to create our indices 
  # in this fashion that the first dimension varies the fastest.
  # 
  # Let's first consider the forecast indices for one particular parameter.
  # The dimensions will be [stations, times, flts, members]. Remeber, the
  # first dimension varies the fastest.
  # 
  forecast.times.index <- as.vector(sims.time.index[stations.index, test.times.index, flts.index, members.index])
  
  if (is.null(sims.stations.index)) {
    right.dimensions.len <- length(test.times.index) * length(flts.index) * length(members.index)
    forecast.stations.index <- rep(stations.index, times = right.dimensions.len)
  } else {
    # Check the index arrays should have consistent dimensions
    stopifnot(identical(dim(sims.time.index), dim(sims.stations.index)))
    forecast.stations.index <- as.vector(sims.stations.index[stations.index, test.times.index, flts.index, members.index])
  }
  
  left.dimensions.len <- length(stations.index) * length(test.times.index)
  forecast.flts.index <- rep(rep(flts.index, each = left.dimensions.len), times = length(members.index))
  
  # Now we can have the indices for one particular forecast parameter by column binding
  indices <- cbind(forecast.stations.index, forecast.times.index, forecast.flts.index)
  
  # But we might want more parameters. We need to replicate these indices. Remeber,
  # the first dimension varies the fastest and parameter is the first dimension
  # in forecasts.
  # 
  forecast.parameters <- rep(parameters.index, times = nrow(indices))
  indices <- indices[rep(1:nrow(indices), each = length(parameters.index)), ]
  
  # Finally we have all the indices
  indices <- cbind(forecast.parameters, indices)
  
  # We can then easily access the values
  if (verbose) {
    cat("Indices found! Copying forecasts ...\n")
  }
  
  analog.forecasts <- array(
    data = forecasts[indices],
    dim = c(length(parameters.index), length(stations.index),
            length(test.times.index), length(flts.index), length(members.index)))
  
  if (verbose) {
    cat("\n*******        Summary Information       ******\n")
    cat("An array with the dimensions [")
    cat(paste0(dim(analog.forecasts), collapse = ','))
    cat("] has been created.\nThis shape indicates:\n")
    cat("-", dim(analog.forecasts)[1], "parameters\n")
    cat("-", dim(analog.forecasts)[2], "stations\n")
    cat("-", dim(analog.forecasts)[3], "test times\n")
    cat("-", dim(analog.forecasts)[4], "forecast lead times\n")
    cat("-", dim(analog.forecasts)[5], "ensemble members\n")
    cat("*****    End of the Summary Information    ****\n")
  }
  
  return(analog.forecasts)
}
