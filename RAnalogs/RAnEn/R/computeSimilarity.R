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
computeSimilarity <- function(
  test_forecasts, search_forecasts, search_times, search_flts,
  search_observations, observation_times, xs = NA, ys = NA,
  parameter_circulars = NA, parameter_weights = NA) {
  
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
  
  similarity_matrices <- .computeSimilarity(
    test_forecasts, search_forecasts, search_times, search_flts, search_observations, observation_times)
}