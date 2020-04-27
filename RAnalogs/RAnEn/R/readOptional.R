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

#' RAnEn::readOptional
#' 
#' RAnEn::readOptional reads the input optional names from an NetCDF file object
#' with `read` file mode.
#' 
#' @author Weiming Hu \email{weiming@@psu.edu}
#' 
#' @param obs Observations or Forecasts
#' @param names The optional names to read
#' @param nc An NetCDF file object
#' @param group_prefix The group prefix to use to find the variables
#' 
#' @return The input Observations or Forecasts but with extra names
#' 
#' @md
#' @keywords internal
readOptional <- function(obj, names, nc, group_prefix) {
  
  # Get some predefined names
  config <- new(Config)
  pairs <- config$getNames()
  
  for (name in names) {
    
    # Make sure that I'm not overwriting any variables in the object
    if (name %in% names(obj)) {
      if (!is.null(obj[[name]])) {
        stop(paste(name, 'already exists in the input object (Internal from RAnEn::readOptional)'))
      }
    }
    
    # This is the variable name to search for from the NetCDF file
    var_name <- paste0(group_prefix, name)
    
    # If the variable name does not exist, skip reading
    if (!(var_name %in% names(nc$var))) {
      next
    }
    
    # Read the variable from the NetCDF file
    var <- ncdf4::ncvar_get(nc, var_name, collapse_degen = F)
    
    # If this variable is an array with only one dimension, I convert it to a vector
    if (!is.null(dim(var))) {
      if (length(dim(var)) == 1) {
        var <- as.vector(var)
      }
    }
    
    # If this variable is for circular parameters, I remove the empty entries
    if (name == pairs$`_CIRCULARS`) {
      var <- var[var != '']
    }
    
    obj[[name]] <- var
  }
  
  return(obj)
}
