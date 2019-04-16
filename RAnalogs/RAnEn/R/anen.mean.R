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

#' RAnEn::anen.mean
#' 
#' @keywords internal
anen.mean <- function(anen.ver, na.rm=T, fun = mean, parallel = F) {
  anen <-  matrix(anen.ver, ncol=dim(anen.ver)[4])   # [stations*days,FLT, members]
  
  if (parallel) {
    require(parallel)
    anen <- split(anen, seq(nrow(anen)))
    anen <- unlist(mclapply(anen, fun, na.rm=na.rm)) # Compute the mean of the members
    
  } else {
    if (identical(mean, fun)) {
      # This is a mean function
      anen <- rowMeans(anen, na.rm = na.rm)
      
    } else if (identical(var, fun)) {
      # This is a variance function
      anen <- rowSums((anen - rowMeans(anen, na.rm = na.rm))^2,
                      na.rm = na.rm) / (dim(anen)[2] - 1)
      
    } else {
      # Some other function
      anen <-  apply(anen, 1, fun, na.rm=na.rm)        # Compute the mean of the members
    }
    
  }
  
  anen <-  matrix(anen, ncol=dim(anen.ver)[3])       # [stations*days, FLT]
  return(anen)
}
