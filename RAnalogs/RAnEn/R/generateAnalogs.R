# "`-''-/").___..--''"`-._
#  (`6_ 6  )   `-.  (     ).`-.__.`)   WE ARE ...
#  (_Y_.)'  ._   )  `._ `. ``-..-'    PENN STATE!
#    _ ..`--'_..-_/  /--'_.' ,'
#  (il),-''  (li),'  ((!.-'
# 
# Author: Weiming Hu <wuh20@psu.edu>
#         Geoinformatics and Earth Observation Laboratory (http://geolab.psu.edu)
#         Department of Geography and Institute for CyberScience
#         The Pennsylvania State University
#
generateAnalogs <- function(
	test_forecasts, search_forecasts, search_times, search_flts,
	search_observations, observation_times, num_members,
	observation_id = 1, quick = T, circulars = NA, weights = NA, 
        xs = NA, ys = NA, preserve_similarity = F, verbose = 1) {
	
	# Check input
	if (!(is.array(test_forecasts) && is.numeric(test_forecasts) && length(dim(test_forecasts)) == 4)) {
		print('ERROR: Test forecasts should be a 4-dimensional numeric array!')
		print('Please use dim(), is.numeric(), and is.array() to check!')
		return(0)
	}
	
	if (!(is.array(search_forecasts) && is.numeric(search_forecasts) && length(dim(search_forecasts)) == 4)) {
		print('ERROR: Search forecasts should be a 4-dimensional numeric array!')
		print('Please use dim(), is.numeric(), and is.array() to check!')
		return(0)
	}
	
	if (!(is.vector(search_times, mode = 'numeric') && length(search_times) == dim(search_forecasts)[3])) {
		print('ERROR: Search forecast times should be a numeric vector with the length of the third dimension of search forecasts!')
		print('Please use is.vector() and length() to check!')
		return(0)
	}
	
	if (!(is.vector(search_flts, mode = 'numeric') && length(search_flts) == dim(search_forecasts)[4])) {
		print('ERROR: Search forecast FLTs should be a numeric vector with the length of the fourth dimension of search forecasts!')
		print('Please use is.vector() and length() to check!')
		return(0)
	}
	
	if (!(is.array(search_observations) && is.numeric(search_observations) && length(dim(search_observations)) == 3)) {
		print('ERROR: Search observations should be a 3-dimensional numeric array!')
		print('Please use dim(), is.numeric(), and is.array() to check!')
		return(0)
	}
	
	if (!(is.vector(observation_times, mode = 'numeric') && length(observation_times) == dim(search_observations)[3])) {
		print('ERROR: Search observation times should be a numeric vector with the length of the third dimension of search observations!')
		print('Please use is.vector() and length() to check!')
		return(0)
	}
	
	if (!(is.numeric(observation_id) && length(observation_id) == 1 && observation_id >= 1 && observation_id <= dim(search_observations)[1])) {
		print('Error: Observation id should be an integer within the number of parameters in observations.')
		return(0)
	} else {
		observation_id = observation_id - 1
	}
	
	if (!anyNA(circulars)) {
		if (!(is.vector(circulars, mode = 'numeric') && !anyNA(circulars) && length(circulars) <= dim(test_forecasts)[1])) {
			print('Error: Circulars should be a numeric vector. Its length should not exceed the number of forecast parameters.')
			return(0)
		}
		
		for (val in circulars) {
			if (val > dim(test_forecasts)[1] || val <= 0) {
				print('Error: Circulars value should be positive and not exceed the number of forecast parameters.')
				return(0)
			}
		}
	} else {
		circulars <- vector(mode = 'numeric', length = 0)
	}
	
	AnEn <- .generateAnalogs(
		test_forecasts, dim(test_forecasts), 
		search_forecasts, dim(search_forecasts), 
		search_times, search_flts,
		search_observations, dim(search_observations),
		observation_times, num_members, observation_id, quick,
		circulars, preserve_similarity, verbose)

    # Because similarity matrix was stored in row-major in C++ and the R object will be column-major.
    # So change the order of dimensions to make it conform with other objects.
    #
    if (preserve_similarity) {
        AnEn$similarity <- aperm(AnEn$similarity, length(dim(AnEn$similarity)):1)
    }
	
	return(AnEn)
}
