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

#' RAnEn::verifyMAE
#' 
#' Underdevelopment.
#' 
#' @export
verifyMAE <- function(anen.ver, obs.ver, boot=F, R=1000, na.rm=T) {
  
  if ( !identical(dim(anen.ver)[1:3], dim(obs.ver)[1:3]) ) {
    print("Error: Observations and Forecasts have incompatible dimensions")
    return(NULL)
  }
  
  obs  <-  matrix(obs.ver,ncol=dim(obs.ver)[3])  
  anen <-  anen.mean(anen.ver, na.rm)
  
  mae <-  abs( (obs - anen) )   
  
  if ( boot == F) {
    mae.tot   <- mean(mae, na.rm=na.rm)
    mae.mean  <- apply( mae, 2, mean, na.rm=na.rm) 
    
    return(list(mean=mae.tot, flt=mae.mean, mat=mae))
  } else {
    
    boot <- apply( mae, 2, boot.fun.ver, R)
    return(list(mean=mean(boot[1,],na.rm=na.rm), flt=boot[1,], mat=boot))  
  }
}
