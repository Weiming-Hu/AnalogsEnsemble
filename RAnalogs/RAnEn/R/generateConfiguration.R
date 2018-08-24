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
generateConfiguration <- function(mode) {
	
	if (mode != 'independentSearch' && mode != 'extendedSearch') {
		stop(paste("Unknown mode", mode))	
	}
	
	config <- list(
		mode = mode, test_forecasts = NULL, search_forecasts = NULL, search_times = NULL, search_flts = NULL,
		search_observations = NULL, observation_times = NULL, observation_id = 1, num_members = NULL,
		circulars = vector(mode = 'numeric', length = 0), weights = NULL, quick = T,
		preserve_similarity = F, preserve_mapping = F, verbose = 1)
	
	if (mode == 'extendedSearch') {
		config <- c(config, list(test_stations_x = NULL, test_stations_y = NULL,
														 search_stations_x = NULL, search_stations_y = NULL,
														 max_num_search_stations = NULL, num_nearest = NULL,
														 distance = 0, preserve_search_stations = F))
	}
	
	class(config) <- "Configuration"
	return(config)
}