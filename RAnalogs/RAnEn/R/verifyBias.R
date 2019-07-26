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

#' RAnEn::verifyBias
#' 
#' RAnEn::verifyBias calculates bias.
#' 
#' RMSE ^ 2 = CRMSE ^ 2 + Bias ^ 2
#' 
#' To set the number of cores to use when parallel is used,
#' `options(mc.cores = 8)`.
#' 
#' @author Guido Cervone \email{cervone@@psu.edu}
#' @author Martina Calovi \email{mxc895@@psu.edu}
#' @author Laura Clemente-Harding \email{laura@@psu.edu}
#' 
#' @param anen.ver A 4-dimensional array. This array is usually created from the `value` column of
#' the `analogs` member in the results of \code{\link{generateAnalogs}}. The dimensions should be
#' `[stations, times, lead times, members]`.
#' @param obs.ver A 3-dimensional array. The dimensions should be `[stations, times, lead times]`.
#' You can generate the array using \code{\link{alignObservations}}.
#' @param boot Whether to use bootstrap.
#' @param R The number of bootstrap replicates. Used by the function `boot::boot`.
#' @param na.rm Whether to remove NA values.
#' @param parallel Whether to turn on parallel processing.
#' 
#' @md
#' @export
verifyBias <- function(anen.ver, obs.ver, boot=F, R=1000, na.rm=T, parallel = F) {
  
  stopifnot(length(dim(anen.ver)) == 4)
  stopifnot(length(dim(obs.ver)) == 3)
  
  if ( !identical(dim(anen.ver)[1:3], dim(obs.ver)[1:3]) ) {
    cat("Error: Observations and Forecasts have incompatible dimensions.\n")
    return(NULL)
  }
  
  obs  <-  matrix(obs.ver, ncol=dim(obs.ver)[3])     # [stations*days, FLT]
  anen <-  anen.mean(anen.ver, na.rm, parallel = parallel) 
  
  # Compute the difference as a function of FLT between the average ensemble mean and the 
  # corresponding observation for each station and day
  bias <- anen - obs
  
  if ( boot == F) {  
    # The average total bias... perhaps we need it?
    bias.tot <- mean(bias, na.rm=na.rm)
    
    # Compute the mean 
    bias.mean <- colMeans(bias, na.rm = na.rm)
    
    return(list(mean=bias.tot, flt=bias.mean, mat=bias))
  } else {
    boot <- apply( bias, 2, boot.fun.ver, R)
    return( list(mean=mean(boot[1,],na.rm=na.rm), flt=boot[1,], mat=boot) )  
  }
}
