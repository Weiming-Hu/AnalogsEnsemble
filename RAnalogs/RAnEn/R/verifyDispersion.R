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

#' RAnEn::verifyDispersion
#' 
#' Underdevelopment.
#' 
#' @export
verifyDispersion <- function(anen.ver, obs.ver, boot=F, R=1000, na.rm=T) {
  
  rmse <- rmse.ver(anen.ver, obs.ver, boot, R, na.rm)
  spread <- spread.ver( anen.ver, na.rm)
  
  return(list(mean=rmse$mean, flt=rbind( rmse$flt, spread$flt), mat=rmse$mat) )
}
