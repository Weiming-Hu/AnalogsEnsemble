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
#' RAnEn::readNc reads an NetCDF file as a list
#' 
#' @author Weiming Hu \email{weiming@@psu.edu}
#' 
#' @param file The file to read
#' @param origin The origin of time to be used in as.POSIXct
#' @param tz The time zone of time to be used in as.POSIXct
#' @param root_group_only Whether to read only root group variables
#' @param var_names By default, `var_names` are set to `NULL` and this function
#' reads all variables from the file. Set this argument if you only want to
#' read a subset of variables from the NetCDF file. To read variables from a
#' sub group, you need to also include the group name as if it was a folder,
#' for exmple, `group_name/variable_name`.
#' 
#' @md
#' @export
readNc <- function(file, origin = "1970-01-01", tz = "UTC",
									 root_group_only = T, var_names = NULL) {
  
  check.package('ncdf4')
  stopifnot(file.exists(file))
  
  # Create a list to store all values
  l <- list()
  
  # Open the NetCDF file
  nc <- ncdf4::nc_open(file)
  
  if (identical(NULL, var_names)) {
  	# Get variable names
  	var_names <- names(nc$var)
  	
  	# Check whether to read sub group variables
  	if (root_group_only) {
  		# Remove variable names with slash to only read the root group variables
  		var_names <- var_names[!grepl('/', var_names)]
  	}
  }
  
  # Read variable
  l <- readOptional(l, var_names, nc, group_prefix = '')	
  
  # Close the file
  ncdf4::nc_close(nc)
  
  return(l)
}