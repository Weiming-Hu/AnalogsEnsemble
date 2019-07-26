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

#' RAnEn::verifyRankHist
#' 
#' RAnEn::verifyRankHist calculates the rank histogram.
#' 
#' @author Guido Cervone \email{cervone@@psu.edu}
#' @author Martina Calovi \email{mxc895@@psu.edu}
#' @author Laura Clemente-Harding \email{laura@@psu.edu}
#' @author Weiming Hu \email{weiming@@psu.edu}
#' 
#' @param anen.ver A 4-dimensional array. This array is usually created from the `value` column of
#' the `analogs` member in the results of \code{\link{generateAnalogs}}. The dimensions should be
#' `[stations, times, lead times, members]`.
#' @param obs.ver A 3-dimensional array. The dimensions should be `[stations, times, lead times]`.
#' You can generate the array using \code{\link{alignObservations}}.
#' @param show.progress Whether to plot a progress bar.
#' 
#' @md
#' @export
verifyRankHist <- function(anen.ver, obs.ver, show.progress = F) {
  
  obs  <-  as.vector(obs.ver)     # [stations*days, FLT]
  anen <-  matrix(anen.ver, ncol=dim(anen.ver)[4])
  
  # Which are the valid rows?
  valid <- !is.na(obs) & !apply(is.na(anen),1,any)
  
  obs <- obs[valid]
  anen <- anen[valid,]
  
  n     <- dim(anen)[1] # Number of test days
  m     <- dim(anen)[2] # Number of members
  
  res <- rep(0,m+1)
  
  # progress bar
  if (show.progress) {
    pb <- txtProgressBar(min = 0, max = n, style = 3)
    counter <- 1
  }
  
  for (i in 1:n ) {
    v   <- sort(anen[i,])
    
    if( obs[i] == 0 ){
      
      n0 <- length( which(v==0) )
      
      # In case the observation is zero
      if( n0 == 0 ){
        res[1] <- res[1]+1
      }else{
        indices <- seq(n0)
        res[indices] <- res[indices] + 1/n0
      }
    } else {
      
      idx <- which(v <= obs[i]) 
      
      if (length(idx) == 0) {
        res[1] <- res[1]+1
      } else {
        index <- max(idx)+1
        res[index] <- res[index] +1
      }
    } # Observation not zero
    
    if (show.progress) {
      setTxtProgressBar(pb, counter)
      counter <- counter + 1
    }
  } # Loop through the days
  
  if (show.progress) {
    close(pb)
  }
  
  # MRE <- 100 * ( (res[1] + res[m+1] ) / n - 2 / (m+1+1) )
  res <- res / n
  
  return(list(mean=mean(res), rank=res))
}
