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

#' RAnEn::subsetStations
#' 
#' RAnEn::subsetStations is a convevient function to subset stations
#' from forecast and observation lists. Given an index vector, presumably
#' from the `ID.R` column from RAnEn::subsetCoordinates, this function
#' can subset the desired stations from a forecast or observation list.
#' 
#' @details 
#' RAnEn::subsetStations will select the stations based on the input
#' index from the following members of the input list (if they exist):
#' 
#' - StationNames
#' - Xs
#' - Ys
#' - Data
#' 
#' @param index An index vector for stations to extract
#' @param l A forecast or observation list. For how to create such a 
#' list, please see
#' [this tutorial](https://weiming-hu.github.io/AnalogsEnsemble/2019/11/18/format-obs.html).
#' For what members to include in the list, see
#' [this doc](https://weiming-hu.github.io/AnalogsEnsemble/2019/01/16/NetCDF-File-Types.html).
#' @param verbose Whether to print progress information.
#' 
#' @return A forecast or observation list depending on your input
#' list type with the subset stations.
#' 
#' @examples 
#' # Subset a forecast list
#' forecasts <- list(
#' 	StationNames = paste0('Station', 1:10),
#' 	Xs = 1:10, Ys = 10:1,
#' 	Data = array(1:200, dim = c(2, 10, 5, 2))
#' )
#' 
#' index <- c(1, 5)
#' 
#' forecasts.sub <- subsetStations(index, forecasts)
#' 
#' forecasts.sub$StationNames
#' 
#' dim(forecasts$Data)
#' 
#' dim(forecasts.sub$Data)
#' 
#' # Subset an observation list
#' observations <- list(
#' 	StationNames = paste0('Station', 1:10),
#' 	Xs = 1:10, Ys = 10:1,
#' 	Data = array(1:100, dim = c(2, 10, 5))
#' )
#' 
#' index <- 6
#' 
#' observations.sub <- subsetStations(index, observations)
#' 
#' observations.sub$StationNames
#' 
#' dim(observations$Data)
#' 
#' dim(observations.sub$Data)
#' 
#' 
#' @md
subsetStations <- function(index, l, verbose = T) {
	
	# Sanity check
	stopifnot(is.list(l))
	stopifnot('Data' %in% names(l))
	
	num.stations <- dim(l$Data)[2]
	if (max(index) > num.stations) {
		stop('Some indices are larger than the number of stations.')
	}
	
	# These are the members to subset
	names <- c('StationNames', 'Xs', 'Ys', 'Data')
	
	for (name in names) {
		if (name %in% names(l)) {
			
			if (verbose) {
				cat('Subset the list member', name, '...\n')
			}
			
			if (name == 'Data') {
				
				if (length(dim(l[[name]])) == 3) {
					l[[name]] <- l[[name]][, index, , drop = F]
				} else if (length(dim(l[[name]])) == 4) {
					l[[name]] <- l[[name]][, index, , , drop = F]
				} else {
					stop('The member Data should have either 3 or 4 dimensions.')
				}
				
			} else {
				
				if (length(l[[name]]) != num.stations) {
					stop(paste('The member', name, 'has a different number of stations.'))
				}
				
				l[[name]] <- as.vector(l[[name]])[index]
			}
			
		}
	}
	
	if (verbose) {
		cat('Done (subsetStations)\n')
	}
	
	return(l)
}
