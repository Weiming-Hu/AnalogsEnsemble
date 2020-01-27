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

#' RAnEn::generateConfiguration
#' 
#' RAnEn::generateConfiguration generates a templated configuration object to
#' be passed to the function \code{\link{generateAnalogs}}.
#' This object can be validated before use by the function
#' \code{\link{validateConfiguration}}.
#' 
#' @author Weiming Hu \email{weiming@@psu.edu}
#' 
#' @param mode A character string of either 'independentSearch' or
#' 'extendedSearch'.
#' 
#' @return A configuration list with parameters for the AnEn algorithm.
#' 
#' @md
#' @export
generateConfiguration <- function(mode) {
  
  available.modes <- c('independentSearch')
  
  if (mode %in% available.modes) {
    # This is a valid mode
    
  } else {
    dists <- adist(mode, available.modes) / nchar(available.modes)
    
    if (length(unique(dists)) == length(dists)) {
      cat("Did you mean", available.modes[order(dists)[1]], "?\n")
    } else {
      cat("Available modes:", available.modes)
    }
    
    stop(paste("Unknown input mode", mode))	
  }
  
  if (mode == 'independentSearch') {
    config <- configAnEnIS()
  }
    
  return(config)
}
