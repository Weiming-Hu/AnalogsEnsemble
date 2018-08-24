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
generateAnalogs <- function(configuration) {
	
	valid <- validateConfiguration(configuration)
	if (!valid) return(valid)
	
	configuration$observation_id = configuration$observation_id - 1
	
	if (configuration$mode == 'independentSearch') {
		
		# Create default values
		test_forecasts_station_x <- vector(mode = 'numeric', length = 0)
		test_forecasts_station_y <- vector(mode = 'numeric', length = 0)
		search_forecasts_station_x <- vector(mode = 'numeric', length = 0)
		search_forecasts_station_y <- vector(mode = 'numeric', length = 0)
		search_extension <- F
		preserve_search_stations <- F
		max_num_search_stations <- 0
		distance <- 0
		num_nearest_stations <- 0
		
		AnEn <- .generateAnalogs(
			configuration$test_forecasts, dim(configuration$test_forecasts),
			test_forecasts_station_x, test_forecasts_station_y,
			configuration$search_forecasts, dim(configuration$search_forecasts),
			search_forecasts_station_x, search_forecasts_station_y,
			configuration$search_times, configuration$search_flts,
			configuration$search_observations, dim(configuration$search_observations),
			configuration$observation_times, configuration$num_members,
			configuration$observation_id, configuration$quick,
			configuration$circulars, search_extension,
			configuration$preserve_similarity, 
			configuration$preserve_mapping, preserve_search_stations,
			max_num_search_stations, distance,
			num_nearest_stations, configuration$verbose)
		
	} else if (configuration$mode == 'extendedSearch') {
		# Create default values
		search_extension <- T
		
		AnEn <- .generateAnalogs(
			configuration$test_forecasts, dim(configuration$test_forecasts),
			configuration$test_stations_x,
			configuration$test_stations_y,
			configuration$search_forecasts, dim(configuration$search_forecasts),
			configuration$search_stations_x,
			configuration$search_stations_y,
			configuration$search_times, configuration$search_flts,
			configuration$search_observations, dim(configuration$search_observations),
			configuration$observation_times, configuration$num_members,
			configuration$observation_id, configuration$quick,
			configuration$circulars, search_extension,
			configuration$preserve_similarity,
			configuration$preserve_mapping,
			configuration$preserve_search_stations,
			configuration$max_num_search_stations,
			configuration$distance,
			configuration$num_nearest,
			configuration$verbose)
		
	} else {
		stop('Unknown configuration mode!')
	}
	
	# Because similarity matrix was stored in row-major in C++ and the R object will be column-major.
	# So change the order of dimensions to make it conform with other objects.
	#
	if (configuration$preserve_similarity) {
		AnEn$similarity <- aperm(AnEn$similarity, length(dim(AnEn$similarity)):1)
	}
	
	if (configuration$mode == 'extendedSearch') {
		if (configuration$preserve_search_stations) {
			AnEn$searchStations <- AnEn$searchStations + 1
		}
	}
	
	# Convert the station index from C counting to R counting
	AnEn$analogs[, , , , 1] <- AnEn$analogs[, , , , 1, drop = F] + 1
	AnEn$similarity[, , , , 1:2] <- AnEn$similarity[, , , , 1:2, drop = F] + 1
	
	if (configuration$preserve_mapping) AnEn$mapping <- AnEn$mapping + 1
	
	return(AnEn)
}
