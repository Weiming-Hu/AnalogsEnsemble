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

#' RAnEn::biasCorrection
#' 
#' @author Weiming Hu \email{weiming@@psu.edu}
#' 
#' Statical bias correction function for analog ensembles.
#' 
#' @param analogs analogs should either be the class of 'AnEn' or a 
#' 5-dimensional array. A variable of the class 'AnEn' is generated
#' from the function \code{\link{compute_analogs}}.
#' 
#' @param observations A 4-dimensional double array that contains 
#' all the observation parameter values. The expected structure is 
#' observations[parameter, station, day, flt] where flt stands for 
#' forecast lead time.
#' 
#' @param forecasts A 4-dimensional double array that contains all
#'  the forecast parameter values. The expected structure is 
#'  forecasts [parameter, station, day, flt] where flt stands for
#'  forecast lead time.
#'
#' @param forecast.ID A positive integer specifying which variable in
#' forecasts are used to compute the bias.
#' 
#' @param test.ID.start A positive integer specifying the start day
#' of tests.
#' 
#' @param group.func A function that summarizes all the member values
#' into one value. For example, it can be 'mean', 'median', 'max', or
#' 'min'.
#' 
#' @param ... Extra parameters to function group.func.
#' 
#' @param keep.bias A logical for whether to keep the bias array
#'
#' @param show.progress A logical for whether to show the progress bar
#' 
#' @return An array or a list depending on whether keep.bias is selected.
#' 
#' @export
biasCorrection <- function(
  analogs = stop("Please set analogs"),
  observations = stop("Please set observations"),
  forecasts = stop("Please set forecasts"),
  forecast.ID = stop("Please set forecast.ID"),
  test.ID.start = stop("Please set test.ID.start"),
  group.func = mean, ..., keep.bias = F, show.progress = T) {
  
  require(RAnEn)
  
  # parameter checks
  if (class(analogs) == 'AnEn') {
    AnEn <- analogs
    analogs <- AnEn$analogs
  } else if (!(is.array(analogs) && 
               is.numeric(analogs) &&
               length(dim(analogs)) == 5)) {
    stop("ERROR: analogs should either be the class 'AnEn' or a 5-dimensional numeric array (please check with functions is.array, is.numeric, and dim).")
  }
  if (!(is.array(forecasts) && 
        is.numeric(forecasts) &&
        length(dim(forecasts)) == 4)) {
    stop("ERROR: forecasts should be a 4-dimensional numeric array! Please use dim(), is.numeric(), and is.array() to check!")
  }
  if (!(is.array(observations) &&
        is.numeric(observations) && 
        length(dim(observations)) == 4)) {
    stop("ERROR: observations should be a 4-dimensional numeric array! Please use dim(), is.numeric(), and is.array() to check!")
  }
  if (dim(forecasts)[2] != dim(observations)[2] || 
      dim(forecasts)[3] != dim(observations)[3] || 
      dim(forecasts)[4] != dim(observations)[4]) {
    stop("ERROR: the second, thrid, and fourth dimensions of forecasts and observations should be the same!")
  }
  if (dim(forecasts)[2] != dim(analogs)[1] ||
      dim(forecasts)[4] != dim(analogs)[3]) {
    stop("ERROR: analogs and forecasts should have the same stations and forecast lead times!")
  }
  if (!(is.numeric(forecast.ID) &&
        length(forecast.ID) == 1 &&
        forecast.ID > 0 &&
        forecast.ID < dim(forecasts)[1])) {
    stop("ERROR: forecast.ID should be a single positive numeric number! And it should not exceed the number of variables in forecasts.")
  }
  if (!(is.numeric(test.ID.start) &&
        length(test.ID.start) == 1 &&
        test.ID.start > 0 &&
        test.ID.start <= dim(forecasts)[3])) {
    stop("ERROR: test.ID.start should be a single positive numeric number! And it should not exceed the number of days in forecasts.")
  }
  if (!is.function(group.func)) {
    stop("ERROR: group.func should be a function.")
  }
  
  # number of members
  members.size <- dim(analogs)[4]
  
  # we will calculate a bias for
  # each station, each test day, and each flt
  #
  bias <- array(NA, dim = dim(analogs)[1:3])
  
  # create an array for bias corrected analogs
  # which has the same shape with analogs
  #
  analogs.cor <- analogs
  
  # progress bar
  if (show.progress) {
    pb <- txtProgressBar(min = 0, max = prod(dim(bias)[c(1, 2, 3)]), style = 3)
    counter <- 1
  }
  
  for (i in 1:dim(bias)[1]) {
    for (j in 1:dim(bias)[2]) {
      for (k in 1:dim(bias)[3]) {
        
        # matrix indexing for members forecast
        index <- cbind(
          rep(forecast.ID, members.size), # variable index
          analogs[i, j, k, , 2],     # station index
          analogs[i, j, k, , 3],     # day index
          rep(k, members.size)            # flt index
        )
        
        members.forecast <- forecasts[index]
        
        # current forecast - mean of selected forecasts
        bias[i, j, k] <- 
          forecasts[forecast.var, i, (j+test.ID.start-1), k] -
          group.func(members.forecast, ...)
        
        analogs.cor[i, j, k, , 1] <-
          analogs.cor[i, j, k, , 1] + bias[i, j, k]
        
        if (show.progress) {
          setTxtProgressBar(pb, counter)
          counter <- counter + 1
        }
      }
    }
  }
  
  if (show.progress) {
    close(pb)
  }
  
  if (keep.bias) {
    return(list(analogs = analogs.cor, bias = bias))
  } else {
    return(analogs.cor)
  }
}