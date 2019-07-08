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

#' RAnEn::verifyThreatScore
#' 
#' RAnEn::verifyThreatScore computes the threat score for a specific threshold.
#' The formula is adopted from
#' [WPC Verification page](https://www.wpc.ncep.noaa.gov/html/scorcomp.shtml).
#' 
#' @param anen.ver A 4-dimensional array for analogs.
#' @param obs.ver A 3-dimensional array for observations.
#' @param threshold The numeric threshold for computing the threat score.
#' Observation values larger than or equal to the threshold will be
#' converted to 1. Analog values will not be compared to this threshold value because
#' **it is assumed that the ensemble function will convert analog ensemble to binary values**.
#' @param ensemble.func A function to convert the ensemble members (the 4th
#' dimension of analogs) into **a binary**. For more datails on this parameter,
#' please see \code{\link{verifyBrier}}.
#' @param ... Extra parameters for the ensemble.func.
#' 
#' @md
#' @export
verifyThreatScore <- function(anen.ver, obs.ver, threshold, ensemble.func, ...) {
  
  if ( !identical(dim(anen.ver)[1:3], dim(obs.ver)[1:3]) ) {
    cat("Error: Observations and forecasts have incompatible dimensions.\n")
    return(NULL)
  }
  
  # Get the number of lead times
  num.flts <- dim(obs.ver)[3]
  
  # Convert each AnEn ensemble to a binary variable 
  anen.ver <- apply(anen.ver, 1:3, ensemble.func, ...)
  
  # Convert observations to a binary variable using the threshold
  obs.ver <- obs.ver >= threshold
  
  ret.flts <- matrix(nrow = 0, ncol = 4)
  
  for (i.flt in 1:num.flts) {
    
    # Convert AnEn and observations to logical vectors
    anen.ver.flt <- as.logical(as.vector(anen.ver[, , i.flt]))
    obs.ver.flt <- as.logical(as.vector(obs.ver[, , i.flt]))
    
    correct <- length(which(anen.ver.flt & obs.ver.flt))
    forecasted <- length(which(anen.ver.flt))
    observed <- length(which(obs.ver.flt))
    
    ts <- correct / (forecasted + observed - correct)
    ret.flts <- rbind(ret.flts, c(ts, correct, forecasted, observed))
  }
  
  colnames(ret.flts) <- c('ts', 'correct', 'forecasted', 'observed')
  rownames(ret.flts) <- paste('FLT', 1:num.flts, sep = '')
  
  return(ret.flts)
}
