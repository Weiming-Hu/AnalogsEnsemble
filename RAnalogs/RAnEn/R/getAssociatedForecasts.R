#  "`-''-/").___..--''"`-._
# (`6_ 6  )   `-.  (     ).`-.__.`)   WE ARE ...
# (_Y_.)'  ._   )  `._ `. ``-..-'    PENN STATE!
#   _ ..`--'_..-_/  /--'_.' ,'
# (il),-''  (li),'  ((!.-'
#
#
# Author: Weiming Hu (weiming@psu.edu), Laura Clemente-Harding (laura@psu.edu)
#         Geoinformatics and Earth Observation Laboratory (http://geolab.psu.edu)
#         Department of Geography and Institute for CyberScience
#         The Pennsylvania State University
#

#' RAnEn::getAssociatedForecasts
#' 
#' RAnEn::getAssociatedForecasts returns the most similar
#' past forecasts that are identified using this analog ensemble
#' and associated with each analog ensemble member. In 
#' other words, this funtion returns the most similar
#' past forecasts to the current forecasts.
#' 
#' @author Weiming Hu \email{weiming@@psu.edu}
#' @author Laura Clemente-Harding \email{laura@@psu.edu}
#' 
#' @param AnEn The AnEn class object returned by \code{\link{generateAnalogs}}.
#' @param config The Configuration class object used to generate AnEn.
#' @param forecast.parameter The parameter index for which variable to extract
#' in the forecast array in `config`.
#' @param i.station The station index in AnEn to extract.
#' @param i.test.day The test day index in AnEn to extract.
#' @param i.flt The FLT index in AnEn to extract.
#' @param similarity.name The similarity name to be used in `AnEn`. This
#' is useful when you have a different similarity matrice name to the regular one.
#' @param forecasts.name The forecast name to be used in `config`. This
#' is useful when you have a different similarity matrice name to the regular one.
#' @param members.to.extract The number of ensemble members to extract. By default
#' to extract all members.
#' 
#' @return An array with the same dimension as analogs in AnEn. For more 
#' information of each dimension, please refer to \code{\link{generateAnalogs}}.
#' The only difference is that the values in this array are extracted
#' from the most similar past forecast that is associated with each analog ensemble
#' member.
#' 
#' @md
#' @export
getAssociatedForecasts <- function(
  AnEn, config, forecast.parameter,
  i.station = NA, i.test.day = NA, i.flt = NA,
  similarity.name = 'similarity',
  forecasts.name = 'forecasts',
  members.to.extract = NA) {
  
  # Sanity check
  stopifnot(forecasts.name %in% names(config))
  stopifnot(similarity.name %in% names(AnEn))
  stopifnot(length(forecast.parameter) == 1)
  
  if (identical(i.station, NA)) i.station <- 1:dim(AnEn[[similarity.name]])[1]
  if (identical(i.test.day, NA)) i.test.day <- 1:dim(AnEn[[similarity.name]])[2]
  if (identical(i.flt, NA)) i.flt <- 1:dim(AnEn[[similarity.name]])[3]
  
  if (identical(members.to.extract, NA)) {
    members.to.extract <- dim(AnEn[[similarity.name]])[4]
  } else {
    stopifnot(members.to.extract <= dim(AnEn[[similarity.name]])[4])
  }
  
  forecasts <- config[[forecasts.name]][forecast.parameter, , , , drop = F]
  dim(forecasts) <- dim(forecasts)[-1]
  
  forecast.stations <- as.vector(AnEn[[similarity.name]][i.station, i.test.day, i.flt, 1:members.to.extract, 2])
  forecast.times <- as.vector(AnEn[[similarity.name]][i.station, i.test.day, i.flt, 1:members.to.extract, 3])
  forecast.flts <- rep(rep(i.flt, each = length(i.station) * length(i.test.day)), times = members.to.extract)
  indices <- cbind(forecast.stations, forecast.times, i.flt)
  
  analog.forecasts <- array(
    data = c(forecasts[indices], forecast.stations, forecast.times),
    dim = c(length(i.station), length(i.test.day), length(i.flt), members.to.extract, 3))
  
  return(analog.forecasts)
}
