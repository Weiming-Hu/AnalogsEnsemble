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

#' RAnEn::verifySpread
#' 
#' RAnEn::verifySpread computes the spread.
#' 
#' Use the following command to set the number of cores to use
#' when parallel is used,
#' `options(mc.cores = 8)`.
#' 
#' @author Guido Cervone \email{cervone@@psu.edu}
#' @author Martina Calovi \email{mxc895@@psu.edu}
#' @author Laura Clemente-Harding \email{laura@@psu.edu}
#' 
#' @param anen.ver A 4-dimensional array. This array is usually created from the `value` column of
#' the `analogs` member in the results of \code{\link{generateAnalogs}}. The dimensions should be
#' `[stations, times, lead times, members]`.
#' @param na.rm Whether to remove NA values.
#' @param parallel Whether to turn on paralle processing.
#' 
#' @md
#' @export
verifySpread <- function(anen.ver, na.rm = T, parallel = F) { 
  
  spread.ver <-  anen.mean(anen.ver, na.rm, fun=var, parallel = parallel)
  
  err.mean  <- sqrt( apply( spread.ver, 2, mean, na.rm=na.rm) )
  err.tot   <- mean(err.mean, na.rm=na.rm)  
  
  return(list(mean=err.tot, flt=err.mean, mat=spread.ver))
}
