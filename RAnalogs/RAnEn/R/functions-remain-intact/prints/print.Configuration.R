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

#' RAnEn::print.Configuration
#' 
#' Overload print function for class Configuration
#' 
#' @author Weiming Hu \email{weiming@@psu.edu}
#' 
#' @param x a Configuration object
#' @param forecasts.names The name of each forecast variable (the first dimension in the forecast array).
#' 
#' @examples 
#' config <- generateConfiguration('independentSearch')
#' print(config)
#' 
#' @export
print.Configuration <- function(x, forecasts.names = NULL) {
	
	if (class(x) != 'Configuration') stop('Not a Configuration object.')
	
  x <- formatConfiguration(x, F)
	valid <- validateConfiguration(x)
	
	cat('Class: Configuration list\n')
	cat('Mode: ', x$mode, '\n')
	cat("Advanced:", x$advanced, '\n')
	cat('Variables:\n')
	print(attr(x, 'names'))
	
	if (!valid) {
		cat('*** The configuration is NOT complete. Check the above message for errors! ***\n')
	} else {
	  # Name the weights
	  weights <- x$weights
	  if (!is.null(forecasts.names)) {
	    stopifnot(length(weights) == length(forecasts.names))
	    names(weights) <- forecasts.names
	  }
	  
	  if (length(x$test_times_compare) == 0) {
	    test.range <- c(NA, NA)
	  } else {
	    test.range <- range(x$test_times_compare)
	    if (is.numeric(test.range)) test.range <- toDateTime(test.range)
	  }
	  
	  if (length(x$search_times_compare) == 0) {
	    search.range <- c(NA, NA)
	  } else {
	    search.range <- range(x$search_times_compare)
	    if (is.numeric(search.range)) search.range <- toDateTime(search.range)
	  }
	  
	  cat("\nThe observation id is", x$observation_id,
	      "\nThere are", length(x$test_times_compare), "test times from",
	      format(test.range[1], format = "%Y-%m-%d %H:%M:%S %Z")
	      , 'to', format(test.range[2], format = "%Y-%m-%d %H:%M:%S %Z"),
	      "\nThere are", length(x$search_times_compare), "search times from",
	      format(search.range[1], format = "%Y-%m-%d %H:%M:%S %Z"), 'to', 
	      format(search.range[2], format = "%Y-%m-%d %H:%M:%S %Z"),
	      '\nWeights are:\n')
	  print(weights)
	}
}
