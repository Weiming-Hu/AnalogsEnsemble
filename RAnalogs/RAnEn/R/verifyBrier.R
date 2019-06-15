#  "`-''-/").___..--''"`-._
# (`6_ 6  )   `-.  (     ).`-.__.`)   WE ARE ...
# (_Y_.)'  ._   )  `._ `. ``-..-'    PENN STATE!
#   _ ..`--'_..-_/  /--'_.' ,'
# (il),-''  (li),'  ((!.-'
#
#
# Author: Guido Cervone (cervone@psu.edu), Martina Calovi (mxc895@psu.edu), Laura Clemente-Harding (laura@psu.edu)
#         Geoinformatics and Earth Observation Laboratory (http://geolab.psu.edu)
#         Department of Geography and Institute for CyberScience
#         The Pennsylvania State University
#

#' RAnEn::verifyBrier
#' 
#' Underdevelopment.
#' 
#' @export
verifyBrier <- function(anen.ver, obs.ver, threshold, ensemble.func, ..., baseline = NULL, ) {
  
  require(verification)
  
  if ( !identical(dim(anen.ver)[1:3], dim(obs.ver)[1:3]) ) {
    print("Error: Observations and Forecasts have incompatible dimensions")
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
  
  # Convert each AnEn ensemble to a probability using the ensemble.func adn the threshold
  anen.ver <- apply(anen.ver, 1:3, ensemble.func)
  anen.ver <- anen.ver >= threshold
  
  # Convert observations to a binary variable using the threshold
  obs.ver <- obs.ver >= threshold
  
  num.flts <- dim(obs.ver)[3]
  ret.flts <- data.frame()
  
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
  
  names(ret.flts) <- c('bs', 'ss', 'reliability', 'resol')
  rownames(ret.flts) <- c(paste('FLT', 1:num.flts, sep = ''), 'All')
  
  return(ret.flts)
}