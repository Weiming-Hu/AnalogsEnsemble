#  "`-''-/").___..--''"`-._
# (`6_ 6  )   `-.  (     ).`-.__.`)   WE ARE ...
# (_Y_.)'  ._   )  `._ `. ``-..-'    PENN STATE!
#   _ ..`--'_..-_/  /--'_.' ,'
# (il),-''  (li),'  ((!.-'
#
#
# Author: Weiming Hu (weiming@psu.edu)
#         Geoinformatics and Earth Observation Laboratory (http://geolab.psu.edu)
#         Department of Geography and Institute for CyberScience
#         The Pennsylvania State University
#

#' RAnEn::verifyBrier
#' 
#' RAnEn::verifyBrier computes the Brier score and its decomposition for each 
#' all lead times available. This function used the NCAR verification package.
#' 
#' @param anen.ver A 4-dimensional array for analogs.
#' @param obs.ver A 3-dimensional array for observations.
#' @param threshold The numeric threshold for computing the brier score.
#' Observation and baseline values larger than or equal to the threshold
#' will be converted to 1. Analog values will not be processed with this
#' threshold value because it is assumed that the ensemble function will
#' convert analog ensemble to probability values.
#' @param ensemble.func A function to convert the ensemble members (the 4th
#' dimension of analogs) into a scala. This scala is usually a probability.
#' @param ... Extra parameters for the ensemble.func.
#' @param baseline A 3-dimensional array for the baseline forecasts.
#' 
#' @export
verifyBrier <- function(anen.ver, obs.ver, threshold, ensemble.func, ..., baseline = NULL) {
  
  require(verification)
  
  if ( !identical(dim(anen.ver)[1:3], dim(obs.ver)[1:3]) ) {
    print("Error: Observations and analogs have incompatible dimensions")
    return(NULL)
  }
  
  stopifnot(is.function(ensemble.func))
  
  if (!identical(baseline, NULL)) {
    if ( !identical(dim(baseline), dim(obs.ver))) {
      print("Error: Observations and baseline forecasts have incompatible dimensions")
      return(NULL)
    }
    
    baseline <- baseline >= threshold
  }
  
  # Convert each AnEn ensemble to a probability using the ensemble.func
  anen.ver <- apply(anen.ver, 1:3, ensemble.func, ...)
  
  # Convert observations to a binary variable using the threshold
  obs.ver <- obs.ver >= threshold
  
  num.flts <- dim(obs.ver)[3]
  ret.flts <- matrix(nrow = 0, ncol = 4)
  
  for (i.flt in 1:num.flts) {
    
    ret.flt <- brier(obs = obs.ver[, , i.flt], pred = anen.ver[, , i.flt],
                     baseline = baseline[, , i.flt], bins = F)
    stopifnot(all(ret.flt$check - ret.flt$bs < 1e-6))
    
    ret.flts <- rbind(ret.flts, c(
      ret.flt$bs, ret.flt$ss, ret.flt$bs.reliability, ret.flt$bs.resol))
  }
  
  # Compute the overall brier score for all lead times
  ret.flt <- brier(obs = obs.ver, pred = anen.ver, baseline = baseline, bins = F)
  stopifnot(all(ret.flt$check - ret.flt$bs < 1e-6))
  
  ret.flts <- rbind(ret.flts, c(
    ret.flt$bs, ret.flt$ss, ret.flt$bs.reliability, ret.flt$bs.resol))
  
  colnames(ret.flts) <- c('bs', 'ss', 'reliability', 'resol')
  rownames(ret.flts) <- c(paste('FLT', 1:num.flts, sep = ''), 'All')
  
  return(ret.flts)
}
