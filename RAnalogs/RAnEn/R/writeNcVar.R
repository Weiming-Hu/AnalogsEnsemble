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

#' RAnEn::writeNcVar
#' 
#' RAnEn::writeNcVar can write a variable to a new NetCDF file if the file does not exist.
#' If the fist exists, RAnEn::writeNcVar tries to append the variable to the existing file.
#'
#' @details
#' This is a more general function than `RAnEn::writeNetCDF` because `RAnEn::writeNetCDF`
#' only takes a `Forecasts` or an `Observations`. But `RAnEn::writeNcVar` takes any numeric
#' arrays (including vectors, matrices, and arrays) and write them to an NetCDF file. This
#' can be helpful when you are saving `AnEn` results from `RAnEn::generateAnalogs`.
#' 
#' @param variable_name The name of the variable
#' @param variable_data The data for the variable. It can be a vector, a matrix, or an array.
#' @param dim_names The dimension names associated with the variable.
#' @param nc_file The complete NetCDF file path
#' @param verbose Whether to be verbose
#' 
#' @md
#' @export
writeNcVar <- function(variable_name, variable_data, dim_names, nc_file, verbose = F) {
  
  
  #################
  # Sanity checks #
  #################
  
  if (is.character(variable_data)) {
    stop('Character variables are not supported yet')
  }
  
  has_dimensions <- !is.null(dim(variable_data))
  
  if (has_dimensions) {
    stopifnot(length(dim(variable_data)) == length(dim_names))
  } else {
    stopifnot(length(dim_names) == 1)
  }
  
  
  ##################################
  # Create dimensions and variable #
  ##################################
  
  # Create dimensions
  nc_dims <- list()
  for (dim_index in 1:length(dim_names)) {
    nc_dims[[dim_index]] <- ncdf4::ncdim_def(name = dim_names[dim_index],
                                             units = '',
                                             create_dimvar = FALSE,
                                             vals = `if`(has_dimensions,
                                                         1:dim(variable_data)[dim_index],
                                                         1:length(variable_data)))
  }
  
  # Create a variable
  nc_var <- ncdf4::ncvar_def(name = variable_name,
                             units = '',
                             dim = nc_dims,
                             missval = NA,
                             verbose = verbose)
  
  
  ##########################
  # Add data to an Nc file #
  ##########################
  
  # Whether to create a new file or to add to an existing file
  # depends on whether the file exists. I need to check this first.
  #
  if (file.exists(nc_file)) {
    nc <- ncdf4::nc_open(filename = nc_file,
                         write = T,
                         verbose = verbose)
    
    tryCatch({
      nc <- ncdf4::ncvar_add(nc = nc,
                             v = nc_var,
                             verbose = verbose)
    }, error = function(e) {
      ncdf4::nc_close(nc)
      stop(e$message)
    })
    
  } else {
    nc <- ncdf4::nc_create(filename = nc_file,
                           vars = nc_var,
                           force_v4 = T,
                           verbose = verbose)
  }
  
  ncdf4::ncvar_put(nc, varid = nc_var, vals = variable_data, verbose = verbose)
  
  # Close the file
  ncdf4::nc_close(nc)
  
  if (verbose) {
    cat(variable_name, 'has been successfully written to', nc_file, '!\n')
  }
}
