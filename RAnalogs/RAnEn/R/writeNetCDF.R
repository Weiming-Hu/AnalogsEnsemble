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
#' @details 
#' The parameter `obj` is an R list object with the
#' following members:
#' 
#' - ParameterNames
#' - ParameterCirculars (optional)
#' - StationNames (optional)
#' - Xs
#' - Ys
#' - Times
#' - Data
#' - FLTs (required for type Forecasts)
#' 
#' Please note that `ParameterWeights` member is not written to files.
#' 
#' @author Weiming Hu \email{weiming@@psu.edu}
#' @param file.type The type of the object to be written to a file. It only supports
#' Observations and Forecasts for now.
#' @param obj A list object to write into the file.
#' @param file.out The output file path.
#' @param global.attr A list with names and values for global attributes. The values
#' will be written as strings. Please make sure list members have names.
#' @param nchars.max The maximum length of string.
#' @return A boolean for whether the writing is complete.
#' 
#' @md
#' @export
writeNetCDF <- function(
  file.type, obj, file.out, global.attrs = list(
    author = paste("RAnEn", packageVersion('RAnEn'), "from GEOlab, Penn State"),
    Time = Sys.time()), nchars.max = 50) {
  
  if (!requireNamespace('ncdf4', quietly = T)) {
    stop(paste('Please install ncdf4.'))
  }
  
  # Check whether the output file path is valid
  if (file.exists(file.out)) {
    stop(paste("Output file", file.out, "already exists."))
  }
  
  # Get name pairs
  config <- new(Config)
  pairs <- config$getNames()
  
  if (file.type == "Observations") {
    required.members <- c(pairs$`_PAR_NAMES`, pairs$`_XS`, pairs$`_YS`, pairs$`_TIMES`, pairs$`_DATA`)
  } else if (file.type == "Forecasts") {
    required.members <- c(pairs$`_PAR_NAMES`, pairs$`_XS`, pairs$`_YS`, pairs$`_TIMES`, pairs$`_FLTS`, pairs$`_DATA`)
  } else {
    stop(paste("The current file type", file.type, "is not supported."))
  }
  
  # Check for required members
  for (required.member in required.members) {
    if (! required.member %in% names(obj)) {
      stop(paste("Required member", required.member, "does not exist in the object!"))
    }
  }
  
  # Prevent NA and NULL variables
  for (name in names(obj)) {
    stopifnot(!is.null(obj[[name]]))
    stopifnot(!identical(NA, obj[[name]]))
  }
  
  # Define the maximum length of strings
  nc.dim.chars <- ncdf4::ncdim_def(pairs$`_DIM_CHARS`, "", 1:nchars.max, create_dimvar = F)
  
  # Define the variable list that the NetCDF file should include
  vars.list <- list()
  
  # Prepare ParameterNames
  stopifnot(dim(obj$Data)[1] == length(obj$ParameterNames))
  num.parameters <- dim(obj$Data)[1]
  nc.dim.parameters <- ncdf4::ncdim_def(pairs$`_DIM_PARS`, "", 1:num.parameters, create_dimvar = F)
  nc.var.par.names <- ncdf4::ncvar_def(pairs$`_PAR_NAMES`, "", list(nc.dim.chars, nc.dim.parameters), prec = "char")
  vars.list <- c(vars.list, list(ParameterNames = nc.var.par.names))
  
  if (pairs$`_CIRCULARS` %in% names(obj)) {
    nc.var.par.circulars <- ncdf4::ncvar_def(pairs$`_CIRCULARS`, "", list(nc.dim.chars, nc.dim.parameters), prec = "char")
    vars.list <- c(vars.list, list(ParameterCirculars = nc.var.par.circulars))
    
    if (length(obj$ParameterCirculars) < length(obj$ParameterNames)) {
      obj$ParameterCirculars <- c(obj$ParameterCirculars, 
                                  rep('', length(obj$ParameterNames) -
                                        length(obj$ParameterCirculars)))
    }
  }
  
  # Prepare Xs and Ys
  stopifnot(dim(obj$Data)[2] == length(obj$Xs) &
              length(obj$Xs) == length(obj$Ys))
  num.stations <- dim(obj$Data)[2]
  nc.dim.stations <- ncdf4::ncdim_def(pairs$`_DIM_STATIONS`, "", 1:num.stations, create_dimvar = F)
  nc.var.xs <- ncdf4::ncvar_def(pairs$`_XS`, "", nc.dim.stations, prec = "double")
  nc.var.ys <- ncdf4::ncvar_def(pairs$`_YS`, "", nc.dim.stations, prec = "double")
  vars.list <- c(vars.list, list(Xs = nc.var.xs), list(Ys = nc.var.ys))
  
  # Prepare StationNames
  if (pairs$`_STATION_NAMES` %in% names(obj)) {
    if (length(obj$StationNames) != length(obj$Xs)) {
      stop('The number of station names does not match the number of coordinates.')
    }
    nc.var.station.names <- ncdf4::ncvar_def(pairs$`_STATION_NAMES`, "", list(nc.dim.chars, nc.dim.stations), prec = "char")
    vars.list <- c(vars.list, list(StationNames = nc.var.station.names))
  }
  
  # Prepare Times
  stopifnot(dim(obj$Data)[3] == length(obj$Times))
  num.times <- dim(obj$Data)[3]
  nc.dim.times <- ncdf4::ncdim_def(pairs$`_DIM_TIMES`, "", 1:num.times, create_dimvar = F)
  nc.var.times <- ncdf4::ncvar_def(pairs$`_TIMES`, "", nc.dim.times, prec = "double")
  vars.list <- c(vars.list, list(Times = nc.var.times))
  
  # Prepare FLTs
  if (file.type == "Forecasts") {
    stopifnot(dim(obj$Data)[4] == length(obj$FLTs))
    num.flts <- dim(obj$Data)[4]
    nc.dim.flts <- ncdf4::ncdim_def(pairs$`_DIM_FLTS`, "", 1:num.flts, create_dimvar = F)
    nc.var.flts <- ncdf4::ncvar_def(pairs$`_FLTS`, "", nc.dim.flts, prec = "double")
    vars.list <- c(vars.list, list(FLTs = nc.var.flts))
  }
  
  # Prepare Data
  if (file.type == "Observations") {
    stopifnot(length(dim(obj$Data)) == 3)
    nc.var.data <- ncdf4::ncvar_def(pairs$`_DATA`, "", list(
      nc.dim.parameters, nc.dim.stations, nc.dim.times), NA, prec = "double")
  } else if (file.type == "Forecasts") {
    stopifnot(length(dim(obj$Data)) == 4)
    nc.var.data <- ncdf4::ncvar_def(pairs$`_DATA`, "", list(
      nc.dim.parameters, nc.dim.stations, nc.dim.times, nc.dim.flts), NA, prec = "double")
  }
  vars.list <- c(vars.list, list(Data = nc.var.data))
  
  # Create nc object
  nc <- ncdf4::nc_create(file.out, vars.list)
  
  # Write global attributes
  if (!is.null(global.attrs)) {
    for (attr.name in names(global.attrs)) {
      ncdf4::ncatt_put(nc, 0, attr.name, global.attrs[[attr.name]])
    }
  }
  
  for (name in names(vars.list)) {
    ncdf4::ncvar_put(nc, vars.list[[name]], obj[[name]])
  }
  
  ncdf4::nc_close(nc)
  
  garbage <- gc(verbose = F, reset = T)
}
