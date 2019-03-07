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
#' Underdevelopment.
#' 
#' @export
verifyRankHist <- function(anen.ver, obs.ver){ 
  
  obs  <-  as.vector(obs.ver)     # [stations*days, FLT]
  anen <-  matrix(anen.ver, ncol=dim(anen.ver)[4])
  
  # Which are the valid rows?
  valid <- !is.na(obs) & !apply(is.na(anen),1,any)
  
  obs <- obs[valid]
  anen <- anen[valid,]
  
  n     <- dim(anen)[1] # Number of test days
  m     <- dim(anen)[2] # Number of members
  
  res <- rep(0,m+1)
  
  for (i in 1:n ) {
    v   <- sort(anen[i,])
    
    if( obs[i] == 0 ){
      
      n0 <- length( which(v==0) )
      
      # In case the observation is zero
      if( n0 == 0 ){
        res[1]=res[1]+1
      }else{
        for( iii in seq(n0) ) {
          res[iii] <- res[iii]+1 / n0
        }
      }
    } else {
      
      idx <- which(v <= obs[i]) 
      
      if (length(idx) == 0) {
        res[1] <- res[1]+1
      } else {
        res[max(idx)+1] = res[max(idx)+1] +1
      }
    } # Observation not zero
  } # Loop through the days
  
  MRE <- 100 * ( (res[1] + res[m+1] ) / n - 2 / (m+1+1) )
  res <- res / n
  
  return(list(mean=mean(res), rank=res))
}