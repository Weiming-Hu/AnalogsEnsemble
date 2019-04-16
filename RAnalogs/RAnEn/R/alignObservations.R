# "`-''-/").___..--''"`-._
#  (`6_ 6  )   `-.  (     ).`-.__.`)   WE ARE ...
#  (_Y_.)'  ._   )  `._ `. ``-..-'    PENN STATE!
#    _ ..`--'_..-_/  /--'_.' ,'
#  (il),-''  (li),'  ((!.-'
# 
# Author: Weiming Hu <wuh20@psu.edu>
#         Geoinformatics and Earth Observation Laboratory (http://geolab.psu.edu)
#         Department of Geography and Institute for CyberScience
#         The Pennsylvania State University

#' RAnEn::alignObservations
#' 
#' This is a helper function that aligns the observations with the dimension
#' format of a forecast for convenience of statistical analysis.
#' 
#' For example, forecasts are usually 4-dimensional and observations are only
#' 3-dimensional. Forecasts have one extra dimension, FLTs, than observations.
#' This function converts the dimensions of observations to forecasts and 
#' artificially add the FLTs dimension to observations by replicating
#' values in observations.
#' 
#' Imagine the following sequence of observation times
#' 
#'     00h 06h 12h 18h 24h 30h 36h 42h 48h 54h
#'     
#' After the alignment with the daily forecasts with 6 FLTs, the aligned observations
#' will be 
#' 
#'     00h 06h 12h 18h 24h 30h
#'     24h 30h 36h 42h 48h 54h
#'     
#' You note that there will be duplicates in observations which cannot
#' be avoided in this format.
#' 
#' @author Weiming Hu \email{weiming@@psu.edu}
#' 
#' @param observations A 3-dimensional array for observations.
#' @param observation.times The time of each observation. The length of
#' times should equal the thrid dimension of observations.
#' @param forecast.times The initial time for forecasts that you want to align
#' observations with.
#' @param flts The offset time in seconds that you want to align 
#' observations with.
#' @param return.na.index Whether to return the NA index.
#' @param show.progress A logical for whether to show the progress bar.
#' @param silent No printing warning messages.
#' 
#' @return If return.na.index is fault, by default, it retuns the aligned observations;
#' otherwise, it returns a list with the aligned observations and a index matrix for which
#' pair of forecast time/FLT could not be found in observation times provided.
#' 
#' @md
#' @export
alignObservations <- function(observations, observation.times, forecast.times,
                              flts, return.na.index = F, show.progress = T, silent = F) {
  
  stopifnot(length(dim(observations)) == 3)
  stopifnot(dim(observations)[3] == length(observation.times))
  
  # Prepare the containers
  observations.aligned <- array(NA, dim = c(dim(observations)[1:2], length(forecast.times), length(flts)))
  na.pairs <- matrix(nrow = 0, ncol = 2)
  colnames(na.pairs) <- c('forecast-time-index', 'flt-index')
  
  # progress bar
  if (show.progress) {
    pb <- txtProgressBar(min = 0, max = length(forecast.times), style = 3)
    counter <- 1
  }
  
  for (i.forecast.time in 1:length(forecast.times)) {
    for (i.flt in 1:length(flts)) {
      
      # Find out the index for this pair of forecast time and FLT
      index <- which(observation.times == forecast.times[i.forecast.time] + flts[i.flt])
      
      if (length(index) == 1) {
        # If the identified index is valid and the only one
        observations.aligned[, , i.forecast.time, i.flt] <- observations[, , index]
      } else {
        # If the index is not valid or there are multiple ones matched with this pair
        # which should signify something is wrong
        #
        na.pairs <- rbind(na.pairs, c(i.forecast.time, i.flt))
      }
    }
    
    if (show.progress) {
      setTxtProgressBar(pb, counter)
      counter <- counter + 1
    }
  }

  if (show.progress) {
    close(pb)
  }
  
  if (!silent && nrow(na.pairs) != 0) {
    cat("Warning: some forecast times/flts are not found in observation times.\n")
    cat("Use return.na.index to get the indices. This will change the output to a list with 2 members.\n")
  }
  
  if (return.na.index) {
    return (list(index = na.pairs, observations.aligned = observations.aligned))
  } else {
    return (observations.aligned)
  }
}
