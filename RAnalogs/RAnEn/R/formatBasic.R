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

#' RAnEn::formatBasic
#' 
#' RAnEn::formatBasic is an internal function called by RAnEn::formatObservations
#' and RAnEn::formatForecasts to convert a data frame to AnEn Observations or 
#' Forecasts.
#' 
#' @md
#' @keywords internal
formatBasic <- function(obj, df, col.par, circular.pars, col.x, col.y,
                        col.station.name, time.series) {
  
  stopifnot(inherits(obj, 'Forecasts') | inherits(obj, 'Observations'))
  
  # Assign unique parameters
  obj$ParameterNames <- unique(df[[col.par]])
  
  if (!is.null(circular.pars)) {
    stopifnot(is.vector(circular.pars, mode = 'character'))
    
    for (name in circular.pars) {
      if (!name %in% obj$ParameterNames) {
        stop(paste(name, 'is not a parameter in the data frame.'))
      }
    }
    
    # Assign circular parameters
    obj$ParameterCirculars <- circular.pars
  }
  
  # Create unique id for stations based on coordinates
  df$..Station.ID.. <-  dplyr::group_indices(
    dplyr::group_by_at(df, .vars = dplyr::vars(col.x, col.y)))
  
  # These are the columns to keep in the unique station data frame
  station.cols <- c(col.x, col.y, "..Station.ID..")
  
  # Add the station name column is it is present
  if (!is.null(col.station.name)) {
    stopifnot(col.station.name %in% names(df))
    station.cols <- c(station.cols, col.station.name)
  }
  
  # Create a data frame with unique station ID
  unique.pts <- dplyr::distinct(
    dplyr::select(df, !!! rlang::syms(station.cols)),
    ..Station.ID.., .keep_all = T)
  
  # Assign station information to object
  if (!is.null(col.station.name)) {
    obj$StationNames <- unique.pts[[col.station.name]]
  }
  
  obj$Xs <- unique.pts[[col.x]]
  obj$Ys <- unique.pts[[col.y]]
  obj$Times <- time.series
  
  return(list(obj = obj, df = df, unique.pts = unique.pts))
}