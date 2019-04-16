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

#' RAnEn::rmse.ver
#' 
#' @keywords internal
rmse.ver <- function(anen.ver, obs.ver, boot=F, R=1000, na.rm=T) {
  
  if ( !identical(dim(anen.ver)[1:3], dim(obs.ver)[1:3]) ) {
    print("Error: Observations and Forecasts have incompatible dimensions")
    return(NULL)
  }
  
  # dim(obs.ver) [1] 105 362 288
  # dim(anen.ver) [1] 105 362 288  21
  
  obs  <-  matrix(obs.ver,ncol=dim(obs.ver)[3])
  anen <-  anen.mean(anen.ver, na.rm)
  
  err  <-  (anen - obs)^2
  
  if ( boot == F) {
    err.tot   <- sqrt( mean(err, na.rm=na.rm) )
    err.mean  <- sqrt( colMeans(err, na.rm = na.rm) )
    
    return(list(mean=err.tot, flt=err.mean, mat=err))
  } else {
    
    boot <- sqrt( apply( err, 2, boot.fun.ver, R))
    boot.tot <- mean(boot[1,],na.rm=na.rm)
    
    return(list(mean=boot.tot, flt=boot[1,], mat=boot))  
  }
}
