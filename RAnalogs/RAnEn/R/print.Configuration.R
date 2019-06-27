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
#' 
#' @examples 
#' config <- generateConfiguration('independentSearch')
#' print(config)
#' 
#' @export
print.Configuration <- function(x) {
	
	if (class(x) != 'Configuration') stop('Not a Configuration object.')
	
  x <- formatConfiguration(x, F)
	valid <- validateConfiguration(x)
	
	cat('Class: Configuration list\n')
	cat('Mode: ', x$mode, '\n')
	cat("Advanced:", x$advanced, '\n')
	cat('Variables:\n')
	print(attr(x, 'names'))
	
	if (!valid) {
		cat('*** The configuration is not complete. It is NOT ready to be used. ***\n')
	}
}
