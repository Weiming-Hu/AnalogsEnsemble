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

#' RAnEn::verifyCorrelation
#' 
#' RAnEn::verifyCorrelation calculates correlation.
#' 
#' @author Guido Cervone \email{cervone@@psu.edu}
#' @author Martina Calovi \email{mxc895@@psu.edu}
#' @author Laura Clemente-Harding \email{laura@@psu.edu}
#' 
#' To set the number of cores to use when parallel is used,
#' `options(mc.cores = 8)`.
#' 
#' @param anen.ver A 4-dimensional array. This array is usually created from the `value` column of
#' the `analogs` member in the results of \code{\link{generateAnalogs}}. The dimensions should be
#' `[stations, times, lead times, members]`.
#' @param obs.ver A 3-dimensional array. The dimensions should be `[stations, times, lead times]`.
#' You can generate the array using \code{\link{alignObservations}}.
#' @param boot Whether to use bootstrap.
#' @param R The number of bootstrap replicates. Used by the function `boot::boot`.
#' @param na.rm Whether to remove NA values.
#' @param parallel Whether to use parallel processing.
#' 
#' @md
#' @export
verifyCorrelation <- function(anen.ver, obs.ver, boot=F, R=1000, na.rm=T, parallel = F) {
  
  if ( !identical(dim(anen.ver)[1:3], dim(obs.ver)[1:3]) ) {
    print("Error: Observations and Forecasts have incompatible dimensions")
    return(NULL)
  }
  
  obs  <-  matrix(obs.ver, ncol=dim(obs.ver)[3])     # [stations*days, FLT]
  anen <-  anen.mean(anen.ver, na.rm, parallel = parallel) 
  
  if ( boot == F) {
    # Compute the difference as a function of FLT between the average ensemble mean and the 
    # corresponding observation for each station and day
    c <- cor(obs,anen,use="pairwise.complete.obs")
    
    # Extract the diagonal of the correlation matrix
    c.diag <- diag(c)
    
    # The average total bias... perhaps we need it?
    # c.tot <- mean(c, na.rm=na.rm)
    
    # Compute the mean 
    c.mean <- mean(c.diag, na.rm=na.rm)
    
    return(list(mean=c.mean, flt=c.diag, mat=c))
    
  } else {
    corr.fun=function(d,i){ 
      cor(d[i,1],d[i,2],use="pairwise.complete.obs")
    }
    
    boot.res = matrix(NA, ncol=ncol(obs),nrow=3) 
    
    for(i in 1:ncol(obs) ) {    
      temp <- data.frame(obs=obs[,i], anen=anen[,i])
      boot.res[, i] <- boot.fun.ver(temp, R, fun=corr.fun)
    }
    
    return(list(mean=mean(boot.res[1,],na.rm=na.rm), flt=boot.res[1,], mat=boot.res))  
  }
}
