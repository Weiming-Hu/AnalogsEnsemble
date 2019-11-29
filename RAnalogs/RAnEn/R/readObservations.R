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
  stopifnot(file.exists(file))
  
  if (!requireNamespace('ncdf4', quietly = T)) {
    stop(paste('Please install ncdf4.'))
  }
  
  observations <- list()
  nc <- ncdf4::nc_open(file)
  observations$Data <- ncdf4::ncvar_get(nc, 'Data', collapse_degen = F)
  observations$Times <- as.POSIXct(ncdf4::ncvar_get(nc, 'Times'), origin = origin, tz = tz)
  observations$Xs <- as.numeric(ncdf4::ncvar_get(nc, 'Xs'))
  observations$Ys <- as.numeric(ncdf4::ncvar_get(nc, 'Ys'))
  
  for (name in c('ParameterCirculars', 'StationNames',
                 'ParameterNames', 'ParameterWeights')) {
    if (name %in% names(nc$var)) {
      observations[[name]] <- as.vector(ncdf4::ncvar_get(nc, name))
      
      if (name == 'ParameterCirculars') {
        # Remove empty element
        observations[[name]] <- observations[[name]][observations[[name]] != '']
      }
    }
  }
  
  ncdf4::nc_close(nc)
  
  garbage <- gc(verbose = F, reset = T)
  
  return(observations)
}
