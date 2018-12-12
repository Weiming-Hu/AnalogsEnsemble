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
                                 parameter_ID = NA) {
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
                             parameter_ID = parameter_ID) 
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
                            parameter_ID = NA) {
  cat("Warning: Function compute_analogs is deprecated. Please use generateAnalogs!\n")
  
  # check deprecation
  if (!identical(NA, parameter_ID)) {
    print("Warning: parameter_ID is deprecated! Use observation_ID instead.")
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
    print('ERROR: forecasts should be a 4-dimensional numeric array! Please use dim(), is.numeric(), and is.array() to check!')
    return(0)
  }
  
  if (!(is.array(observations) &&
        is.numeric(observations) &&
        length(dim(observations)) == 4)) {
    print('ERROR: observations should be a 4-dimensional numeric array! Please use dim(), is.numeric(), and is.array() to check!')
    return(0)
  }
  
  if(dim(forecasts)[2] != dim(observations)[2] ||
     dim(forecasts)[3] != dim(observations)[3] ||
     dim(forecasts)[4] != dim(observations)[4]) {
    print('ERROR: the second, thrid, and fourth dimensions of forecasts and observations should be the same.')
    return(0)
  }
  
  if (!(is.numeric(circulars) &&
        is.vector(circulars) &&
        length(circulars) <= dim(forecasts)[1])) {
    print('ERROR: circular should be a numeric vector. The length of the vector should not be larger than the number of parameters in forecasts.')
    return(0)
  }
  
  if (identical(NA, weights)) {
    weights <- rep(1, dim(forecasts)[1])
  }
  if (!(is.vector(weights) &&
        is.numeric(weights) &&
        length(weights) == dim(forecasts)[1])) {
    print('ERROR: weights should be a numeric vector. The length should equal the number of parameters.')
    return(0)
  }
  
  if (identical(NA, stations_ID)) {
    stations_ID <- 1:dim(forecasts)[2]
  }
  if (!(is.numeric(stations_ID) &&
        is.vector(stations_ID) &&
        length(stations_ID) <= dim(forecasts)[2])) {
    print('ERROR: stations_ID should be a numeric vector. The length should not be larger than the number of stations in forecasts.')
    return(0)
  }
  if (!all(!duplicated(stations_ID))) {
    print("Warning: there are duplicate(s) in stations_ID. The program will ignore duplicates!")
    stations_ID <- unique(stations_ID)
  }
  
  if (!(is.numeric(test_ID_start) &&
        length(test_ID_start) == 1)) {
    print("ERROR: test_ID_start should be a numeric value.")
    return(0)
  }
  
  if (!(is.numeric(test_ID_end) &&
        length(test_ID_end) == 1)) {
    print("ERROR: test_ID_end should be a numeric value.")
    return(0)
  }
  
  if (!(is.numeric(train_ID_start) &&
        length(train_ID_start) == 1)) {
    print("ERROR: train_ID_start should be a numeric value.")
    return(0)
  }
  
  if (identical(NA, observation_ID) &&
      dim(observations)[1] == 1) {
    observation_ID = 1
  }
  if (!(is.numeric(observation_ID) &&
        length(observation_ID) == 1 &&
        observation_ID <= dim(observations)[1])) {
    print("ERROR: observation_ID should be a numeric value. It should not be larger than the number of parameters in observations.")
    return(0)
  }
  
  if (!(is.numeric(members_size) &&
        length(members_size) == 1)) {
    print("ERROR: members_size should be a numeric value.")
    return(0)
  }
  
  if (identical(NA, train_ID_end)) {
    if (rolling < 0) {
      train_ID_end = test_ID_end + rolling
    }
    if (train_ID_end < 0) {
      print("ERROR: train_ID_end = test_ID_end + rolling is negative!")
      return(0)
    }
  }
  if (!(is.numeric(train_ID_end) &&
        length(train_ID_end) == 1)) {
    print("ERROR: train_ID_end should be a numeric value.")
    return(0)
  }
  
  if (members_size >= (train_ID_end - train_ID_start + 1)) {
    print("ERROR: members_size is larger than number of train days!")
    return(0)
  }
  
  if (!(is.logical(quick) &&
        length(quick) == 1)) {
    print("ERROR: quick should be a logic value.")
    return(0)
  }
  
  if (!(is.numeric(rolling) &&
        length(rolling) == 1 &&
        rolling <= 0)) {
    print("ERROR: rolling should be a non-positive numeric value.")
    return(0)
  }
  
  if (!(is.logical(search_extension) &&
        length(search_extension) == 1)) {
    print("ERROR: search_extension is a logic value.")
    return(0)
  }
  
  if (search_extension) {
    if (identical(NA, xs)) {
      print("ERROR: search extension needs xs.")
      return(0)
    }
    if (is.array(xs) &&
        length(dim(xs)) == 1) {
      xs <- as.numeric(xs)
    }
    if (!(is.numeric(xs) &&
          is.vector(xs) &&
          length(xs) == dim(forecasts)[2])) {
      print("ERROR: xs should a numeric vector. The length equals to the number of stations in forecasts.")
      return(0)
    }
    
    if (identical(NA, ys)) {
      print("ERROR: search extension needs ys.")
      return(0)
    }
    if (is.array(ys) &&
        length(dim(ys)) == 1) {
      ys <- as.numeric(ys)
    }
    if (!(is.numeric(ys) &&
          is.vector(ys) &&
          length(ys) == dim(forecasts)[2])) {
      print("ERROR: ys should a numeric vector. The length equals to the number of stations in forecasts.")
      return(0)
    }
    
    if (identical(0, distance) && identical(0, num_nearest)) {
      print("ERROR: please set num_nearest and/or distance for search extension.")
      return(0)
    }
    
    if (!identical(0, distance)) {
      if (!(is.numeric(distance) &&
            length(distance) == 1 &&
            distance > 0)) {
        print("ERROR: distance should be a positive numeric value.")
        return(0)
      }
    }
    
    if (!identical(0, num_nearest)) {
      if (!(is.numeric(num_nearest) &&
            length(num_nearest) == 1 &&
            num_nearest > 0)) {
        print("ERROR: num_nearest should be a positive numeric value.")
        return(0)
      }
    }
  }
  
  if (!(is.logical(observation_from_extended_station) &&
        length(observation_from_extended_station) == 1)) {
    print("ERROR: observation_from_extended_station should a logical value.")
    return(0)
  }
  
  if (!(is.logical(recompute_sd_for_extended_station) &&
        length(recompute_sd_for_extended_station) == 1)) {
    print("ERROR: recompute_sd_for_extended_station should a logical value.")
    return(0)
  }
  
  if (!(is.logical(output_search_stations) &&
        length(output_search_stations) == 1)) {
    print("ERROR: output_search_stations should a logical value.")
    return(0)
  }
  
  if (!(is.logical(output_metric) &&
        length(output_metric) == 1)) {
    print("ERROR: output_metric should a logical value.")
    return(0)
  }
  
  if (!(is.numeric(verbose) &&
        length(verbose) == 1 &&
        verbose %in% c(0, 1, 2))) {
    print("ERROR: verbose should be 0, 1, or 2")
    return(0)
  }
  
  ###################################
  # Code for backward compatibility #
  ###################################
  
  if (search_extension) {
    config <- generateConfiguration('extendedSearch')
    
    config$test_stations_x <- xs[test_ID_start:test_ID_end]
    config$test_stations_y <- ys[test_ID_start:test_ID_end]
    config$search_stations_x <- xs[train_ID_start:train_ID_end]
    config$search_stations_y <- ys[train_ID_start:train_ID_end]
    config$num_nearest <- num_nearest
    config$distance <- distance
    config$preserve_search_stations <- output_search_stations
    
    config$max_num_search_stations <- 10
    
  } else {
    config <- generateConfiguration('independentSearch')
  }

  test.forecasts <- forecasts[, , test_ID_start:test_ID_end, , drop = F]
  search.forecasts <- forecasts[, , train_ID_start:train_ID_end, , drop = F]
  
  tmp.search.observations <- observations[, , train_ID_start:train_ID_end, , drop = F]
  search.observations <- aperm(tmp.search.observations, c(4, 3, 2, 1))
  search.observations <- array(search.observations,
                               dim = c(dim(tmp.search.observations)[3] 
                                       * dim(tmp.search.observations)[4], 
                                       dim(tmp.search.observations)[2],
                                       dim(tmp.search.observations)[1]))
  search.observations <- aperm(search.observations, c(3, 2, 1))
  rm(tmp.search.observations)
  
  search.times <- (1:dim(search.forecasts)[3]) * 100
  search.flts <- 1:dim(search.forecasts)[4]
  observation.times <- rep(search.times, each = length(search.flts)) + search.flts
  
  config$test_forecasts <- test.forecasts
  config$search_forecasts <- search.forecasts
  config$search_times <- search.times
  config$search_flts <- search.flts
  config$search_observations <- search.observations
  config$observation_times <- observation.times
  config$observation_id <- observation_ID
  config$num_members <- members_size
  config$circulars <- circulars
  config$weights <- weights
  config$quick <- quick
  config$preserve_similarity <- output_metric
  config$preserve_mapping <- F
  config$verbose <- verbose
  
  valid <- validateConfiguration(config)
  if (!valid) return(valid)
  
  if (observation_from_extended_station)
    cat('Warning: observation_from_extended_station is not supported.\n')
  
  if (recompute_sd_for_extended_station)
    cat('Warning: recompute_sd_for_extended_station is not supported.\n')
  
  if (rolling < 0) cat('Warning: rolling is not supported.\n')
  
  if (cores != 0) cat('Warning: cores are automatically detected.\n')
  
  
  config$observation_id = config$observation_id - 1
  
  
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
    
    AnEn <- .generateAnalogs(
      config$test_forecasts, dim(config$test_forecasts),
      test_forecasts_station_x, test_forecasts_station_y,
      config$search_forecasts, dim(config$search_forecasts),
      search_forecasts_station_x, search_forecasts_station_y,
      config$search_times, config$search_flts,
      config$search_observations, dim(config$search_observations),
      config$observation_times, config$num_members,
      config$observation_id, config$quick,
      config$circulars, search_extension,
      config$extend_observations,
      config$preserve_real_time,
      config$preserve_similarity, 
      config$preserve_mapping, preserve_search_stations,
      max_num_search_stations, distance,
      num_nearest_stations,
      config$time_match_mode,
      config$max_par_nan,
      config$max_flt_nan,
      config$verbose)
    
  } else if (config$mode == 'extendedSearch') {
    # Create default values
    search_extension <- T
    
    AnEn <- .generateAnalogs(
      config$test_forecasts, dim(config$test_forecasts),
      config$test_stations_x,
      config$test_stations_y,
      config$search_forecasts, dim(config$search_forecasts),
      config$search_stations_x,
      config$search_stations_y,
      config$search_times, config$search_flts,
      config$search_observations, dim(config$search_observations),
      config$observation_times, config$num_members,
      config$observation_id, config$quick,
      config$circulars, search_extension,
      config$extend_observations,
      config$preserve_real_time,
      config$preserve_similarity,
      config$preserve_mapping,
      config$preserve_search_stations,
      config$max_num_search_stations,
      config$distance,
      config$num_nearest,
      config$time_match_mode,
      config$max_par_nan,
      config$max_flt_nan,
      config$verbose)
    
  } else {
    stop('Unknown configuration mode!')
  }
  
  # Because similarity matrix was stored in row-major in C++ and the R object will be column-major.
  # So change the order of dimensions to make it conform with other objects.
  #
  if (config$preserve_similarity) {
    AnEn$similarity <- aperm(AnEn$similarity, length(dim(AnEn$similarity)):1)
  }
  
  if (config$mode == 'extendedSearch') {
    if (config$preserve_search_stations) {
      AnEn$searchStations <- AnEn$searchStations + 1
    }
  }
  
  if (!config$preserve_real_time) {
    AnEn$analogs[, , , , 3] <- AnEn$analogs[, , , , 3, drop = F] %/% dim(config$test_forecasts)[4] + 1
  }
  
  # Convert the station index from C counting to R counting
  AnEn$analogs[, , , , 2] <- AnEn$analogs[, , , , 2, drop = F] + 1
  AnEn$similarity[, , , , 2:3] <- AnEn$similarity[, , , , 2:3, drop = F] + 1
  
  if (config$preserve_mapping) AnEn$mapping <- AnEn$mapping + 1
  
  return(AnEn)
}
