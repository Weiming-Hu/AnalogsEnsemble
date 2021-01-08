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

#' RAnEn::readObservations
#' 
#' RAnEn::readObservations reads observation data from the NetCDF file with a
#' [standard format](https://weiming-hu.github.io/AnalogsEnsemble/2019/01/16/NetCDF-File-Types.html).
#' Most often, the file should be created from gribConverter.
#' 
#' @author Weiming Hu \email{weiming@@psu.edu}
#' 
#' @param file The observation NetCDF file to read.
#' @param origin The origin of time to be used in as.POSIXct.
#' @param tz The time zone of time to be used in as.POSIXct.
#' 
#' @return A list object.
#' 
#' @md
#' @export
readObservations <- function(file, origin = '1970-01-01', tz = 'UTC') {
  # check.package('ncdf4')
  stopifnot(file.exists(file))
  
  config <- new(Config)
  pairs <- config$getNames()
  
  observations <- generateObservationsTemplate()
  
  nc <- ncdf4::nc_open(file)
  
  # Check if Observations is the root group
  # If it is the root group, no variable prefix is needed
  # If it is not the root group, the variable prefix should be "Observations/"
  # 
  var_prefix <- ''
  if (any(grepl("^Observations/", names(nc$var)))) {
    var_prefix <- 'Observations/'
  }
  
  # Take care reading the required variables
  var_data <- paste0(var_prefix, pairs$`_DATA`)
  var_times <- paste0(var_prefix, pairs$`_TIMES`)
  
  # Check required variable names
  stopifnot(var_data %in% names(nc$var))
  stopifnot(var_times %in% names(nc$var))
  
  # Read required variables
  observations[[pairs$`_DATA`]] <- ncdf4::ncvar_get(nc, var_data, collapse_degen = F)
  observations[[pairs$`_TIMES`]] <- ncvar_get_times(nc, var_times, origin, tz)
  
  # Read optional variables
  optional_names <- c(pairs$`_PAR_NAMES`, pairs$`_CIRCULARS`,
                      pairs$`_STATION_NAMES`, pairs$`_XS`, pairs$`_YS`)
  observations <- readOptional(observations, optional_names, nc, var_prefix)
  
  ncdf4::nc_close(nc)
  
  garbage <- gc(verbose = F, reset = T)
  
  return(observations)
}
