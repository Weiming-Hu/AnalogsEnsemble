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

##################
#   Deprecated   #
##################
#' RAnEn::rcpp_compute_analogs
#' 
#' RAnEn::rcpp_compute_analogs is deprecated. Please see
#' \code{\link{compute_analogs}}
#' 
#' @import Rcpp BH
#' @importFrom Rcpp evalCpp
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
  cat("WARNING: function name rcpp_compute_analogs is deprecated. Please use generateAnalogs!\n")
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
#' RAnEn::compute_analogs computes Analog Ensembles and return the results
#' in an AnEn class object.
#' 
#' For algorithm details, please see references.
#' 
#' @author Weiming Hu \email{weiming@@psu.edu}
#' 
#' @import Rcpp BH
#' 
#' @importFrom Rcpp evalCpp
#' 
#' @useDynLib RAnEn
#' 
#' @param forecasts A 4-dimensional double array that contains all the 
#' forecast parameter values. The expected structure is \bold{forecasts
#' [parameter, station, day, flt]} where flt stands for forecast lead time.
#' @param observations A 4-dimensional double array that contains all the 
#' observation parameter values. The expected structure is \bold{
#' observations[parameter, station, day, flt]} where flt stands for 
#' forecast lead time.
#' @param circulars An integer vector that specifies which parameter(s) in 
#' the forecasts should be circular. For example, wind direction is a circular
#'  parameter because 0 and 360 are the same directinos.
#' @param weights An double vector that specifies the weight strategy used 
#' while computing the analog ensembles. The number of the weights should be 
#' the same as the number of parameters in the forecasts. By default it sets 
#' a uniform weights for all parameters.
#' @param stations_ID An integer vector that specifies the station ids for 
#' which you want to compute analog ensemble. By default it uses all stations.
#' @param test_ID_start An integer specifies the first day that will be used 
#' for testing.
#' @param test_ID_end An integer specifies the last day that will be used for 
#' testing.
#' @param train_ID_start An integer specifies the first day that will be used 
#' for training.
#' @param train_ID_end An integer specifies the last day that will be used for
#'  training. If rolling is set, train_ID_end will by default set to 
#'  test_ID_start + rolling.
#' @param observation_ID An integer specifies for which parameter you want to
#'  compute analog ensembles. By default, it is set to 1 if observations only
#' have one parameter.
#' @param members_size An integer specifies how many members there should be in 
#' each analog ensemble.
#' @param rolling An integer that is either 0 or negative. 0 indicates that no 
#' rolling strategy will be used; a negative integer indicates the offset of 
#' training days.
#' @param quick A boolean value that specifies the sorting strategy. After 
#' computing the standard deviations, when choosing the most matching past 
#' forecasts, TRUE indicates partial sort and FALSE indicates global sort.
#' @param cores An integer indicating that how many threads you want to create 
#' if OpenMP is supported in your machine.
#' @param search_extension A boolean value that specifies whether to use extended 
#' search space.
#' @param observation_from_extended_station A boolean value that specifies whether
#'  to take observations from the extended stations.
#' @param recompute_sd_for_extended_station A boolean value that specifies whether 
#' to recompute standard deviation for extended stations. This might cause a 
#' significant slow-down of the process.
#' @param verbose An integer indicating the level of output information. 0 means 
#' no output, 1 means little, and 2 means detailed output. By default, it is 1.
#' 
#' @return An AnEn list. Possible list members include analogs, metrics, and 
#' search_stations. The list member analogs is a 5-dimensional array that contains
#' values and indices of ensemble members from the variable observations.
#' The structure returned is \bold{analogs[stations, #' days, flt, members, ...]}
#' where flt stands for forecast lead time. The size of #' the fifth dimension is 3,
#' storing the value [,,,,1], station index [,,,,2], and day index [,,,,3]. The list 
#' member metrics shows the similarity between the test forecast (specified in the element's
#' name) and all available train forecasts. The list member search_stations has the
#' extended search stations ID for the main station specified in element's name.
#' 
#' @examples
#' library(RAnEn)
#'
#' # load the example datasets
#' data(Forecasts)
#' data(Observations)
#' dim(forecasts)
#' dim(observations)
#' 
#' # use the last 2 days for testing
#' test_ID_start      <- 8
#' test_ID_end        <- 10
#' 
#' # use the first 7 days for training
#' train_ID_start      <- 1
#' train_ID_end        <- 7
#' 
#' # other parameters for the function
#' weights             <- c(1,1,1,1)
#' stations_ID         <- c(1,2)
#' members_size        <- 3
#' rolling             <- -2
#' quick               <- TRUE
#' forecasts.circulars <- 4
#' observation_ID      <- 1
#' cores               <- 1
#' 
#' 
#' # compute analogs
#' analogs <- compute_analogs(forecasts,
#'                            observations,
#'                            forecasts.circulars,
#'                            weights = weights,
#'                            stations_ID = stations_ID,
#'                            test_ID_start = test_ID_start,
#'                            test_ID_end = test_ID_end,
#'                            train_ID_start = train_ID_start,
#'                            train_ID_end = train_ID_end,
#'                            observation_ID = observation_ID,
#'                            members_size = members_size,
#'                            rolling = rolling,
#'                            quick = quick,
#'                            cores = cores)
#' 
#' dim(analogs)
#' 
#' 
#' @references 
#' Delle Monache, Luca, et al. "Probabilistic weather prediction with an 
#' analog ensemble." Monthly Weather Review 141.10 (2013): 3498-3516.
#' 
#' Cervone, Guido, et al. "Short-term photovoltaic power forecasting using 
#' Artificial Neural Networks and an Analog Ensemble." Renewable Energy (2017).
#' 
#' Alessandrini, S., et al. "An analog ensemble for short-term probabilistic 
#' solar power forecast." Applied energy 157 (2015): 95-110.
#' 
#' Alessandrini, S., et al. "A novel application of an analog ensemble for 
#' short-term wind power forecasting." Renewable Energy 76 (2015): 768-781.
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
  cat("WARNING: function name compute_analogs is deprecated. Please use generateAnalogs!\n")
  
  # check deprecation
  if (!identical(NA, parameter_ID)) {
    print("WARNING: parameter_ID is deprecated! Use observation_ID instead.")
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
    print("WARNING: there are duplicate(s) in stations_ID. The program will ignore duplicates!")
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
    config <- generateConfiguration('IndependentSearch')
  }
  
  test.forecasts <- forecasts[, , test_ID_start:test_ID_end, , drop = F]
  search.forecasts <- forecasts[, , train_ID_start:train_ID_end, , drop = F]
  
  tmp.search.observations <- ob[, , train_ID_start:train_ID_end, , drop = F]
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
  
  if (rolling < 0) cat('Warning: rolling is not supported.')
  
  if (cores != 0) cat('Warning: cores are automatically detected.')
  
  
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
