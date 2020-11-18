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

#' RAnEn::readNc
#' 
#' RAnEn::readNc reads an NetCDF file as a list with `ncdf4` interface.
#' 
#' @author Weiming Hu \email{weiming@@psu.edu}
#' 
#' @param file The file to read
#' @param root_group_only Whether to read only root group variables
#' @param var_names By default, `var_names` are set to `NULL` and this function
#' reads all variables from the file. Set this argument if you only want to
#' read a subset of variables from the NetCDF file. If this argument is set,
#' `root_group_only` will be ignored. To read variables from a sub group,
#' you need to also include the group name as if it was a folder,
#' for exmple, `group_name/variable_name`. If `var_names` is a named vector,
#' the names will be used in the list.
#' @param include_root_attrs Whether to read the root group attributes.
#' @param return_class Setting this argument to simply change the class
#' of the returned list to support formatted output, e.g. `AnEn`.
#' 
#' @md
#' @export
readNc <- function(file, root_group_only = T,
									 var_names = NULL,
									 include_root_attrs = F,
									 return_class = NULL) {
  
  check.package('ncdf4')
  stopifnot(file.exists(file))
  
  # Create a list to store all values
  l <- list()
  
  # Open the NetCDF file
  nc <- ncdf4::nc_open(file)
  
  # Whether I need to rename list element
  rename_list <- F
  
  if (identical(NULL, var_names)) {
  	# Get variable names
  	var_names <- names(nc$var)
  	
  	# Check whether to read sub group variables
  	if (root_group_only) {
  		# Remove variable names with slash to only read the root group variables
  		var_names <- var_names[!grepl('/', var_names)]
  	}
  	
  } else {
  	if (!identical(NULL, names(var_names))) {
  		# If var_names are set with names, I need to rename list elements
  		rename_list <- T
  	}
  }
  
  # Read variable
  l <- readOptional(l, var_names, nc, group_prefix = '')
  
  if (rename_list) {
  	names_index <- which(nchar(names(var_names)) != 0)
  	for (name_index in names_index) {
  		names(l)[name_index] <- names(var_names)[name_index]
  	}
  }
  
  if (include_root_attrs) {
    l$attributes <- ncdf4::ncatt_get(nc, 0)
  }
  
  # Close the file
  ncdf4::nc_close(nc)
  
  if (!identical(NULL, return_class)) {
  	class(l) <- return_class
  }
  
  return(l)
}
