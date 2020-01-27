# "`-''-/").___..--''"`-._
#  (`6_ 6  )   `-.  (     ).`-.__.`)   WE ARE ...
#  (_Y_.)'  ._   )  `._ `. ``-..-'    PENN STATE!
#    _ ..`--'_..-_/  /--'_.' ,'
#  (il),-''  (li),'  ((!.-'
# 
# Author: Weiming Hu <cervone@psu.edu>
#         Geoinformatics and Earth Observation Laboratory (http://geolab.psu.edu)
#         Department of Geography and Institute for CyberScience
#         The Pennsylvania State University
#

#' RAnEn::plotAnalogTimeSeries
#' 
#' RAnEn::plotAnalogTimeSeries plots forecasts and observations
#' for comparison over time.
#' 
#' @details 
#' Sometimes, it is helpful to plot forecasts and observations
#' for a long time series. However, this is always limited
#' by forecast lead times because the lead times are usually 
#' for several days. What if we want to plot a time series that
#' spans longer than that?
#' 
#' This function generates a plot for that purpose. It simply 
#' stacks forecasts from different days and all forecasts lead
#' times. Overlapping lead times will be cut off and earlier
#' lead times will be preferred and kept.
#' 
#' @param start.time A POSIXct for the start time.
#' @param end.time A POSIXct for the end time.
#' @param i.station The station index to visualize. This can
#' be a single scalar so that the same station index for 
#' observations, AnEn, and forecasts is used; or it can be
#' a named vector like this:
#' `i.station = c('obs' = 4, 'anen' = 15, 'fcst' = 10)` or
#' `i.station = c('obs' = 4, 'anen' = 15)`.
#' @param obs.id The observation parameter ID.
#' @param obs.times A vector for observation times. Usually
#' this is `observations$Times`.
#' @param obs.data An array for observation data. Usually
#' this is `observations$Data`.
#' @param anen.times A vector for AnEn times. Usually
#' this is `config$test_time_compares`.
#' @param anen.flts A vector for AnEn flts. Usually
#' this is `config$flts`.
#' @param anen.data An 4-dimensional array for analogs.
#' @param fcst.id The forecast parameter ID.
#' @param fcst.times A vector for forecast times. Usually
#' this is `forecasts$Times`.
#' @param fcst.flts A vector for forecast flts. Usually
#' this is `forecasts$flts`.
#' @param fcst.data An array for forecast data. Usually
#' this is `forecasts$Data`.
#' @param max.flt The cut off point value for both
#' forecasts and AnEn lead times.
#' @param par.name The variable name to be shown on figures.
#' @param return.data Whether to return the plot data
#' so that you can generate your own plots. This will
#' suppress plotting the figure inside this function.
#' 
#' @return If `return.data` is TRUE, a list with two
#' data frames is returned; otherwise, it returns a ggplot
#' object.
#' 
#' @md
#' @export
plotAnalogTimeSeries <- function(
  start.time, end.time, i.station,
  obs.id, obs.times, obs.data,
  anen.times, anen.flts, anen.data,
  fcst.id = NULL, fcst.times = NULL,
  fcst.flts = NULL, fcst.data = NULL,
  max.flt = 82800, par.name = '',
  return.data = F) {
  
  # Sanity checks  
  for (name in c('ggplot2', 'abind', 'dplyr')) {
    if (!requireNamespace(name, quietly = T)) {
      stop(paste('Please install', name, '.'))
    }
  }
  
  stopifnot(inherits(start.time, 'POSIXct'))
  stopifnot(inherits(end.time, 'POSIXct'))
  stopifnot(length(dim(obs.data)) == 3)
  stopifnot(length(dim(anen.data)) == 4)
  stopifnot(obs.id <= dim(obs.data)[1])
  stopifnot(length(obs.id) == 1)
  
  if (inherits(anen.times, 'numeric')) {
    anen.times <- toDateTime(anen.times)
  } else {
    stopifnot(inherits(anen.times, 'POSIXct'))
  }
  
  if (inherits(obs.times, 'numeric')) {
    obs.times <- toDateTime(obs.times)
  } else {
    stopifnot(inherits(obs.times, 'POSIXct'))
  }
  
  if (length(i.station) == 3 || length(i.station) == 2) {
    if (is.null(names(i.station))) {
      stop("i.station should have names 'obs', 'anen', and 'fcst")
    }
    
    if (!all(c('obs', 'anen', 'fcst') %in% names(i.station))) {
      msg <- paste("Names 'obs', 'anen', and 'fcst' are required",
                   "in the i.station vector.")
      stop(msg)
    }
    
    i.obs.station <- i.station['obs']
    i.anen.station <- i.station['anen']
    i.fcst.station <- i.station['fcst']
  } else {
    stopifnot(length(i.station) == 1)
    i.obs.station <- i.anen.station <- i.fcst.station <- i.station
  }
  
  stopifnot(i.obs.station <= dim(obs.data)[2])
  stopifnot(i.anen.station <= dim(anen.data)[1])
  
  if (!is.null(fcst.id)) {
    stopifnot(!is.null(fcst.times))
    stopifnot(!is.null(fcst.flts))
    stopifnot(!is.null(fcst.data))
    
    stopifnot(fcst.id <= dim(fcst.data)[1])
    stopifnot(length(fcst.id) == 1)
    stopifnot(i.fcst.station <= dim(fcst.data)[2])
    
    if (inherits(fcst.times, 'numeric')) {
      fcst.times <- toDateTime(fcst.times)
    } else {
      stopifnot(inherits(fcst.times, 'POSIXct'))
    }
  }
  
  if (!is.null(fcst.data)) {
    stopifnot(length(dim(fcst.data)) == 4)
  }
  
  # Find the subset for observation time
  i.start <- max(which(obs.times <= start.time))
  i.end <- min(which(obs.times >= end.time + max.flt))
  
  # Make sure there is only one time found for start and end
  if (length(i.start) != 1 | length(i.end) != 1) {
    stop('Start and/or end times can not be found in observation times!')
  }
  
  # Subset observation data
  obs <- obs.data[obs.id, i.obs.station, i.start:i.end, drop = F]
  obs <- abind::adrop(obs, drop = 1:2)
  df.obs <- data.frame(
    Time = obs.times[i.start:i.end],
    Value = obs,
    Method = "Observation")
  rm(obs)
  
  # Find the subset for analogs time
  i.start <- which(anen.times == start.time)
  i.end <- which(anen.times == end.time)
  
  # Make sure the unique found
  if (length(i.start) != 1 | length(i.end) != 1) {
    stop('Start and/or end times can not be found in AnEn test times!')
  }
  
  # Subset analog data
  i.flts <- which(anen.flts <= max.flt)
  anen.flts <- anen.flts[i.flts]
  anen <- anen.data[i.anen.station, i.start:i.end, i.flts, , drop = F]
  anen <- abind::adrop(anen, drop = 1)
  
  num.times <- i.end - i.start + 1
  stopifnot(num.times == dim(anen)[1])
  num.flts <- dim(anen)[2]
  num.members <- dim(anen)[3]
  
  df.anen <- data.frame(
    Time = rep(rep(anen.times[i.start:i.end], times = num.flts) +
                 rep(anen.flts, each = num.times),
               times = num.members),
    Value = as.vector(anen))
  
  rm(anen)
  
  # Find the subset for forecast time
  if (!is.null(fcst.data)) {
    i.start <- which(fcst.times == start.time)
    i.end <- which(fcst.times == end.time)
    
    # Make sure the unique found
    if (length(i.start) != 1 | length(i.end) != 1) {
      stop('Start and/or end times can not be found in forecast times from the configuration!')
    }
    
    # Subset forecast data
    i.flts <- which(fcst.flts <= max.flt)
    fcst.flts <- fcst.flts[i.flts]
    fcsts <- fcst.data[fcst.id, i.fcst.station, i.start:i.end, i.flts, drop = F]
    fcsts <- abind::adrop(fcsts, drop = 1:2)
    
    stopifnot(num.times == dim(fcsts)[1])
    num.flts <- dim(fcsts)[2]
    
    df.fcst <- data.frame(
      Time = rep(fcst.times[i.start:i.end], times = num.flts) +
        rep(fcst.flts, each = num.times),
      Value = as.vector(fcsts),
      Method = 'Forecast')
    
    # Combine forecast and observation data frames
    df.c <- rbind(df.obs, df.fcst)
    rm(fcsts, num.flts, df.fcst)
    
  } else {
    df.c <- df.obs
  }
  
  rm(df.obs)
  garbage <- gc(reset = T)
  rm(garbage)
  
  # Create ribbons for AnEn
  
  df.anen.ribbons <- dplyr::group_by(df.anen, Time)
  df.anen.ribbons <- dplyr::summarise(
    df.anen.ribbons,
    min = min(Value, na.rm = T),
    low = quantile(Value, probs = 0.25, na.rm = T),
    median = median(Value, na.rm = T),
    high = quantile(Value, probs = 0.75, na.rm = T),
    max = max(Value, na.rm = T))
  
  # A fast way to remove infinite numbers
  df.anen.ribbons <- do.call(
    data.frame, lapply(
      df.anen.ribbons, function(x) {
        replace(x, is.infinite(x), NA)}))
  
  # Remove rows with NA
  df.anen.ribbons <- df.anen.ribbons[complete.cases(df.anen.ribbons), ]
  
  df.c <- rbind(df.c, data.frame(
    Time = df.anen.ribbons$Time,
    Value = df.anen.ribbons$median,
    Method = 'AnEn'))
  
  if (return.data) {
    return(list(AnEn.Ensemble = df.anen.ribbons,
                Time.Series = df.c))
  } else {
    ggplot2::ggplot() +
      ggplot2::theme_minimal() +
      ggplot2::geom_ribbon(
        data = df.anen.ribbons, fill = 'lightgrey',
        mapping = ggplot2::aes(x = Time, ymin = min, ymax = max)) +
      ggplot2::geom_ribbon(
        data = df.anen.ribbons, fill = 'lightblue',
        mapping = ggplot2::aes(x = Time, ymin = low, ymax = high)) +
      ggplot2::geom_line(
        data = df.c, size = 1, mapping = ggplot2::aes(
          x = Time, y = Value, color = Method)) +
      ggplot2::labs(x = 'Time', y = par.name) +
      ggplot2::scale_color_brewer(palette = 'Dark2') +
      ggplot2::theme(legend.position = 'bottom')
  }
}
