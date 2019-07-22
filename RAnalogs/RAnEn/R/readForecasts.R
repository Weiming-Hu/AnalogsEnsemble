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

#' RAnEn::readForecasts
#' 
#' RAnEn::readForecasts reads forecast data from the NetCDF file with a
#' [standard format](https://weiming-hu.github.io/AnalogsEnsemble/2019/01/16/NetCDF-File-Types.html).
#' Most often, the file should be created from gribConverter.
#' 
#' @author Weiming Hu \email{weiming@@psu.edu}
#' 
#' @param file The forecast NetCDF file to read.
#' @param origin The origin of time to be used in as.POSIXct.
#' @param tz The time zone of time to be used in as.POSIXct.
#' 
#' @return A list object.
#' 
#' @md
#' @export
readForecasts <- function(file, origin = '1970-01-01', tz = 'UTC') {
  require(ncdf4)
  stopifnot(file.exists(file))
  
  forecasts <- list()
  
  nc <- nc_open(file)
  forecasts$ParameterNames <- ncvar_get(nc, 'ParameterNames')
  forecasts$Data <- ncvar_get(nc, 'Data', collapse_degen = F)
  forecasts$Times <- as.POSIXct(ncvar_get(nc, 'Times'), origin = origin, tz = tz)
  forecasts$Xs <- ncvar_get(nc, 'Xs')
  forecasts$Ys <- ncvar_get(nc, 'Ys')
  forecasts$FLTs <- ncvar_get(nc, 'FLTs')
  
  nc_close(nc)
  
  garbage <- gc(verbose = F, reset = T)
  
  return(forecasts)
}