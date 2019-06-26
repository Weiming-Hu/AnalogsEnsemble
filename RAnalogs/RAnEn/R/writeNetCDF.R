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

#' RAnEn::writeNetCDF
#' 
#' RAnEn::writeNetCDF writes a formatted object into a NetCDF file. The output NetCDF
#' file can be used by C++ utilities.
#' 
#' The parameter `obj` for the file type `Observations` is an R list object with the
#' following members:
#' 
#' - ParameterNames
#' - StationNames (optional)
#' - Xs
#' - Ys
#' - Times
#' - Data
#' 
#' @author Weiming Hu \email{weiming@@psu.edu}
#' @param file.type The type of the object to be written to a file. It only supports
#' Observations for now.
#' @param obj A list object to write into the file.
#' @param file.out The output file path.
#' @param global.attr A list with names and values for global attributes. The values
#' will be written as strings. Please make sure list members have names.
#' @param nchars.max The maximum length of string.
#' @return A boolean for whether the writing is complete.
#' 
#' @md
#' @export
writeNetCDF <- function(file.type, obj, file.out,
                        global.attrs = NULL, nchars.max = 50) {
  require(ncdf4)
  
  # Check whether the output file path is valid
  if (file.exists(file.out)) {
    stop(paste('Output file', file.out, 'already exists.'))
  }
  
  if (file.type == 'Observations') {
    
    # Check for required members for the type Observations
    required.members <- c('ParameterNames', 'Xs', 'Ys', 'Times', 'Data')
    for (required.member in required.members) {
      if (! required.member %in% names(obj)) {
        stop(paste('Required member', required.member, 'does not exist in the object!'))
      }
    }
    
    # The member FLTs should never appear in the type of Observations
    if ('FLTs' %in% names(obj)) {
      stop('FLTs found in the object. Are you sure this is the type of Observations?')
    }
    
    # Sanity checks
    stopifnot(length(dim(obj$Data)) == 3)
    stopifnot(dim(obj$Data)[1] == length(obj$ParameterNames))
    stopifnot(dim(obj$Data)[2] == length(obj$Xs))
    stopifnot(length(obj$Xs) == length(obj$Ys))
    stopifnot(dim(obj$Data)[3] == length(obj$Times))
    
    # Get the dimension information from the member Data
    num.parameters <- dim(obj$Data)[1]
    num.stations <- dim(obj$Data)[2]
    num.times <- dim(obj$Data)[3]
    
    # Define dimensions
    nc.dim.parameters <- ncdim_def("num_parameters", "", 1:num.parameters, create_dimvar = F)
    nc.dim.stations <- ncdim_def("num_stations", "", 1:num.stations, create_dimvar = F)
    nc.dim.times <- ncdim_def("num_times", "", 1:num.times, create_dimvar = F)
    nc.dim.chars <- ncdim_def("num_chars", "", 1:nchars.max, create_dimvar = F)
    
    # Define variables
    nc.var.data <- ncvar_def("Data", "", list(nc.dim.parameters, nc.dim.stations, nc.dim.times), NA, prec = "double")
    nc.var.par.names <- ncvar_def("ParameterNames", "", list(nc.dim.chars, nc.dim.parameters), prec = "char")
    nc.var.xs <- ncvar_def("Xs", "", nc.dim.stations, prec = 'double')
    nc.var.ys <- ncvar_def("Ys", "", nc.dim.stations, prec = 'double')
    nc.var.times <- ncvar_def("Times", "", nc.dim.times, prec = 'double')
    
    # Create the NetCDF file
    if ('StationNames' %in% names(obj)) {
      nc.var.station.names <- ncvar_def("StationNames", "", list(nc.dim.chars, nc.dim.stations), prec = "char")
      
      nc <- nc_create(file.out, list(
        nc.var.data, nc.var.par.names, nc.var.xs, nc.var.ys,
        nc.var.times, nc.var.station.names))
      
      ncvar_put(nc, nc.var.station.names, obj$StationNames)
      
    } else {
      nc <- nc_create(file.out, list(
        nc.var.data, nc.var.par.names, nc.var.xs, nc.var.ys, nc.var.times))
    }
    
    ncvar_put(nc, nc.var.data, obj$Data, start = c(1, 1, 1))
    ncvar_put(nc, nc.var.par.names, obj$ParameterNames)
    ncvar_put(nc, nc.var.xs, obj$Xs)
    ncvar_put(nc, nc.var.ys, obj$Ys)
    ncvar_put(nc, nc.var.times, obj$Times)
    
    # Write the global attributes
    if (!identical(global.attrs, NULL)) {
      for (attr.name in names(global.attrs)) {
        ncatt_put(nc, 0, attr.name, global.attrs[[attr.name]])
      }
    }
    
    nc_close(nc)
    
  } else {
    stop(paste('The current file type', file.type, 'is not supported.'))
  }
  
}
