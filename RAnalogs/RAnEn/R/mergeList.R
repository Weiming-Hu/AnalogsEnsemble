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

#' RAnEn::mergeList
#' 
#' RAnEn::mergeList combine two lists. It does not allow names in conflict
#' unless the name is further a list. This is similar to a depth-first copy
#' for list variables.
#' 
#' @author Weiming Hu \email{weiming@@psu.edu}
#' 
#' @param from A list to copy from
#' @param to A list to copy to
#' @param names the names to copy, usually `names(from)`
#' 
#' @examples 
#' library(RAnEn)
#' from <- list(C = list(C = 3, D = list(A = 1)), D = 3)
#' to <- list(A = 1, B = 2, C = list(A = 1, B = 2))
#' merged <- mergeList(from, to, names(from))
#' 
#' print(from, recursive = T)
#' print(to, recursive = T)
#' print(merged, recursive = T)
#' 
#' @md
#' @export
mergeList <- function(from, to, names) {
	
	stopifnot(is.list(from))
	stopifnot(is.list(to))
	stopifnot(all(names %in% names(from)))
	
	for (name in names) {
		if (! name %in% names(to)) {
			# Copy if name does not exist in the target list
			to[[name]] <- from[[name]]
		} else {
			
			if (is.list(from[[name]]) & is.list(to[[name]])) {
				# Recursively copy with list objects
				to[[name]] <- mergeList(from[[name]], to[[name]], names(from[[name]]))
				
			} else {
				# Conflict
				stop('Name conflict')
			}
		}
	}
	
	return(to)
}
