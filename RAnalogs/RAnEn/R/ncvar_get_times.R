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

#' RAnEn::ncvar_get_times
#' 
#' RAnEn::ncvar_get_times is a convenient function to read Times from 
#' a already open NetCDF file connection. It echos the API of `ncdf4:ncvar_get` 
#' but it differs in that it checks for any reading imprecision and corrects 
#' for small errors.
#' 
#' @author Weiming Hu \email{weiming@@psu.edu}
#' 
#' @param nc An object of class ncdf4 (as returned by either function nc_open or function nc_create), indicating what file to read from.
#' @param var_name The variable name for `Times`
#' @param origin The origin used for converting to POSIX time
#' @param tz The time zone
#' @param to_posix Whether to convert times to POSIX
#' @param epsilon The maximum reading error to correct. Otherwise, an error is thrown.
#' 
#' @return A vector of times
#' 
#' @md
#' @export
ncvar_get_times <- function(nc, var_name, origin = '1970-01-01', tz = 'UTC', to_posix = T, epsilon = 1e-5) {
  
  #############################################################################
  #                                                                           #
  # Note: R might introduce numerical imprecision when reading double values  #
  # from NetCDF files. This can be observed when a double with no decimal     #
  # in the NetCDF was read with a decimal value. I need to remove this error. #
  # Epsilon is the tolerance threshold for the error.                         #
  #                                                                           #
  #############################################################################
  
  times <- ncdf4::ncvar_get(nc, var_name)
  
  # Calculate imprecision error
  decimal <- times %% 1
  
  if (all(decimal < epsilon)) {
    times <- times - decimal
    
  } else {
    stop('The reading error is too large. Consider checking ncdf4::ncvar_get and changing epsilon.')
  }
  
  if (to_posix) {
    times <- as.POSIXct(times, origin = origin, tz = tz)
  }
  
  return(times)
}
