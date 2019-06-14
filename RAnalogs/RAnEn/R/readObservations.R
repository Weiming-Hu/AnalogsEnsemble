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
  require(ncdf4)
  stopifnot(file.exists(file))
  
  observations <- list()
  nc <- nc_open(file)
  observations$Data <- ncvar_get(nc, 'Data', collapse_degen = F)
  observations$Times <- as.POSIXct(ncvar_get(nc, 'Times'), origin = origin, tz = tz)
  nc_close(nc)
  
  return(forecasts)
}