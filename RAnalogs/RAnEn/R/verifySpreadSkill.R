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

#' RAnEn::verifySpreadSkill
#' 
#' Underdevelopment.
#' 
#' @md
#' @export
verifySpreadSkill <- function(anen.ver, obs.ver, boot=F, R=1000, intervals = 13, na.rm=T) {
  
  if ( !identical(dim(anen.ver)[1:3], dim(obs.ver)[1:3]) ) {
    print("Error: Observations and Forecasts have incompatible dimensions")
    return(NULL)
  }
  
  # Convert to [stations x days x flt]
  obs       <-  as.vector(obs.ver)
  anen      <-  matrix(anen.ver,ncol=dim(anen.ver)[4])
  
  m     <- ncol(anen) # Number of members
  mdegf <- m / (m+1)
  
  # Compute indexes
  anen.mean <-  rowMeans(anen ,na.rm=na.rm)
  error     <-  ( anen.mean-obs )^2
  spread    <-  rowSums((anen - rowMeans(anen, na.rm = na.rm))^2,
                        na.rm = na.rm) / (dim(anen)[2] - 1)
  
  #anen.all <- matrix(anen.ver, nrow=c(dim(anen.ver)[1] * dim(anen.ver)[2] * dim(anen.ver)[3]) )
  
  # Combine the indeces and remove the NAs
  mat <- data.frame(mean=anen.mean, spread=spread, error=error)
  
  if ( na.rm == T )
    mat <- na.omit( mat )
  
  # Reorder according to the spread
  o   <- order( mat$spread )
  mat <- mat[o, ]
  
  # Compute how many days/flt we compute at once
  step  <- floor ( nrow(mat) / intervals )
  spread.skill.res <- matrix( nrow=intervals, ncol=2)
  
  # Store the results of the bootstrap
  boot.res <-  matrix(NA, ncol=intervals, nrow=3) 
  
  start = 0 
  
  for( i in 1:intervals ){
    end <- start + step
    
    if(boot == T){ #bootstrap confidence intervals only if required
      
      temp <- mat[start:end, 3]
      
      err.boot <- boot.fun.ver(temp, R)
      
      # Normalize the results
      boot.res[, i] <- sqrt( err.boot * mdegf )
    } 
    
    spread.skill.res[i, 1] <- sqrt(mean(mat[start:end, 2]))
    spread.skill.res[i, 2] <- sqrt(mean(mat[start:end, 3]) * mdegf)
    
    start <- end # I think it should be end + 1
  }
  
  R2 <- ( cor(spread.skill.res[,1],spread.skill.res[,2],use="pairwise.complete.obs") )^2
  
  return( list(R2=R2, spread.skill.res=spread.skill.res, boot.res=boot.res) )  
  
}
