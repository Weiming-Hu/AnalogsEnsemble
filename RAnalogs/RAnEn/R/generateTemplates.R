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

#' RAnEn::generateForecastsTemplate
#' 
#' RAnEn::generateForecastsTemplate generates an empty forecasts.
#' This will guide you to create the correct forecasts that can
#' then be used in RAnEn::generateAnalogs.
#' 
#' @return An empty list with members
#' 
#' @md
#' @export
generateForecastsTemplate <- function() {
  
  # Get a config
  config <- new(Config)
  
  # Get the name pairs
  pairs <- config$getNames()
  
  # Create the template
  forecast.names <- c(
    pairs$`_DATA`, pairs$`_PAR_NAMES`, pairs$`_CIRCULARS`,
    pairs$`_STATION_NAMES`, pairs$`_XS`, pairs$`_YS`,
    pairs$`_TIMES`, pairs$`_FLTS`)
  
  forecasts <- vector("list", length(forecast.names))
  names(forecasts) <- forecast.names
  
  class(forecasts) <- c('Forecasts', class(forecasts))
  return(forecasts)
}

#' RAnEn::generateObservationsTemplate
#' 
#' RAnEn::generateObservationsTemplate generates an empty observations.
#' This will guide you to create the correct observations. that can
#' then be used in RAnEn::generateAnalogs.
#' 
#' @return An empty list with members
#' 
#' @md
#' @export
generateObservationsTemplate <- function() {
  
  # Get a config
  config <- new(Config)
  
  # Get the name pairs
  pairs <- config$getNames()
  
  # Create the template
  obs.names <- c(
    pairs$`_DATA`, pairs$`_PAR_NAMES`, pairs$`_STATION_NAMES`,
    pairs$`_XS`, pairs$`_YS`, pairs$`_TIMES`)
  
  observations <- vector("list", length(obs.names))
  names(observations) <- obs.names
  
  class(observations) <- c('Observations', class(observations))
  return(observations)
}

