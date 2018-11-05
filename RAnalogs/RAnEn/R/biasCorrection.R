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
#' @param AnEn An AnEn object.
#' 
#' @param config The configuration object used to generate the AnEn results.
#' 
#' @param forecast.ID Which parameter in the forecasts should be used to generate bias.
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
  AnEn, config, forecast.ID, group.func = mean, ..., keep.bias = F, show.progress = T) {
  
  require(RAnEn)
  
  # parameter checks
  if (class(AnEn) != 'AnEn') {
    stop("AnEn should be an AnEn object.")
  }
  if (config$preserve_real_time) {
    stop("Please don't use the option preserve_real_time in configuration when generating AnEn. This slows down the bias correction significantly.")
  }
  if (!config$preserve_mapping) {
    stop("Please use the option preserve_mapping in configuration when generating AnEn. If not, it slows down the bias correction significantly.")
  }
  if (class(config) != 'Configuration') {
    stop("config should be a configuration object.")
  }
  if (!is.function(group.func)) {
    stop("group.func should be a function.")
  }
  
  # number of members
  members.size <- config$num_members
  
  # we will calculate a bias for
  # each station, each test day, and each flt
  #
  bias <- array(NA, dim = dim(AnEn$analogs)[1:3])
  
  # create an array for bias corrected analogs
  # which has the same shape with analogs
  #
  analogs.cor <- AnEn$analogs
  
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
          rep(forecast.ID, members.size),                                          # variable index
          AnEn$analogs[i, j, k, , 2],                                              # station index
          unlist(lapply(AnEn$analogs[i, j, k, , 3], function(x, mapping, k) {
            return(which(x == mapping[k, ]))}, mapping = AnEn$mapping, k = k)),    # forecast day index
          rep(k, members.size)                                                     # flt index
        )
        
        members.forecast <- config$search_forecasts[index]
        
        # current forecast - mean of selected forecasts
        bias[i, j, k] <- 
          config$test_forecasts[forecast.ID, i, j, k] -
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
  
  AnEn$analogs.cor <- analogs.cor
  
  if (keep.bias) {
    AnEn$bias <- bias
  }
  
  return(AnEn)
}