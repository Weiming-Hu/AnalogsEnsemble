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

#' RAnEn::sortStations
#' 
#' RAnEn::sortStations sorts the data based on either 'Xs', 'Ys', or 'StationNames'.
#' 
#' @param obj A Forecasts or Observations
#' @param sort.stations Either one of `Xs`, `Ys`, or `StationNames`, or a numeric
#' vector for the order. For example, if a numeric vector is passed, like `c(1, 4, 2, 3)`,
#' the stations and the data will be ordered in such a way that: the first station is not
#' moved, the second station is moved to the third position, the third station is moved
#' to the forth position, and the forth station is moved to the second position. And
#' there are only four stations!
#' 
#' @md
#' @export
sortStations <- function(obj, sort.stations=NULL, verbose=F) {
  
  stopifnot(inherits(obj, 'Forecasts') | inherits(obj, 'Observations'))
  
  # Sort stations if needed
  if (identical(sort.stations, NULL)) {
    if (verbose) {
      cat("Stations are not sorted. Use sort.stations to sort them.\n")
      cat("Tips: Make sure station orders are consistent between forecasts and observations when generating analogs.\n")
    }
    
  } else {
    
    if (identical(sort.stations, 'Xs')) {
      if (verbose) {
        cat("Sort data by Xs ...\n")
      }
      
      index <- order(obj$Xs)
      
    } else if (identical(sort.stations, 'Ys')) {
      if (verbose) {
        cat("Sort data by Ys ...\n")
      }
      
      index <- order(obj$Ys)
      
    } else if (identical(sort.stations, 'StationNames')) {
      
      if (is.null(obj$StationNames)) {
        warning("Station names are missing. Data are not sorted.")
        index <- NULL
        
      } else {
        if (verbose) {
          cat("Sort data by station names ...\n")
        }
        
        index <- order(obj$StationNames)
      }
      
    } else if (is.numeric(sort.stations)) {
      
      # Make sure the correct number of indices are provided
      stopifnot(length(sort.stations) == dim(obj$Data)[2])
      stopifnot(max(sort.stations) <= dim(obj$Data)[2])
      stopifnot(min(sort.stations) >= 1)
      stopifnot(length(unique(sort.stations)) == length(sort.stations))
      
      index <- sort.stations
      
    } else {
      warning("Sorting tag unsupported. Data are not sorted.")
      index <- NULL
    }
    
    if (!identical(index, NULL)) {
      
      obj$Xs <- obj$Xs[index]
      obj$Ys <- obj$Ys[index]
      obj$StationNames <- obj$StationNames[index]
      
      if (inherits(obj, 'Observations')) {
        obj$Data <- obj$Data[, index, , drop = F]
      } else if (inherits(obj, 'Forecasts')) {
        obj$Data <- obj$Data[, index, , , drop = F]
      } else {
        stop('This is fatal. This error should be catched! Please help me report this. Thanks.')
      }
    }
  }
  
  return(obj) 
}