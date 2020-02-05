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
  if (!requireNamespace('ncdf4', quietly = T)) {
    stop(paste('Please install ncdf4.'))
  }
  
  stopifnot(file.exists(file))
  
  config <- new(Config)
  pairs <- config$getNames()
  
  forecasts <- list()
  
  nc <- ncdf4::nc_open(file)
  forecasts[[pairs$`_DATA`]] <- ncdf4::ncvar_get(nc, pairs$`_DATA`, collapse_degen = F)
  forecasts[[pairs$`_TIMES`]] <- as.POSIXct(ncdf4::ncvar_get(nc, pairs$`_TIMES`), origin = origin, tz = tz)
  forecasts[[pairs$`_FLTS`]] <- ncdf4::ncvar_get(nc, pairs$`_FLTS`)
  
  for (name in c(pairs$`_PAR_NAMES`, pairs$`_CIRCULARS`,
                 pairs$`_STATION_NAMES`, pairs$`_XS`, pairs$`_YS`)) {
    
    if (name %in% names(nc$var)) {
      forecasts[[name]] <- as.vector(ncdf4::ncvar_get(nc, name))
      
      if (name == pairs$`_CIRCULARS`) {
        # Remove empty element
        forecasts[[name]] <- forecasts[[name]][forecasts[[name]] != '']
      }
    }
  }
  
  ncdf4::nc_close(nc)
  
  garbage <- gc(verbose = F, reset = T)
  
  return(forecasts)
}
