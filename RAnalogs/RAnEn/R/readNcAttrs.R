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

#' RAnEn::readNcAttrs
#' 
#' RAnEn::readNcAttrs reads NetCDF attributes. Only attributes under
#' groups or in the root group are read. Attributes associated with variables
#' are not read.
#' 
#' @author Weiming Hu \email{weiming@@psu.edu}
#' 
#' @param file The file to read
#' @param recursive Whether to read attributes recursively
#' 
#' @md
#' @export
readNcAttrs <- function(file, recursive) {
	x <- list()
	nc <- RNetCDF::open.nc(file)
	x <- add.global.attr.nc(nc, x, recursive)
	RNetCDF::close.nc(nc)
	return(x)
}

add.global.attr.nc <- function(nc, x, recursive) {
	GLOBAL_ID <- 'NC_GLOBAL'
	nc_inq <- RNetCDF::grp.inq.nc(nc)
	
	if (nc_inq$ngatts > 0) {
		for (attr_id in 0:(nc_inq$ngatts - 1)) {
			attr_inq <- RNetCDF::att.inq.nc(nc, GLOBAL_ID, attr_id)
			x[[attr_inq$name]] <- RNetCDF::att.get.nc(nc, GLOBAL_ID, attr_inq$name)
		}
	}
	
	if (recursive) {
		groups <- RNetCDF::grp.inq.nc(nc)
		if (length(groups$grps) > 0) {
			for (group_id in 1:length(groups$grps)) {
				group <- RNetCDF::grp.inq.nc(groups$grps[[group_id]])
				attrs <- add.global.attr.nc(groups$grps[[group_id]], list(), recursive)
				if (length(attrs) > 0) {
					x[[group$name]] <- attrs
				}
			}
		}
	}
	
	return(x)
}
