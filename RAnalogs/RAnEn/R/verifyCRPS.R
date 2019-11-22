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

#' RAnEn::verifyCRPS
#' 
#' RAnEn::verifyCRPS calculates CRPS.
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
#' @param int.step When boot is used, this is the integral length.
#' @param na.rm Whether to remove NA values.
#' 
#' @export
verifyCRPS <- function(anen.ver, obs.ver, boot = F, R = 1000, int.step = 150, na.rm = T) {
  
  stopifnot(length(dim(anen.ver)) == 4)
  stopifnot(length(dim(obs.ver)) == 3)
  
  if ( !identical(dim(anen.ver)[1:3], dim(obs.ver)[1:3]) ) {
    cat("Error: Observations and Forecasts have incompatible dimensions.\n")
    return(NULL)
  } 
  
  n     <- dim(obs.ver)[2] # Number of test days
  flt   <- dim(obs.ver)[3] # Forecast lead times
  m     <- dim(anen.ver)[4] # Number of members
  
  #
  #forecasts=anen.ver
  #measurements=obs.ver
  
  # Reshape FLT x days x stations
  # Add an index to each row so that we know to which flt it corresponds to.  This is needed later when 
  # we remove NAs to keep track of which flt a value is referring to
  obs  <-  as.vector(aperm(obs.ver,c(3,2,1)))
  obs  <-  cbind(1:dim(obs.ver)[3],obs)
  
  # Reshape the analogs to [FLT x days x stations] x analogs
  anen <-  matrix(aperm(anen.ver, c(3,2,1,4)), ncol=dim(anen.ver)[4])
  
  # Check if we should remove NAs.  This is likely to must always be true
  if ( na.rm==T ) {
    # Determine the NAs in either obs or forecasts
    obs.na  <- is.na(obs[,2])
    anen.na <- apply(is.na(anen), 1, any)
    
    # These are the values where there are no nas either in observations or forecasts
    valid <- !obs.na & !anen.na
    obs   <- obs[valid,]
    anen  <- anen[valid,]
  }
  
  
  if ( boot == F) {
    stopifnot(requireNamespace(verification, quietly = T))

    # Run the CRPS
    deco     <- crpsDecomposition(obs[,2], anen)
    crps     <- data.frame(crps=deco$CRPS,spot=deco$CRPSpot,reli=deco$Reli)
    
    # Now let's do it by FLT
    crps.flt <- matrix(NA, ncol=flt, nrow=3)
    row.names(crps.flt) <- c("crps","spot","reli")
    
    # Now by FLT
    for(i in 1:flt ) {
      
      # The values at this FLT
      valid <- which(obs[,1] == i)
      
      if(length(valid) > 1){
        deco         <- crpsDecomposition(obs[valid,2], anen[valid,])
        crps.flt[,i] <- c(deco$CRPS,deco$CRPSpot,deco$Reli)
      }
    }
    
    return(list(stats=crps,crps.flt=crps.flt))
    
  } else {
    # Everything underneat here is for the bootstrapping
    
    #defining the step for computing the integral of CRPS
    soglia    <- abs( max(obs[,2]) - min(obs[,2]) ) / int.step 
    minsoglia <- min( obs[,2] )
    maxsoglia <- max( obs[,2] ) + soglia
    
    # These are all the integration steps
    soglia.seq <- seq(minsoglia, maxsoglia, soglia)
    
    
    ########################
    ########################
    # The integration matrix   
    #casi    <- matrix( nrow=nrow(anen), ncol=length(soglia.seq) )
    
    # return which analogs (x) are less than the threshols (soglia)
    fun.1 <- function( x, ts ) {
      fun.2 <- function(t, x) {
        return(length( which(x <= t)))
      }      
      return(sapply(ts, fun.2, x))
    }
    
    # Generate all the cases 
    integrations <- t(apply(anen, 1, fun.1, soglia.seq)) / m
    
    
    
    ########################
    ########################
    # Compute the integration
    crps <- rep(NA, nrow(anen))
    
    for(i in 1:length(crps)) {
      
      # Identify the closest index of the integration steps associated with the current observation
      indice <- which.min( abs( soglia.seq - obs[i,2]) )
      
      # This is the integration to the left of the observation
      
      indexes.lower <- 1:indice
      indexes.upper <- (indice+1):(ncol(integrations))  # Originally in Stefano's code the upper limit was maxsoglia / soglia
      
      crps1 <- sum( integrations[i,indexes.lower]^2 * soglia )
      crps2 <- sum( (integrations[i,indexes.upper]-1)^2 * soglia ) 
      
      crps[i] <- crps1 + crps2
    }
    
    
    ########################      
    ########################      
    # Book for all the values
    crps.boot <- boot.fun.ver( crps, R)
    
    
    ########################
    ########################      
    # Repeat the bootstrap by times
    crps.boot.flt <- matrix(NA, ncol=flt, nrow=3)
    
    for(i in 1:flt) {
      anen.t <- crps[ which(obs[,1] == i) ]
      
      if(length(anen.t) > 0){
        crps.boot.flt[,i] <- boot.fun.ver( anen.t, R)
      }
    }
    
    crps.output=list(crps=mean(crps), 
                     crps.boot=crps.boot, 
                     crps.boot.flt=crps.boot.flt)
    return(crps.output) 
    
  }
}


# The deprecated version
# verifyCRPS <- function(anen.ver, obs.ver, boot=F, R=1000, na.rm=T) {
#   
#   alpha <- function(xt, xhut, xhut.plus.one, na.rm=na.rm) {
#     if ( na.rm==T && ( is.na(xt) || is.na(xhut) || is.na(xhut.plus.one)) ) {
#       return(0)
#     }
#     
#     if (xt <= xhut) { return( 0 ) }
#     if (xt >= xhut && xt <= xhut.plus.one) { return( xt-xhut ) }
#     
#     return( xhut.plus.one - xhut )
#   }
#   
#   beta <- function(xt, xhut, xhut.plus.one, na.rm=na.rm) {
#     if ( na.rm==T && ( is.na(xt) || is.na(xhut) || is.na(xhut.plus.one)) ) {
#       return(0)
#     }
#     
#     if (xt <= xhut) { return( xhut.plus.one - xhut ) }
#     if (xt >= xhut && xt <= xhut.plus.one) { return( xhut.plus.one-xt ) }
#     
#     return( 0 )
#   }
#   
#   
#   if ( !identical(dim(anen.ver)[1:3], dim(obs.ver)[1:3]) ) {
#     print("Error: Observations and Forecasts have incompatible dimensions")
#     return(NULL)
#   }
#   
#   
#   obs  <-  matrix(obs.ver, ncol=dim(obs.ver)[3])     # [stations*days, FLT]
#   anen <-  array(anen.ver, dim=c(dim(anen.ver)[1] * dim(anen.ver)[2], dim(anen.ver)[3], dim(anen.ver)[4]))
#   
#   n     <- dim(obs)[1] # Number of test days
#   flt   <- dim(obs)[2] # Forecast lead times
#   m     <- dim(anen)[3] # Number of members
#   
#   CRPS <- matrix(0, nrow=n, ncol=flt)
#   
#   # Eq 8.12 page 162
#   for ( t in 1:n ) {
#     #print(t/n)
#     for ( f in 1:flt ) {
#       xt   <- obs[t,f]
#       
#       asum <-  0
#       # This is to compute the alpha
#       for ( i in 1:m ) {
#         xhut <- anen[t,f,i]
#         
#         if ( i == m ) {
#           xhut.plus.one <- Inf 
#         } else {
#           xhut.plus.one <- anen[t,f,i+1]
#         }
#         
#         a <- alpha(xt, xhut, xhut.plus.one, na.rm=na.rm)
#         asum <- asum + a 
#       }
#       
#       bsum <-  0
#       # This is to compute the alpha
#       for ( i in 0:(m-1) ) {
#         xhut.plus.one <- anen[t,f,i+1]
#         
#         if ( i == 0 ) {
#           xhut <- -Inf 
#         } else {
#           xhut <- anen[t,f,i]
#         }
#         
#         b <- beta(xt, xhut, xhut.plus.one, na.rm=na.rm)
#         bsum <- bsum + b 
#       }
#       
#       CRPS[t, f] <- ( asum * (i/m)^2 ) + ( bsum * (1-i/m)^2 )
#     }
#   }
#   
#   # Let's compute the average
#   
#   res <- colMeans(CRPS, na.rm=na.rm)
#   
#   return(list(mean=mean(res,na.rm=na.rm), flt=res, mat=CRPS))
#   
#   #return(res)
# }
