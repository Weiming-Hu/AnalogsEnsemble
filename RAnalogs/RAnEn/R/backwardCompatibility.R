#' RAnEn::generateConfiguration
#' 
#' @export
generateConfiguration <- function(mode, advanced = F) {
  return(list(mode = mode, advanced = advanced))
}

#' RAnEn::generateAnalogs
#' 
#' The generic method definition
#' 
#' @export
generateAnalogs <- function(arg, ...) UseMethod("generateAnalogs")

#' RAnEn::generateAnalogs
#' 
#' For backward compatibility
#' 
#' @export
generateAnalogs.list <- function(config, ...) {
  
  if (config$advanced) stop('Please change the code to use later APIs')
  
  # Copy the configuration
  config.new <- new(Config)
  
  config.new$save_analogs <- T
  config.new$save_analogs_day_index <- T
  config.new$save_obs_time_index_table <- T
  
  if (!is.null(config$num_members)) {
    config.new$num_analogs <- config$num_members
  }
  if (!is.null(config$observation_id)) {
    config.new$observation_id <- config$observation_id
  }
  if (!is.null(config$weights)) {
    config.new$weights <- config$weights
  }
  if (!is.null(config$quick)) {
    config.new$quick <- config$quick
  }
  if (!is.null(config$prevent_search_future)) {
    config.new$prevent_search_future <- config$prevent_search_future
  }
  if (!is.null(config$extend_observations)) {
    config.new$extend_observation <- config$extend_observations
  }
  if (!is.null(config$preserve_similarity)) {
    if (config$preserve_similarity) {
      config.new$save_similarity <- T
      config.new$save_similarity_day_index <- T
      config.new$save_similarity_station_index <- T
    }
  }
  if (!is.null(config$preserve_mapping)) {
    config.new$save_obs_time_index_table <- config$preserve_mapping
  }
  if (!is.null(config$preserve_search_stations)) {
    config.new$save_search_stations_index <- config$preserve_search_stations
  }
  if (!is.null(config$preserve_std)) {
    config.new$save_sds <- config$preserve_std
  }
  if (!is.null(config$max_par_nan)) {
    config.new$max_par_nan <- config$max_par_nan
  }
  if (!is.null(config$max_flt_nan)) {
    config.new$max_flt_nan <- config$max_flt_nan
  }
  if (!is.null(config$operational)) {
    config.new$operation <- config$operational
  }
  if (!is.null(config$max_num_sims)) {
    config.new$num_similarity <- config$max_num_sims
  }
  if (!is.null(config$FLT_radius)) {
    config.new$flt_radius <- config$FLT_radius
  }
  if (!is.null(config$verbose)) {
    config.new$verbose <- config$verbose
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
  fcsts$FLTs <- config$flts
  if (length(config$circulars) != 0) {
    fcsts$ParameterCirculars <- config$circulars
  }
  fcsts$Data <- config$forecasts
  fcsts$Times <- config$forecast_times
  
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
  } else if (config$mode == 'extendedSearch') {
    AnEn <- generateAnalogs(fcsts, obs, test_times, search_times, config.new, 'SSE')
  } else {
    stop("Mode not recognized. You should use RAnEn version 4+")
  }
  
  return (AnEn)
}
