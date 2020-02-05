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

generateConfiguration <- function(mode, advanced = F) {
  return(list(mode = mode, advanced = advanced))
}

generateAnalogs <- function(arg, ...) UseMethod("generateAnalogs")

generateAnalogs.list <- function(config, ...) {
  
  check.package('abind')
  if (config$advanced) stop('Advanced configuration is deprecated. Please change the code to use later APIs')
  
  # Copy the configuration
  config.new <- new(Config)
  
  config.new$save_analogs <- T
  config.new$save_analogs_day_index <- T
  config.new$save_obs_time_index_table <- T
  
  # These are the names in the old config to be migrated from
  from.names <- c("num_members", "observation_id", "weights", "quick",
                  "prevent_search_future", "extend_observations",
                  "preserve_mapping", "preserve_search_stations",
                  "preserve_std", "max_par_nan", "max_flt_nan",
                  "operational", "max_num_sims", "FLT_radius", "verbose")
  
  # There are the names in the new config to be written into.
  # These names should match up with the from names
  # 
  to.names <- c("num_analogs", "observation_id", "weights", "quick",
                "prevent_search_future", "extend_observation",
                "save_obs_time_index_table", "save_search_stations_index",
                "save_sds", "max_par_nan", "max_flt_nan", "operation",
                "num_similarity", "flt_radius", "verbose")
  
  stopifnot(length(from.names) == length(to.names))
  
  # Migrate configurations
  for (i in 1:length(from.names)) {
    if (!is.null(config[[from.names[i]]])) {
      config.new[[to.names[i]]] <- config[[from.names[i]]]
    }
  }
  
  # Some names must be specifically migrated
  if (!is.null(config$preserve_similarity)) {
    if (config$preserve_similarity) {
      config.new$save_similarity <- T
      config.new$save_similarity_day_index <- T
      config.new$save_similarity_station_index <- T
    }
  }
  
  if (config$mode == "extendedSearch") {
    config.new$distance <- config$distance
    config.new$num_nearest <- ifelse(
      config$num_nearest > config$max_num_search_stations,
      yes = config$num_nearest,
      no = config$max_num_search_stations)
  }
  
  # Copy forecasts
  fcsts <- generateForecastsTemplate()
  fcsts$Data <- config$forecasts
  fcsts$FLTs <- config$flts
  fcsts$Times <- config$forecast_times
  
  if (length(config$circulars) != 0) {
    fcsts$ParameterCirculars <- config$circulars
  }
  
  if (config$mode == "extendedSearch") {
    if (!is.null(config$forecast_stations_x)) {
      fcsts$Xs <- config$forecast_stations_x
    }
    if (!is.null(config$forecast_stations_y)) {
      fcsts$Ys <- config$forecast_stations_y
    }
  }
  
  # Copy observations
  obs <- generateObservationsTemplate()
  obs$Data <- config$search_observations
  obs$Times <- config$observation_times
  
  # Copy search and test times
  test_times <- config$test_times_compare
  search_times <- config$search_times_compare
  
  # Generate analogs
  if (config$mode == 'independentSearch') {
    AnEn <- generateAnalogs(fcsts, obs, test_times, search_times, config.new, 'IS')
    
    analogs.station.index <- array(
      data = rep(1:dim(AnEn$analogs)[1], times = prod(dim(AnEn$analogs)[2:4])),
      dim = dim(AnEn$analogs))
    
    if (config.new$save_similarity) {
      sims.station.index <- array(
        data = rep(1:dim(AnEn$similarity)[1], times = prod(dim(AnEn$similarity)[2:4])),
        dim = dim(AnEn$similarity))
    }
    
  } else if (config$mode == 'extendedSearch') {
    AnEn <- generateAnalogs(fcsts, obs, test_times, search_times, config.new, 'SSE')
    
    analogs.station.index <- AnEn$similarity_station_index[, , , 1:config.new$num_analogs, drop = F]
    sims.station.index <- AnEn$similarity_station_index
    AnEn$similarity_station_index <- NULL
    
  } else {
    stop("Mode not recognized. You should use RAnEn version 4+")
  }
  
  # Arrange the results to the old format
  AnEn$analogs <- abind::abind(AnEn$analogs, analogs.station.index, AnEn$analogs_time_index, along = 5)
  AnEn$analogs_time_index <- NULL
  
  if (config.new$save_similarity) {
    AnEn$similarity <- abind::abind(AnEn$similarity, sims.station.index, AnEn$similarity_time_index, along = 5)
    AnEn$similarity_time_index <- NULL
  }
  
  return (AnEn)
}
