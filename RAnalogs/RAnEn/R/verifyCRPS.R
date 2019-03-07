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
#' Underdevelopment.
#' 
#' @export
verifyCRPS <- function(anen.ver, obs.ver, boot=F, R=1000, na.rm=T) {
  
  alpha <- function(xt, xhut, xhut.plus.one, na.rm=na.rm) {
    if ( na.rm==T && ( is.na(xt) || is.na(xhut) || is.na(xhut.plus.one)) ) {
      return(0)
    }
    
    if (xt <= xhut) { return( 0 ) }
    if (xt >= xhut && xt <= xhut.plus.one) { return( xt-xhut ) }
    
    return( xhut.plus.one - xhut )
  }
  
  beta <- function(xt, xhut, xhut.plus.one, na.rm=na.rm) {
    if ( na.rm==T && ( is.na(xt) || is.na(xhut) || is.na(xhut.plus.one)) ) {
      return(0)
    }
    
    if (xt <= xhut) { return( xhut.plus.one - xhut ) }
    if (xt >= xhut && xt <= xhut.plus.one) { return( xhut.plus.one-xt ) }
    
    return( 0 )
  }
  
  
  if ( !identical(dim(anen.ver)[1:3], dim(obs.ver)[1:3]) ) {
    print("Error: Observations and Forecasts have incompatible dimensions")
    return(NULL)
  }
  
  
  obs  <-  matrix(obs.ver, ncol=dim(obs.ver)[3])     # [stations*days, FLT]
  anen <-  array(anen.ver, dim=c(dim(anen.ver)[1] * dim(anen.ver)[2], dim(anen.ver)[3], dim(anen.ver)[4]))
  
  n     <- dim(obs)[1] # Number of test days
  flt   <- dim(obs)[2] # Forecast lead times
  m     <- dim(anen)[3] # Number of members
  
  CRPS <- matrix(0, nrow=n, ncol=flt)
  
  # Eq 8.12 page 162
  for ( t in 1:n ) {
    #print(t/n)
    for ( f in 1:flt ) {
      xt   <- obs[t,f]
      
      asum <-  0
      # This is to compute the alpha
      for ( i in 1:m ) {
        xhut <- anen[t,f,i]
        
        if ( i == m ) {
          xhut.plus.one <- Inf 
        } else {
          xhut.plus.one <- anen[t,f,i+1]
        }
        
        a <- alpha(xt, xhut, xhut.plus.one, na.rm=na.rm)
        asum <- asum + a 
      }
      
      bsum <-  0
      # This is to compute the alpha
      for ( i in 0:(m-1) ) {
        xhut.plus.one <- anen[t,f,i+1]
        
        if ( i == 0 ) {
          xhut <- -Inf 
        } else {
          xhut <- anen[t,f,i]
        }
        
        b <- beta(xt, xhut, xhut.plus.one, na.rm=na.rm)
        bsum <- bsum + b 
      }
      
      CRPS[t, f] <- ( asum * (i/m)^2 ) + ( bsum * (1-i/m)^2 )
    }
  }
  
  # Let's compute the average
  
  res <- apply(CRPS,2,mean,na.rm=na.rm)
  
  return(list(mean=mean(res,na.rm=na.rm), flt=res, mat=CRPS))
  
  #return(res)
}
