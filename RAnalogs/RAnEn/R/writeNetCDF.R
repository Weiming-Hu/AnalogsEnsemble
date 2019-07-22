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
writeNetCDF <- function(file.type, obj, file.out,
												global.attrs = NULL, nchars.max = 50) {
	require(ncdf4)
	
	# Check whether the output file path is valid
	if (file.exists(file.out)) {
		stop(paste("Output file", file.out, "already exists."))
	}
	
	if (file.type == "Observations") {
		required.members <- c("ParameterNames", "Xs", "Ys", "Times", "Data")
	} else if (file.type == "Forecasts") {
		required.members <- c("ParameterNames", "Xs", "Ys", "Times", "FLTs", "Data")
	} else {
		stop(paste("The current file type", file.type, "is not supported."))
	}
	
	# Check for required members
	for (required.member in required.members) {
		if (! required.member %in% names(obj)) {
			stop(paste("Required member", required.member, "does not exist in the object!"))
		}
	}
	
	# Define the maximum length of strings
	nc.dim.chars <- ncdim_def("num_chars", "", 1:nchars.max, create_dimvar = F)
	
	# Define the variable list that the NetCDF file should include
	vars.list <- list()
	
	# Prepare ParameterNames
	stopifnot(dim(obj$Data)[1] == length(obj$ParameterNames))
	num.parameters <- dim(obj$Data)[1]
	nc.dim.parameters <- ncdim_def("num_parameters", "", 1:num.parameters, create_dimvar = F)
	nc.var.par.names <- ncvar_def("ParameterNames", "", list(nc.dim.chars, nc.dim.parameters), prec = "char")
	vars.list <- c(vars.list, list(ParameterNames = nc.var.par.names))
	
	if ("ParameterCirculars" %in% names(obj)) {
	  nc.var.par.circulars <- ncvar_def("ParameterCirculars", "", list(nc.dim.chars, nc.dim.parameters), prec = "char")
	  vars.list <- c(vars.list, list(ParameterCirculars = nc.var.par.circulars))
	  
	  if (length(obj$ParameterCirculars) < length(obj$ParameterNames)) {
	    obj$ParameterCirculars <- c(obj$ParameterCirculars, 
	                                rep('', length(obj$ParameterNames) -
	                                      length(obj$ParameterCirculars)))
	  }
	}
	
	# Prepare StationNames
	if ("StationNames" %in% names(obj)) {
		nc.var.station.names <- ncvar_def("StationNames", "", list(nc.dim.chars, nc.dim.stations), prec = "char")
		vars.list <- c(vars.list, list(StationNames = nc.var.station.names))
	}
	
	# Prepare Xs and Ys
	stopifnot(dim(obj$Data)[2] == length(obj$Xs) &
							length(obj$Xs) == length(obj$Ys))
	num.stations <- dim(obj$Data)[2]
	nc.dim.stations <- ncdim_def("num_stations", "", 1:num.stations, create_dimvar = F)
	nc.var.xs <- ncvar_def("Xs", "", nc.dim.stations, prec = "double")
	nc.var.ys <- ncvar_def("Ys", "", nc.dim.stations, prec = "double")
	vars.list <- c(vars.list, list(Xs = nc.var.xs), list(Ys = nc.var.ys))
	
	# Prepare Times
	stopifnot(dim(obj$Data)[3] == length(obj$Times))
	num.times <- dim(obj$Data)[3]
	nc.dim.times <- ncdim_def("num_times", "", 1:num.times, create_dimvar = F)
	nc.var.times <- ncvar_def("Times", "", nc.dim.times, prec = "double")
	vars.list <- c(vars.list, list(Times = nc.var.times))
	
	# Prepare FLTs
	if (file.type == "Forecasts") {
		stopifnot(dim(obj$Data)[4] == length(obj$FLTs))
		num.flts <- dim(obj$Data)[4]
		nc.dim.flts <- ncdim_def("num_flts", "", 1:num.flts, create_dimvar = F)
		nc.var.flts <- ncvar_def("FLTs", "", nc.dim.flts, prec = "double")
		vars.list <- c(vars.list, list(FLTs = nc.var.flts))
	}
	
	# Prepare Data
	if (file.type == "Observations") {
		stopifnot(length(dim(obj$Data)) == 3)
		nc.var.data <- ncvar_def("Data", "", list(
			nc.dim.parameters, nc.dim.stations, nc.dim.times), NA, prec = "double")
	} else if (file.type == "Forecasts") {
		stopifnot(length(dim(obj$Data)) == 4)
		nc.var.data <- ncvar_def("Data", "", list(
			nc.dim.parameters, nc.dim.stations, nc.dim.times, nc.dim.flts), NA, prec = "double")
	}
	vars.list <- c(vars.list, list(Data = nc.var.data))
	
	# Create nc object
	nc <- nc_create(file.out, vars.list)
	
	# Write global attributes
	if (!identical(global.attrs, NULL)) {
		for (attr.name in names(global.attrs)) {
			ncatt_put(nc, 0, attr.name, global.attrs[[attr.name]])
		}
	}
	
	for (name in names(vars.list)) {
		ncvar_put(nc, vars.list[[name]], obj[[name]])
	}
	
	nc_close(nc)
	
	garbage <- gc(verbose = F, reset = T)
}
