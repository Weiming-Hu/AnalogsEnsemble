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

#' RAnEn::toForecastTime
#' 
#' RAnEn::toForecastTime converts the observation time index to forecast time index.
#' 
#' This function does the following conversion.
#' 
#' `Observation time index [ + Forecast lead time index ] ----> Forecast time index`
#' 
#' To pin down the exact time of a forecast, two things are needed: forecast time and the lead time; to pin down
#' the exact time of an observation, only the observation time is needed. Therefore, this function is devoted to
#' convert from observation times to forecast times. Please note that, an observation time **CAN** be associated
#' with multiple forecast times because of the existence of forecast lead times. If you have constraints on the
#' lead time, then the match between forecasts and observations becomes one-on-one.
#' 
#' The observation time index usually comes from the `Time` column of the `analogs` member in the results of
#' \code{\link{generateAnalogs}}. This time is observation times. However, please note that the times in the 
#' `similarity` member are **forecast times**. These two are different.
#' 
#' @author Guido Cervone \email{cervone@@psu.edu}
#' @author Martina Calovi \email{mxc895@@psu.edu}
#' @author Laura Clemente-Harding \email{laura@@psu.edu}
#' 
#' @param observation.time.id The observation time index(es) from the `Time` column of the `analogs` member in
#' in the results of \code{\link{generateAnalogs}}.
#' @param mapping The mapping table created from \code{\link{generateTimeMapping}}.
#' @param flt The lead time index.
#' 
#' @md
#' @export
toForecastTime <- function(observation.time.id, mapping, flt = NULL) {
  
  mat <- NULL
  
  if (is.null(flt)) {
    
    if (length(observation.time.id) != 1) {
      stop('When all FLTs are requested, only one observation time id at a time.')
    }
    
    cs <- apply(mapping == observation.time.id, 1, any)
    
    if ( any(cs) ) {
      
      for ( flt in which(cs) ) {
        day <- which(mapping[flt, ] == observation.time.id)
        mat <- rbind(mat, c(day,flt))
      }
    } 
    
    return(mat)
    
  } else {
    mapping <- mapping[flt, ]
    
    return(sapply(observation.time.id, function(x, mapping) {
      which(x == mapping)},
      mapping = mapping))
  }
}

