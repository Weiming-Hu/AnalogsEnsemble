# "`-''-/").___..--''"`-._
#  (`6_ 6  )   `-.  (     ).`-.__.`)   WE ARE ...
#  (_Y_.)'  ._   )  `._ `. ``-..-'    PENN STATE!
#    _ ..`--'_..-_/  /--'_.' ,'
#  (il),-''  (li),'  ((!.-'
# 
# Author: Weiming Hu <weiming@psu.edu>
#         Geoinformatics and Earth Observation Laboratory (http://geolab.psu.edu)
#         Department of Geography and Institute for CyberScience
#         The Pennsylvania State University
#

#' RAnEn::verifyCRMSE
#' 
#' RAnEn::verfiyCRMSE computes the center RMSE.
#' 
#' RMSE ^ 2 = CRMSE ^ 2 + Bias ^ 2
#' 
#' To set the number of cores to use when parallel is used,
#' `options(mc.cores = 8)`.
#' @param boot Whether to use bootstrap.
#' @param R The number of bootstrap replicates. Used by the function `boot::boot`.
#' @param na.rm Whether to remove NA values.
#' @param parallel Whether to use parallel processing.
#' 
#' @md
#' @export
verifyCRMSE <- function(anen.ver, obs.ver, boot=F, R=1000, na.rm=T, parallel = F) {
  
  stopifnot(length(dim(anen.ver)) == 4)
  stopifnot(length(dim(obs.ver)) == 3)
  
  if ( !identical(dim(anen.ver)[1:3], dim(obs.ver)[1:3]) ) {
    cat("Error: Observations and Forecasts have incompatible dimensions.\n")
    return(NULL)
  }
  
  # dim(obs.ver) [1] 105 362 288
  # dim(anen.ver) [1] 105 362 288  21
  
  obs  <-  matrix(obs.ver,ncol=dim(obs.ver)[3])
  anen <-  anen.mean(anen.ver, na.rm, parallel = parallel)
  
  err <- anen - obs
  bias <- colMeans(err, na.rm = na.rm)
  err <- err^2
  diff <- err - matrix(bias^2, nrow = nrow(err), ncol = length(bias), byrow = T)
  
  if ( boot == F) {
    err.tot   <- sqrt( mean(err - mean(bias, na.rm = na.rm)^2, na.rm=na.rm) )
    err.mean  <- sqrt( apply( diff, 2, mean, na.rm=na.rm) )
    
    return(list(mean=err.tot, flt=err.mean, mat=err))
  } else {
    
    boot <- sqrt( apply( diff, 2, boot.fun.ver, R))
    boot.tot <- mean(boot[1,],na.rm=na.rm)
    
    return(list(mean=boot.tot, flt=boot[1,], mat=boot))  
  }
}
