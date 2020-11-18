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
#' RAnEn::readNc reads an NetCDF file as a list with `RNetCDF` interface.
#' 
#' @author Weiming Hu \email{weiming@@psu.edu}
#' 
#' @param file The file to read
#' @param recursive Whether to read variables recursively
#' @param read_attrs Whether to read attributes. Make sure there are no
#' name conflicts between attributes and variables. Only attributes under
#' groups or in the root group are read. Attributes associated with variables
#' are not read.
#' 
#' @md
#' @export
readNc2 <- function(file, recursive = F, read_attrs = F) {
	
	nc <- RNetCDF::open.nc(file)
	results <- RNetCDF::read.nc(nc, recursive = recursive)
	RNetCDF::close.nc(nc)
	
	if (read_attrs) {
		attrs <- readNcAttrs(file, recursive = recursive)
		results <- mergeList(results, attrs, names(results))
	}
	
	return(results)
}
