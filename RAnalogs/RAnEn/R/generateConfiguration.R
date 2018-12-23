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

#' RAnEn::generateConfiguration
#' 
#' RAnEn::generateConfiguration generates a templated configuration object to
#' be passed to the function \code{\link{generateAnalogs}} instead of passing various
#' parameters one by one. This object can be validated before use by the function
#' \code{\link{validateConfiguration}}.
#' 
#' For configuration with mode 'independentSearch':
#' - mode: 'independentSearch' indicates only searching the forecasts at the current location.
#' - test_forecasts: The forecasts for which anlog ensembles will be generated.
#' - search_forecasts: The forecasts from which analogs are searched.
#' - search_times: Time information for search forecasts.
#' - search_flts: FLT information for search forecasts.
#' - search_observations: The observations from which analog ensemble members are selected.
#' - observation_times: Time information for search observations.
#' - observation_id: The parameter index for which the observation parameter to use. For example,
#' if you have several observation parameters in search observations, this parameter tells the
#' package which observation parameter to use.
#' - num_members: The number of ensemble member to keep in analog ensembles.
#' - circulars: The index of forecast parameters in search forecasts that are circular.
#' - weights: The weight for forecast parameters in search forecasts.
#' - quick: Whether to use quick sort algorithm.
#' - extend_observations: After getting the most similar forecast indices, take the corresponding observations from the search station.
#' - preserve_real_time: Whether to preserve the real time information in returned AnEn.
#' - preserve_similarity: Whether to preserve the similarity matrix in returned AnEn.
#' - preserve_mapping: Whether to preserve the mapping matrix in return AnEn.
#' - verbose: Verbose level. You can find settings for different values in the
#' [C++ documentation verbose](https://weiming-hu.github.io/AnalogsEnsemble/CXX/class_an_en.html#a25984b953516a987e2e9eb23048e5d60).
#' - time_match_mode: This is the method used to computed the mapping matrix between forecast times/flts and observation times.
#' 0 for strict search (return error when matching observation times cannot be found) and 1 for loose search.
#' - max_par_nan: The number of NAN values allowed when computing similarity across different parameters.
#' Set it to NA to allow any number of NAN values.
#' - max_flt_nan: The number of NAN values allowed when computing FLT window averages.
#' Set it to NA to allow any number of NAN values.
#' 
#' For configuration with mode 'extendedSearch': (skipping the aforementioned parameters)
#' - mode: 'extendedSearch' indicates search the forecasts from both the current location and the nearby locations.
#' - test_stations_x: X coordinates of test stations/grid points.
#' - test_stations_y: Y coordinates of test stations/grid points.
#' - search_stations_x: X coordinates of search stations/grid points.
#' - search_stations_y: Y coordinates of search stations/grid points.
#' - max_num_search_stations: The maximum search stations to keep in the similarity matrix. The use of this parameter
#' will only affect the number of search stations to keep in the similarity matrix. This won't affect the correctness
#' of nearby station selection. This parameter is used in case of insufficient memeory. A good rule of thumb would be
#' setting it to num_nearest when num_nearest is used, or when distance is used, setting it to an estimated number that
#' is slightly bigger than the expexted number of stations that will be found within the distance. Again, even if you 
#' set it to a number that is wierdly off, the selected nearby stations will be gauranteed to fulfill your requirement.
#' - num_nearest: The number of nearest neighbors.
#' - distance: The distance to search for nearby stations/grid points.
#' - preserve_search_stations: Whether to preserve the search station list in returned AnEn.
#' 
#' Please find detailed terminology explanation in the
#' [Vocabulary post](https://weiming-hu.github.io/AnalogsEnsemble/2018/10/06/vocabulary.html).
#' If you still can't find helpful information or can't understand the parameters, please open a ticket.
#' 
#' @author Weiming Hu \email{weiming@@psu.edu}
#' 
#' @param mode A character string of either 'independentSearch' or
#' 'extendedSearch'.
#' 
#' @return A configuration list with all the parameters you need to specify.
#' Please don't change the mode of the configuration mode. If you need a configuration
#' with a different mode, you need to call the function again with the desired mode.
#' 
#' @md
#' @export
generateConfiguration <- function(mode) {
  
  available.modes <- c('independentSearch', 'extendedSearch')
  
  if (mode %in% available.modes) {
    # This is a valid mode
    
  } else {
    dists <- adist(mode, available.modes) / nchar(available.modes)
    
    if (length(unique(dists)) == length(dists)) {
      cat("Did you mean", available.modes[order(dists)[1]], "?\n")
    } else {
      cat("Available modes:", available.modes)
    }
    
    stop(paste("Unknown input mode", mode))	
  }
  
  config <- list(
    mode = mode,
    test_forecasts = NULL,
    search_forecasts = NULL, 
    search_times = NULL, 
    search_flts = NULL,
    search_observations = NULL, 
    observation_times = NULL,
    num_members = NULL,
    observation_id = 1,
    circulars = vector(mode = 'numeric', length = 0), 
    weights = vector(mode = 'numeric', length = 0), 
    quick = T,
    extend_observations = F, 
    preserve_real_time = F, 
    preserve_similarity = F,
    preserve_mapping = T, 
    time_match_mode = 1, 
    max_par_nan = 0, 
    max_flt_nan= 0, 
    verbose = 1)
  
  if (mode == 'extendedSearch') {
    config <- c(config, list(
      test_stations_x = NULL,
      test_stations_y = NULL,
      search_stations_x = NULL, 
      search_stations_y = NULL,
      num_nearest = 0,
      distance = 0,
      max_num_search_stations = 0,
      preserve_search_stations = F))
  }
  
  class(config) <- "Configuration"
  return(config)
}
