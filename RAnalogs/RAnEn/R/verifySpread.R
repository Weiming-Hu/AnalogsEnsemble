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
#' Underdevelopment.
#' 
#' @export
verifySpread <- function(anen.ver, na.rm=T) { 
  
  spread.ver <-  anen.mean(anen.ver, na.rm, fun=var)
  
  err.mean  <- sqrt( apply( spread.ver, 2, mean, na.rm=na.rm) )
  err.tot   <- mean(err.mean, na.rm=na.rm)  
  
  return(list(mean=err.tot, flt=err.mean, mat=spread.ver))
}