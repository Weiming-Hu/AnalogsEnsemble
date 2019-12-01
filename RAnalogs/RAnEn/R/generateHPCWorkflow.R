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

#' RAnEn::generateHPCWorkflow
#' 
#' RAnEn::generateHPCWorkflow can generate all configuration
#' files for an HPC workflow. If your Analog Ensemble
#' generation requires supercomputers, this is a very helpful
#' function to generate the configuration files for each
#' computational node.
#' 
#' @details 
#' If forecast and observations are stored in multiple
#' files, it is assumed that they are splitted by time.
#' 
#' @param test.start A POSIXct for the start test time
#' @param test.end A POSIXct for the end test time
#' @param search.start A POSIXct for the start search time
#' @param search.end A POSIXct for the end search time
#' @param fcst.files The forecast file(s)
#' @param obs.files The observation file(s)
#' @param num.members The number of members in each analog
#' @param output.folder The output folder path
#' @param batch.stations How many stations to process in each node
#' @param batch.times The number of seconds included in each node.
#' This is used to parallel along time. For example, you have 3
#' test days and you want to process each day separately, you can
#' set `batch.times` to `24 * 60 * 60`.
#' @param obs.id The observation id
#' @param weights The weights for each forecast variable
#' @param verbose Verbose level. Please see
#' [this doc](https://weiming-hu.github.io/AnalogsEnsemble/CXX/class_an_en.html#a25984b953516a987e2e9eb23048e5d60)
#' @param max.par.nan Maximum number of NAN in parameters.
#' @param max.flt.nan Maximum number of NAN in FLTs.
#' @param max.num.sims How many similarity metrics to keep
#' in the file.
#' @param overwrite Whether to overwrite existing configuration files.
#' 
#' @examples 
#' \dontrun{
#' test.start <- as.POSIXct('2018-08-09', tz = 'UTC')
#' test.end <- as.POSIXct('2018-08-19', tz = 'UTC')
#' search.start <- as.POSIXct('2017-01-20', tz = 'UTC')
#' search.end <- as.POSIXct('2017-12-15', tz = 'UTC')
#' 
#' fcst.files <- list.files(
#'   path = '/glade/u/home/wuh20/scratch/data/AnEn/forecasts',
#'   pattern = paste('2017|2018', collapse = ''),
#'   full.names = T)
#' obs.files <- list.files(
#'   path = '/glade/u/home/wuh20/scratch/data/AnEn/analysis',
#'   pattern = paste('2017|2018', collapse = ''),
#'   full.names = T)
#' 
#' max.flt <- 302400
#' output.folder <- '.'
#' num.members <- 20
#' 
#' batch.stations <- 1000
#' batch.times <- 24 * 3600 * 4
#' 
#' generateHPCWorkflow(
#'   test.start = test.start,
#'   test.end = test.end,
#'   search.start = search.start,
#'   search.end = search.end,
#'   fcst.files = fcst.files,
#'   obs.files = obs.files,
#'   num.members = num.members,
#'   output.folder = output.folder,
#'   batch.stations = batch.stations,
#'   batch.times = batch.times,
#'   overwrite = T)
#' }
#' 
#' @md
#' @export
generateHPCWorkflow <- function(
  test.start, test.end, search.start, search.end,
  fcst.files, obs.files, num.members, output.folder,
  batch.stations, batch.times,
  obs.id = 1, weights = NULL, verbose = 3,
  max.par.nan = 1, max.flt.nan = 1,
  max.num.sims = 2 * num.members, overwrite = F) {
  
  # Check the types of input times
  if (!all(sapply(c(
    test.start, test.end,
    search.start, search.end), function (x) {
      return(inherits(x, 'POSIXct'))}))) {
    stop('Start and end times for test and search should be POSIXct.')
  }
  
  # Combine the meta information from NetCDF file(s)
  fcst.meta <- combineMeta(fcst.files, verbose = verbose)
  obs.meta <- combineMeta(obs.files, verbose = verbose)
  
  for (meta.name in c('fcst.meta', 'obs.meta')) {
    meta <- get(meta.name)
    
    # Define the columns to check for uniqueness
    col.names <- c('num.pars', 'num.stations')
    if ('num.flts' %in% names(meta)) {
      col.names <- c(col.names, 'num.flts')
    }
    
    # Check whether the column has only one unique value
    for (col.name in col.names) {
      if (length(unique(meta[[col.name]])) != 1) {
        msg <- paste(col.name, 'in', meta.name,
                     'is not consistent.')
        stop(msg)
      }
    }
  }
  rm(meta)
  
  # Stations in forecasts and observations
  # should be the same
  # 
  stopifnot(identical(
    fcst.meta$num.stations[1],
    obs.meta$num.stations[1]))
  
  # Check weights
  if (is.null(weights)) {
    weights <- rep(1, fcst.meta$num.pars[1])
  } else {
    stopifnot(length(weights) == fcst.meta$num.pars[1])
  }
  
  # Subset meta tables
  fcst.meta.search <- subset(
    fcst.meta, end.time >= search.start &
      start.time <= search.end)
  fcst.meta.test <- subset(
    fcst.meta, end.time >= test.start &
      start.time <= test.end)
  obs.meta.search <- subset(
    obs.meta, end.time >= search.start &
      start.time <= search.end + max.flt)
  rm(fcst.meta, obs.meta)
  
  summary <- distributeConfig(
    test.start = test.start, test.end = test.end,
    search.start = search.start, search.end = search.end,
    fcst.meta.test = fcst.meta.test,
    fcst.meta.search = fcst.meta.search,
    obs.meta.search = obs.meta.search,
    batch.stations = batch.stations,
    batch.times = batch.times,
    num.members = num.members,
    output.folder = output.folder, 
    obs.id = obs.id, weights = weights,
    verbose = verbose, max.par.nan = max.par.nan,
    max.flt.nan = max.flt.nan,
    max.num.sims = max.num.sims,
    overwrite = overwrite)
  
  cat('\n\n********* SUMMARY *********\n',
      'HPC workflow has been generated.\n\n',
      'There are ', summary$num.stations * summary$num.times,
      ' tasks for analogGenerator.\n',
      'The computation for each task includes:\n',
      ' - ', summary$num.stations, ' station(s)\n',
      ' - ', summary$num.times, ' time(s)\n',
      '*********   END   *********\n', sep = '')
}

getIndex <- function(
  file.name, value, var.name = 'Times') {
  
  if (!requireNamespace('ncdf4', quietly = T)) {
    stop('Please install ncdf4')
  }
  
  nc <- ncdf4::nc_open(as.character(file.name))
  var <- ncdf4::ncvar_get(nc, var.name)
  ncdf4::nc_close(nc)
  
  return(which(var == value))
}

addFiles <- function(
  meta.table, file.type, station.start,
  station.end, time.start, time.end) {
  
  # Determine the argument names
  if (file.type == 'test forecast') {
    arg.file <- 'test-forecast-nc'
    arg.start <- 'test-start'
    arg.count <- 'test-count'
  } else if (file.type == 'search forecast') {
    arg.file <- 'search-forecast-nc'
    arg.start <- 'search-start'
    arg.count <- 'search-count'
  } else if (file.type == 'search observation') {
    arg.file <- 'observation-nc'
    arg.start <- 'obs-start'
    arg.count <- 'obs-count'
  } else {
    stop('Invalid file type.')
  }
  
  rows.to.append <- paste('# Add', file.type, 'files')
  
  for (i.row in 1:nrow(meta.table)) {
    
    starts <- c(1, station.start, 1)
    counts <- c(
      meta.table$num.pars[i.row],
      station.end - station.start + 1,
      meta.table$num.times[i.row])
    
    if (grepl('forecast', file.type)) {
      starts <- c(starts, 1)
      counts <- c(counts, meta.table$num.flts[i.row])
    }
    
    if (meta.table$start.time[i.row] <= time.start &
        meta.table$end.time[i.row] >= time.start) {
      starts[3] <- getIndex(
        file.name = meta.table$file.name[i.row],
        value = time.start)
      counts[3] <- counts[3] - starts[3] + 1
    }
    
    if (meta.table$start.time[i.row] <= time.end &
        meta.table$end.time[i.row] >= time.end) {
      end.index <- getIndex(
        file.name = meta.table$file.name[i.row],
        value = time.end)
      counts[3] <- end.index - starts[3] + 1
    }
    
    rows.to.append <- c(
      rows.to.append,
      paste(arg.file, '=',
            meta.table$file.name[i.row]),
      paste(arg.start, '=', starts - 1),
      paste(arg.count, '=', counts))
  }
  
  rows.to.append <- c(rows.to.append, '')
  
  return(rows.to.append)
}

distributeConfig <- function(
  test.start, test.end, search.start, search.end,
  fcst.meta.test, fcst.meta.search, obs.meta.search,
  batch.stations, batch.times, num.members, output.folder, 
  obs.id, weights, verbose, max.par.nan, max.flt.nan,
  max.num.sims, overwrite) {
  
  if (!requireNamespace('stringr', quietly = T)) {
    stop('Please install stringr.')
  }
  
  if (!is.numeric(verbose)) {
    stop('verbose should be a numeric.')
  }
  
  # Define how to batch stations and times
  chunks.station <- seq(
    from = 1, by = batch.stations,
    to = fcst.meta.search$num.stations[1])
  chunks.time <- seq(
    from = test.start, to = test.end,
    by = batch.times)
  
  for (i.chunk.station in 1:length(chunks.station)) {
    for (i.chunk.time in 1:length(chunks.time)) {
      
      # Create the file suffix for this iteration
      file.suffix <- paste0(
        'station-',
        stringr::str_pad(
          string = i.chunk.station, pad = 0, 
          width = nchar(length(chunks.station))),
        '_time-', stringr::str_pad(
          string = i.chunk.time, pad = 0,
          width = nchar(length(chunks.time))))
      
      # Create the file name for the configuration file
      file.cfg <- paste0(
        output.folder, '/analogGenerator_', file.suffix, '.cfg')
      
      if (file.exists(file.cfg) & !overwrite) {
        msg <- paste(file.cfg, 'exists. Use overwrite = T.')
        stop(msg)
      }
      
      station.chunk.start <- chunks.station[i.chunk.station]
      station.chunk.end <- station.chunk.start + batch.stations - 1
      if (station.chunk.end > fcst.meta.search$num.stations[1]) {
        station.chunk.end <- fcst.meta.search$num.stations[1]
      }
      
      time.chunk.start <- chunks.time[i.chunk.time]
      time.chunk.end <- chunks.time[i.chunk.time] + batch.times
      if (time.chunk.end > test.end) {
        time.chunk.end <- test.end
      }
      
      rows.to.write <- c(
        '# Configuration file for analogGenerator',
        '# Generated by RAnEn::generateHPCWorkflow',
        paste('# Time of generation', Sys.time()), '#',
        paste('# Global test time : [',
              test.start, ',', test.end, ']'), 
        paste('# Global search time : [',
              search.start, ',', search.end, ']'), '#',
        paste('# Current station subset', i.chunk.station,
              ': [', station.chunk.start, ',',
              station.chunk.end, ']'),
        paste('# Current test time subset', i.chunk.time,
              ': [', time.chunk.start, ',',
              time.chunk.end, ']'), '#', '',
        paste('analog-nc', '=', paste0(
          output.folder, '/analogs_', file.suffix, '.nc')),
        paste('similarity-nc', '=', paste0(
          output.folder, '/sims_', file.suffix, '.nc')),
        paste('observation-id', '=', obs.id - 1),
        paste('members', '=', num.members),
        paste('max-par-nan', '=', max.par.nan),
        paste('max-flt-nan', '=', max.flt.nan),
        paste('max-num-sims', '=', max.num.sims),
        paste('verbose', '=', verbose),
        paste('weights =', weights),
        'test-along = 2',
        'search-along = 2',
        'obs-along = 2', '',
        addFiles(meta.table = fcst.meta.test,
                 file.type = 'test forecast',
                 station.start = station.chunk.start,
                 station.end = station.chunk.end,
                 time.start = time.chunk.start,
                 time.end = time.chunk.end),
        addFiles(meta.table = fcst.meta.search,
                 file.type = 'search forecast',
                 station.start = station.chunk.start,
                 station.end = station.chunk.end,
                 time.start = search.start,
                 time.end = search.end),
        addFiles(meta.table = obs.meta.search,
                 file.type = 'search observation',
                 station.start = station.chunk.start,
                 station.end = station.chunk.end,
                 time.start = search.start,
                 time.end = search.end))
      
      # Write to a cfg file
      if (verbose >= 3) {
        cat('Writing', file.cfg, '\n')
      }
      con <- file(file.cfg, 'w+')
      writeLines(rows.to.write, con = con)
      close(con)
    }
  }
  
  return(list(num.times = length(chunks.time),
              num.stations = length(chunks.station)))
}

combineMeta <- function(nc.files, verbose) {
  if (!requireNamespace('ncdf4', quietly = F)) {
    stop('Please install ncdf4.')
  }
  
  var.names <- c('ParameterNames', 'Xs', 'Ys', 'Times')
  
  df <- data.frame()
  
  for (file in nc.files) {
    if (verbose >= 3) {
      cat('Reading meta information from', file, '\n')
    }
    
    nc <- ncdf4::nc_open(file)
    
    # Check all variable names are available
    names.intersect <- intersect(names(nc$var), var.names)
    names.not.found <- setdiff(var.names, names.intersect)
    if (length(names.not.found) != 0) {
      msg <- paste(paste(names.not.found, collapse = ', '),
                   'not found in', file)
      stop(msg)
    }
    
    time.range <- range(ncdf4::ncvar_get(nc, 'Times'), na.rm = T)
    start.time <- as.POSIXct(
      time.range[1], origin = '1970-01-01', tz = 'UTC')
    end.time <- as.POSIXct(
      time.range[2], origin = '1970-01-01', tz = 'UTC')
    
    df.single <- data.frame(
      file.name = file,
      start.time = start.time,
      end.time = end.time,
      num.pars = nc$var$Data$size[1],
      num.stations = nc$var$Data$size[2],
      num.times = nc$var$Data$size[3])
    
    if ('FLTs' %in% names(nc$var)) {
      df.single$num.flts <- nc$var$Data$size[4]
    }
    
    ncdf4::nc_close(nc)
    
    df <- rbind(df, df.single)
  }
  
  if (verbose >= 3) {
    cat('Meta information has been combined\n')
  }
  return(df)
}
