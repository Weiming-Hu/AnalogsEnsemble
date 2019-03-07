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

#' RAnEn::biasCorrectionInsitu
#' 
#' Statical bias correction function for analog ensembles. This method uses the bias of the 
#' forecast model to correct AnEn, therefore the word 'in situ'.
#' 
#' For a given forecast from AnEn for a certain station/grid point, day, and FLT, first,
#' the mean of the selected forecasts, also the most similar past forecasts, is calculated;
#' then the difference of the mean of the selected forecasts and the current forecast is calculated.
#' This difference is then added to the AnEn forecasts.
#' 
#' This method only works when the vairable forecasted by AnEn also exists in the model forecasts.
#' For example, temperature is a model forecast variable. So we can bias correct AnEn forecasts for
#' temperature using the model forecasts. If the AnEn forecast variable does not exist in the model,
#' this method is not applicable.
#' 
#' @author Weiming Hu \email{weiming@@psu.edu}
#' 
#' @param AnEn An AnEn object.
#' 
#' @param config The configuration object used to generate the AnEn results.
#' @param forecast.ID Which parameter in the forecasts should be used to generate bias.
#' @param group.func A function that summarizes all the member values
#' into one value. For example, it can be 'mean', 'median', 'max', or 'min'.
#' 
#' @param ... Extra parameters to function group.func.
#' @param keep.bias A logical for whether to keep the bias array
#' @param show.progress A logical for whether to show the progress bar
#' @param overwrite A logical for whether to overwrite correction results.
#' @param density.limit The probability density threshold. If the density of the bin including
#' the current forecast is smaller than or equals to this value, the current forecast will be bias
#' corrected. This parameter is useful when only a portion of the test
#' forecasts need to be bias corrected.
#' 
#' @return An AnEn object.
#' 
#' @export
biasCorrectionInsitu <- function(
  AnEn, config, forecast.ID, group.func = mean, ...,
  keep.bias = F, show.progress = T, overwrite = F, density.limit = 1) {
  
  config <- convertToAdvance(config)
  
  # Check for overwriting
  if ('analogs.cor.insitu' %in% names(AnEn) ||
      'bias.insitu' %in% names(AnEn)) {
    if (!overwrite) {
      stop('Corrected (in situ) AnEn already exists. Use overwrite = T to orverwrite them.')
    }
  }
  
  # parameter checks
  if (class(AnEn) != 'AnEn') {
    stop("AnEn should be an AnEn object.")
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
  bc.counter <- 0
  
  # Extract the index of search forecast days
  index.search.fcst.days <- sapply(config$search_times_compare, function (t) {
    return(which(t == config$search_times))
  })
  
  # progress bar
  if (show.progress) {
    pb <- txtProgressBar(min = 0, max = prod(dim(bias)[c(1, 2, 3)]), style = 3)
    pb.counter <- 1
  }
  
  for (i in 1:dim(bias)[1]) {
    for (k in 1:dim(bias)[3]) {
      
      # Build a histogram based on all search forecasts for station i and FLT k
      hist <- hist(config$search_forecasts[forecast.ID, i, index.search.fcst.days, k], plot = F)
      
      for (j in 1:dim(bias)[2]) {
        
        if (all(is.na(AnEn$analogs[i, j, k, , 1]))) {
          # If this ensemble only contains NA values, skip the bias calculation
          if (show.progress) {
            setTxtProgressBar(pb, pb.counter)
            pb.counter <- pb.counter + 1
          }
          next
        }
        
        # Get the current forecast
        index.current.test.day <- config$test_times_compare[j] == config$test_times
        current.fcst <- config$test_forecasts[forecast.ID, i, which(index.current.test.day), k]
        
        if (is.na(current.fcst)) {
          if (show.progress) {
            setTxtProgressBar(pb, pb.counter)
            pb.counter <- pb.counter + 1
          }
          next
        }
        
        # Get the density of the bin where the current forecast is included.
        diff <- abs(hist$mids - current.fcst)
        density.current.bin <- hist$density[which(diff == min(diff))[1]]
        
        if (density.current.bin > density.limit) {
          if (show.progress) {
            setTxtProgressBar(pb, pb.counter)
            pb.counter <- pb.counter + 1
          }
          next
        }
        
        # matrix indexing for members forecast
        index <- cbind(
          rep(forecast.ID, members.size),                                          # variable index
          AnEn$analogs[i, j, k, , 2],                                              # station index
          unlist(lapply(AnEn$analogs[i, j, k, , 3], function(x, cont) {
            return(which(x == cont))}, cont = AnEn$mapping[k, ])),                 # forecast day index
          rep(k, members.size)                                                     # flt index
        )
        
        if (nrow(index) != members.size || ncol(index) != 4) {
          stop("Something went wrong inside the AnEn computation. Please report to the developer.")
        } else {
          members.forecast <- config$search_forecasts[index]
          
          # current forecast - mean of selected forecasts
          bias[i, j, k] <- current.fcst - group.func(members.forecast, ...)
          
          analogs.cor[i, j, k, , 1] <- analogs.cor[i, j, k, , 1] + bias[i, j, k]  
          bc.counter <- bc.counter + 1
        }
        
        if (show.progress) {
          setTxtProgressBar(pb, pb.counter)
          pb.counter <- pb.counter + 1
        }
      }
    }
  }
  
  if (show.progress) {
    close(pb)
    cat("RAnEn::biasCorrectionInsitu:", bc.counter, 
        paste("(", round(bc.counter / prod(dim(bias)[1:3]), 1) * 100, "%)", sep = ''),
        "test forecasts have been corrected.\n")
  }
  
  AnEn$analogs.cor.insitu <- analogs.cor
  
  if (keep.bias) {
    AnEn$bias.insitu <- bias
  }
  
  return(AnEn)
}
