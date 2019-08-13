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

##################
#   Deprecated   #
##################

#' RAnEn::rcpp_compute_analogs
#' 
#' Deprecated function. Please see \code{\link{generateAnalogs}}.
#' 
#' @import Rcpp BH
#' 
#' @importFrom Rcpp evalCpp
#' 
#' @useDynLib RAnEn
#' 
#' @export
rcpp_compute_analogs <- function(forecasts,
                                 observations,
                                 circulars = vector(mode = 'numeric'),
                                 weights = NA,
                                 stations_ID = NA,
                                 test_ID_start, test_ID_end,
                                 train_ID_start = 1, train_ID_end = NA,
                                 observation_ID = NA,
                                 members_size,
                                 rolling = 0,
                                 quick = T,
                                 cores = 1,
                                 search_extension = F,
                                 xs = NA,
                                 ys = NA,
                                 distance = 0.0,
                                 num_nearest = 0,
                                 observation_from_extended_station = F,
                                 recompute_sd_for_extended_station = F,
                                 output_search_stations = F,
                                 output_metric = F,
                                 verbose = 1,
                                 parameter_ID = NA,
                                 max_par_nan = 0,
                                 max_flt_nan = 0) {
  cat("Warning: Function rcpp_compute_analogs is deprecated. Please use generateAnalogs!\n")
  analogs <- compute_analogs(forecasts = forecasts,
                             observations = observations,
                             circulars = circulars,
                             weights = weights,
                             stations_ID = stations_ID,
                             test_ID_start = test_ID_start,
                             test_ID_end = test_ID_end,
                             train_ID_start = train_ID_start,
                             train_ID_end = train_ID_end,
                             observation_ID = observation_ID,
                             members_size = members_size,
                             rolling = rolling,
                             quick = quick,
                             cores = cores,
                             search_extension = search_extension,
                             xs = xs,
                             ys = ys,
                             distance = distance,
                             num_nearest = num_nearest,
                             observation_from_extended_station = observation_from_extended_station,
                             recompute_sd_for_extended_station = recompute_sd_for_extended_station,
                             output_search_stations = output_search_stations,
                             output_metric = output_metric,
                             verbose = verbose,
                             parameter_ID = parameter_ID,
                             max_par_nan = max_par_nan,
                             max_flt_nan = max_flt_nan) 
  return(analogs)
}

#' RAnEn::compute_analogs
#' 
#' Deprecated function. Please see \code{\link{generateAnalogs}}.
#' 
#' @import Rcpp BH
#' 
#' @importFrom Rcpp evalCpp
#' 
#' @useDynLib RAnEn
#' 
#' @export
compute_analogs <- function(forecasts,
                            observations,
                            circulars = vector(mode = 'numeric'),
                            weights = NA,
                            stations_ID = NA,
                            test_ID_start, test_ID_end,
                            train_ID_start = 1, train_ID_end = NA,
                            observation_ID = NA,
                            members_size,
                            rolling = 0,
                            quick = T,
                            cores = 0,
                            search_extension = F,
                            xs = NA,
                            ys = NA,
                            distance = 0.0,
                            num_nearest = 0,
                            observation_from_extended_station = F,
                            recompute_sd_for_extended_station = F,
                            output_search_stations = F,
                            output_metric = F,
                            verbose = 1,
                            parameter_ID = NA,
                            max_par_nan = 0,
                            max_flt_nan = 0) {
  cat("Warning: Function compute_analogs is deprecated. Please use generateAnalogs!\n")
  
  # check deprecation
  if (!identical(NA, parameter_ID)) {
    cat("Warning: parameter_ID is deprecated! Use observation_ID instead.\n")
    if (!identical(NA, observation_ID)) {
      return(0)
    }
    
    if (!(is.numeric(parameter_ID) &&
          length(parameter_ID) == 1)) {
      observation_ID = parameter_ID
    }
  }
  
  # validate parameters
  if (!(is.array(forecasts) &&
        is.numeric(forecasts) &&
        length(dim(forecasts)) == 4)) {
    cat('ERROR: forecasts should be a 4-dimensional numeric array! Please use dim(), is.numeric(), and is.array() to check!\n')
    return(0)
  }
  
  if (!(is.array(observations) &&
        is.numeric(observations) &&
        length(dim(observations)) == 4)) {
    cat('ERROR: observations should be a 4-dimensional numeric array! Please use dim(), is.numeric(), and is.array() to check!\n')
    return(0)
  }
  
  if(dim(forecasts)[2] != dim(observations)[2] ||
     dim(forecasts)[3] != dim(observations)[3] ||
     dim(forecasts)[4] != dim(observations)[4]) {
    cat('ERROR: the second, thrid, and fourth dimensions of forecasts and observations should be the same.\n')
    return(0)
  }
  
  if (!(is.numeric(circulars) &&
        is.vector(circulars) &&
        length(circulars) <= dim(forecasts)[1])) {
    cat('ERROR: circular should be a numeric vector. The length of the vector should not be larger than the number of parameters in forecasts.\n')
    return(0)
  }
  
  if (identical(NA, weights)) {
    weights <- rep(1, dim(forecasts)[1])
  }
  if (!(is.vector(weights) &&
        is.numeric(weights) &&
        length(weights) == dim(forecasts)[1])) {
    cat('ERROR: weights should be a numeric vector. The length should equal the number of parameters.\n')
    return(0)
  }
  
  if (identical(NA, stations_ID)) {
    stations_ID <- 1:dim(forecasts)[2]
  }
  if (!(is.numeric(stations_ID) &&
        is.vector(stations_ID) &&
        length(stations_ID) <= dim(forecasts)[2])) {
    cat('ERROR: stations_ID should be a numeric vector. The length should not be larger than the number of stations in forecasts.\n')
    return(0)
  }
  if (!all(!duplicated(stations_ID))) {
    cat("Warning: there are duplicate(s) in stations_ID. The program will ignore duplicates!\n")
    stations_ID <- unique(stations_ID)
  }
  
  if (!(is.numeric(test_ID_start) &&
        length(test_ID_start) == 1)) {
    cat("ERROR: test_ID_start should be a numeric value.\n")
    return(0)
  }
  
  if (!(is.numeric(test_ID_end) &&
        length(test_ID_end) == 1)) {
    cat("ERROR: test_ID_end should be a numeric value.\n")
    return(0)
  }
  
  if (!(is.numeric(train_ID_start) &&
        length(train_ID_start) == 1)) {
    cat("ERROR: train_ID_start should be a numeric value.\n")
    return(0)
  }
  
  if (identical(NA, observation_ID) &&
      dim(observations)[1] == 1) {
    observation_ID = 1
  }
  if (!(is.numeric(observation_ID) &&
        length(observation_ID) == 1 &&
        observation_ID <= dim(observations)[1])) {
    cat("ERROR: observation_ID should be a numeric value. It should not be larger than the number of parameters in observations.\n")
    return(0)
  }
  
  if (!(is.numeric(members_size) &&
        length(members_size) == 1)) {
    cat("ERROR: members_size should be a numeric value.\n")
    return(0)
  }
  
  if (identical(NA, train_ID_end)) {
    if (rolling < 0) {
      train_ID_end = test_ID_start + rolling
    }
    if (train_ID_end < 0) {
      cat("ERROR: train_ID_end = test_ID_start + rolling is negative!\n")
      return(0)
    }
  }
  if (!(is.numeric(train_ID_end) &&
        length(train_ID_end) == 1)) {
    cat("ERROR: train_ID_end should be a numeric value.\n")
    return(0)
  }
  
  if (members_size >= (train_ID_end - train_ID_start + 1)) {
    cat("ERROR: members_size is larger than number of train days!\n")
    return(0)
  }
  
  if (!(is.logical(quick) &&
        length(quick) == 1)) {
    cat("ERROR: quick should be a logic value.\n")
    return(0)
  }
  
  if (!(is.numeric(rolling) &&
        length(rolling) == 1 &&
        rolling <= 0)) {
    cat("ERROR: rolling should be a non-positive numeric value.\n")
    return(0)
  }
  
  if (!(is.logical(search_extension) &&
        length(search_extension) == 1)) {
    cat("ERROR: search_extension is a logic value.\n")
    return(0)
  }
  
  if (search_extension) {
    if (identical(NA, xs)) {
      cat("ERROR: search extension needs xs.\n")
      return(0)
    }
    if (is.array(xs) &&
        length(dim(xs)) == 1) {
      xs <- as.numeric(xs)
    }
    if (!(is.numeric(xs) &&
          is.vector(xs) &&
          length(xs) == dim(forecasts)[2])) {
      cat("ERROR: xs should a numeric vector. The length equals to the number of stations in forecasts.\n")
      return(0)
    }
    
    if (identical(NA, ys)) {
      cat("ERROR: search extension needs ys.\n")
      return(0)
    }
    if (is.array(ys) &&
        length(dim(ys)) == 1) {
      ys <- as.numeric(ys)
    }
    if (!(is.numeric(ys) &&
          is.vector(ys) &&
          length(ys) == dim(forecasts)[2])) {
      cat("ERROR: ys should a numeric vector. The length equals to the number of stations in forecasts.\n")
      return(0)
    }
    
    if (identical(0, distance) && identical(0, num_nearest)) {
      cat("ERROR: please set num_nearest and/or distance for search extension.\n")
      return(0)
    }
    
    if (!identical(0, distance)) {
      if (!(is.numeric(distance) &&
            length(distance) == 1 &&
            distance > 0)) {
        cat("ERROR: distance should be a positive numeric value.\n")
        return(0)
      }
    }
    
    if (!identical(0, num_nearest)) {
      if (!(is.numeric(num_nearest) &&
            length(num_nearest) == 1 &&
            num_nearest > 0)) {
        cat("ERROR: num_nearest should be a positive numeric value.\n")
        return(0)
      }
    }
  }
  
  if (!(is.logical(observation_from_extended_station) &&
        length(observation_from_extended_station) == 1)) {
    cat("ERROR: observation_from_extended_station should a logical value.\n")
    return(0)
  }
  
  if (!(is.logical(recompute_sd_for_extended_station) &&
        length(recompute_sd_for_extended_station) == 1)) {
    cat("ERROR: recompute_sd_for_extended_station should a logical value.\n")
    return(0)
  }
  
  if (!(is.logical(output_search_stations) &&
        length(output_search_stations) == 1)) {
    cat("ERROR: output_search_stations should a logical value.\n")
    return(0)
  }
  
  if (!(is.logical(output_metric) &&
        length(output_metric) == 1)) {
    cat("ERROR: output_metric should a logical value.\n")
    return(0)
  }
  
  if (!(is.numeric(verbose) &&
        length(verbose) == 1)) {
    cat("ERROR: verbose should be an integer.\n")
    return(0)
  }
  
  ###################################
  # Code for backward compatibility #
  ###################################
  
  if (search_extension) {
    config <- generateConfiguration('extendedSearch', TRUE)
    
    config$test_stations_x <- xs
    config$test_stations_y <- ys
    config$search_stations_x <- xs
    config$search_stations_y <- ys
    config$num_nearest <- num_nearest
    config$distance <- distance
    config$preserve_search_stations <- output_search_stations
    
    config$max_num_search_stations <- 10
    
  } else {
    config <- generateConfiguration('independentSearch', TRUE)
  }

  test.forecasts <- forecasts
  search.forecasts <- forecasts
  
  test.times <- (1:dim(forecasts)[3])
  search.times <- (1:dim(forecasts)[3])
  
  # Make sure rolling is an integer
  if (rolling == 0) {
    config$operational <- FALSE
    flts <- seq(0, by = 1 / dim(forecasts)[4], length.out = dim(forecasts)[4])
  } else {
    config$operational <- TRUE
    rolling <- floor(rolling)
    flts <- seq(0, by = abs(rolling) / dim(forecasts)[4], length.out = dim(forecasts)[4])
  }
  
  observation.times <- unique(rep(search.times, each = length(flts)) + flts)
  
  mapping <- generateTimeMapping(search.times, flts, observation.times, 1, verbose)
  
  search.observations <- array(dim = c(dim(observations)[1:2], length(observation.times)))
  
  for (i.time in 1:dim(observations)[3]) {
    for (i.flt in 1:dim(observations)[4]) {
      i.obs <- mapping[i.flt, i.time]
      
      for (i.par in 1:dim(observations)[1]) {
        for (i.station in 1:dim(observations)[2]) {
          search.observations[i.par, i.station, i.obs] <-
            observations[i.par, i.station, i.time, i.flt]
        }
      }
      
    }
  }
  
  config$test_forecasts <- test.forecasts
  config$test_times <- test.times
  config$search_forecasts <- search.forecasts
  config$search_times <- search.times
  config$flts <- flts
  config$search_observations <- search.observations
  config$observation_times <- observation.times
  config$observation_id <- observation_ID
  config$num_members <- members_size
  config$circulars <- circulars
  config$weights <- weights
  config$quick <- quick
  config$preserve_similarity <- output_metric
  config$preserve_mapping <- F
  config$preserve_std <- F
  config$verbose <- verbose
  config$max_par_nan = max_par_nan
  config$max_flt_nan = max_flt_nan
  
  config$test_times_compare <- test.times[test_ID_start:test_ID_end]
  config$search_times_compare <- search.times[train_ID_start:train_ID_end]
  
  config <- formatConfiguration(config, config$verbose > 2)
  valid <- validateConfiguration(config)
  if (!valid) return(valid)
  
  if (observation_from_extended_station)
    cat('Warning: observation_from_extended_station is not supported.\n')
  
  if (recompute_sd_for_extended_station)
    cat('Warning: recompute_sd_for_extended_station is not supported.\n')

  if (cores != 0) cat('Warning: cores are automatically detected.\n')
  
  if (config$mode == 'independentSearch') {
    
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
    
  } else if (config$mode == 'extendedSearch') {
    # Create default values
    search_extension <- T
    
  } else {
    stop('Unknown configuration mode!')
  }
  
  AnEn <- generateAnalogs(config)
  
  # Convert index for 3D observations back to 4D observations
  if (verbose >= 3)
    cat("Convert observation index (use the latest API to avoid it and save some time) ...\n")
  
  for (i.flt in 1:dim(AnEn$analogs)[3]) {
    AnEn$analogs[, , i.flt, , 3] <- sapply(AnEn$analogs[, , i.flt, , 3], function(x, cont) {
      if (is.na(x)) return(NA)
      else if (x < 0) return(x)
      else return (which(x == cont))}, cont = mapping[i.flt, ])
  }
  
  if (verbose >= 3) cat("Done (compute_analogs)!\n")
  
  return(AnEn)
}
