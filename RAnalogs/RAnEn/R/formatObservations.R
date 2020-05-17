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

#' RAnEn::formatObservations
#' 
#' RAnEn::formatObservations generates the observation list required
#' by analog computation.
#' 
#' @details 
#' The observation list is an R list with members including `ParameterNames`,
#' `Xs`, `Ys`, `Data`, and etc., with a full list accessible
#' [here](https://weiming-hu.github.io/AnalogsEnsemble/2019/01/16/NetCDF-File-Types.html#observations).
#' RAnEn::formatObservations make it easier to convert a data frame into
#' a such list data structure.
#' 
#' I read [this tutorial](https://cran.r-project.org/web/packages/dplyr/vignettes/programming.html)
#' when developing functions with `dplyr`. It is very informative of using variables
#' with `dplyr` functions.
#' 
#' @param df A data frame to be converted to an R list.
#' @param col.par The column name for parameter names.
#' @param col.x The column name for station x coordinates.
#' @param col.y The column name for station y coordinates.
#' @param col.time The column name for times. The column should be POSIXct.
#' @param time.series The times to be extract into observations. This should
#' be a POSIXct vector.
#' @param col.value The column name for data values.
#' @param verbose Whether to print progress messages.
#' @param preview How many entries to preview in progress messages.
#' @param remove.duplicates Whether to remove redundant values associated
#' with the same time. Sometimes, it is possible that, due to equipment mulfunctions,
#' there are multiple measurements at the same time. Idealy this should cleaned prior
#' to this function, but this function is able to keep the first appearance and remove
#' the rest.
#' @param circular.pars A character vector for the circular parameter names.
#' @param col.station.name The column name for station names.
#' @param show.progress Whether to show a progress bar.
#' @param sort.stations Sort station. It can be `Xs`, `Ys`, or `StationNames` if it is set.
#' 
#' @return An R list for observation data.
#' 
#' @examples 
#' \dontrun{
#' # How to download this file? Please see the tutorial
#' # https://github.com/Weiming-Hu/AnalogsEnsemble/blob/master/RAnalogs/examples/demo-5_observation-conversion.Rmd
#' #
#' obs <- read.table('~/Desktop/hourly_44201_2019.csv',
#'                   sep = ',', quote = '"', header = T, stringsAsFactors = F)
#' 
#' # Sample data
#' df <- obs[sample(nrow(obs), floor(nrow(obs) * 0.01)), ]
#' 
#' # Create a POSIXct time
#' df$POSIX <- as.POSIXct(
#'   paste(df$Date.GMT, df$Time.GMT),
#'   format = '%Y-%m-%d %H:%M', tz = 'UTC')
#' 
#' # Create unique station names
#' df$StationName <- paste(
#'   df$State.Name, df$County.Name, df$Site.Num, sep = '-')
#' 
#' # Create a target time series
#' time.series <- seq(
#'   from = as.POSIXct('2019-03-03', tz = 'UTC'),
#'   to = as.POSIXct('2019-06-03', tz = 'UTC'),
#'   by = 'hour')
#' 
#' # Format observations
#' observations <- formatObservations(
#'   df = df, col.par = 'Parameter.Name',
#'   col.x = 'Longitude', col.y = 'Latitude',
#'   col.time = 'POSIX', time.series = time.series,
#'   col.value = 'Sample.Measurement',
#'   circular.pars = 'Ozone',
#'   col.station.name = 'StationName',
#'   show.progress = T)
#' 
#' # Check data
#' i.station = 50
#' plot(observations$Times,
#'      observations$Data[1, i.station, ],
#'      col = 'red', pch = 16)
#' 
#' df.sub <- subset(
#'   df, Latitude == observations$Ys[i.station] &
#'     Longitude == observations$Xs[i.station])
#' df.sub <- df.sub[order(df.sub$POSIX),]
#' lines(df.sub$POSIX, df.sub$Sample.Measurement)
#' 
#' # Write formatted observations to a file
#' writeNetCDF('Observations', observations, '~/Desktop/obs.nc')
#' }
#' 
#' @md

# By specifying this variable, I'm aware when I will be tapping into
# the data.table structure.
#
# Reference:
# https://cran.r-project.org/web/packages/data.table/vignettes/datatable-importing.html
#
.datatable.aware = TRUE

#' @export
formatObservations <- function(
  df, col.par, col.x, col.y, col.time, time.series, col.value,
  verbose = T, preview = 2, remove.duplicates = T,
  circular.pars = NULL, col.station.name = NULL,
  show.progress = F, sort.stations = NULL) {
  
  check.package('dplyr')
  check.package('rlang')

  
  ####################################################################
  #                           Sanity check                           #
  ####################################################################
  
  stopifnot(is.data.frame(df))
  
  for (name in c(col.par, col.x, col.y, col.time, col.value)) {
    if (! name %in% names(df)) {
      stop(paste(name, 'is not a variable in the input data frame.'))
    }
  }
  
  if (!inherits(time.series, 'POSIXct')) {
    stop('time.series should be POSIXct')
  }
  
  if (!inherits(df[[col.time]], 'POSIXct')) {
    stop(paste(col.time, 'should be POSIXct'))
  }
  
  if (any(duplicated(time.series))) {
    stop('time.seires does not allow duplicates.')
  }
  
  if (inherits(df, 'data.table')) {
    is.data.table <- T
    check.package('data.table')
    
  } else {
    is.data.table <- F
    if (verbose) {
      cat('You can speed up the process by making df a data.table\n')
    }
  }
  
  
  ###################################################################
  #                       Preprocessing                             #
  ###################################################################
  
  # Initialize observation list
  if (verbose) {
    cat('Preprocessing the data frame input ...\n')
  }
  
  # Remove extra rows that are outside of the specified time period.
  time.series <- sort(time.series)
  df <- df[which(
    df[[col.time]] >= time.series[1] &
      df[[col.time]] <= time.series[length(time.series)]), ]
  
  # Initialize an empty Observations object
  observations <- RAnEn::generateObservationsTemplate()
  
  # Assign unique parameters
  observations$ParameterNames <- unique(df[[col.par]])
  num.pars <- length(observations$ParameterNames)
  
  if (verbose) {
    cat(num.pars, 'unique parameters extracted:',
        head(observations$ParameterNames, n = preview),
        ifelse(num.pars <= preview, '', '...'), '\n')
  }
  
  if (!is.null(circular.pars)) {
    stopifnot(is.vector(circular.pars, mode = 'character'))
    
    for (name in circular.pars) {
      if (!name %in% observations$ParameterNames) {
        stop(paste(name, 'is not a parameter in the data frame.'))
      }
    }
    
    # Assign circular parameters
    observations$ParameterCirculars <- circular.pars
  }
  
  # Create unique id for stations based on coordinates
  df$..Station.ID.. <-  dplyr::group_indices(
    dplyr::group_by_at(df, .vars = dplyr::vars(col.x, col.y)))
  
  # These are the columns to keep in the unique station data frame
  station.cols <- c(col.x, col.y, "..Station.ID..")
  
  # Add the station name column is it is present
  if (!is.null(col.station.name)) {
    stopifnot(col.station.name %in% names(df))
    station.cols <- c(station.cols, col.station.name)
  }
  
  # Create a data frame with unique station ID
  unique.pts <- dplyr::distinct(
    dplyr::select(df, !!! rlang::syms(station.cols)),
    ..Station.ID.., .keep_all = T)
  
  # Assign station information to Observations object
  if (!is.null(col.station.name)) {
    if (nrow(unique.pts) != length(unique(unique.pts[[col.station.name]]))) {
      stop(paste('Some entries in', col.station.name,
                 'correspond to multiple coordinates.',
                 'You might want to change the variable.'))
    } 
    observations$StationNames <- unique.pts[[col.station.name]]
  }
  
  observations$Xs <- unique.pts[[col.x]]
  observations$Ys <- unique.pts[[col.y]]
  num.stations <- nrow(unique.pts)
  
  if (verbose) {
    cat(num.stations, 'unique points extracted\n')
  }
  
  # Define observation times
  num.times <- length(time.series)
  observations$Times <- time.series
  
  if (verbose) {
    cat(num.times, 'unique times defined:',
        as.character(head(observations$Times, n = preview)),
        ifelse(num.times <= preview, '', '...'), '\n')
  }
  
  # Preallocate array data
  obs.dims <- c(num.pars, num.stations, num.times)
  if (verbose) {
    cat('Creating the observation data array [',
        paste(obs.dims, collapse = ','), '] ...\n')
  }
  
  observations$Data <- array(NA, dim = obs.dims)
  
  # Pre define some variables to reuse during the for loop
  df.template <- data.frame(target = time.series)
  time.val.cols <- c(col.time, col.value)
  
  # Set keys for fast row subset
  if (is.data.table) {
    if (verbose) {
      cat("Setting data table subset keys for fast row subset ...\n")
    }
    
    data.table::setkeyv(df, c(col.par, '..Station.ID..'))
  }
  
  if (verbose) {
    cat("Converting data frame to array ...\n")
  }
  
  if (show.progress) {
    pb <- txtProgressBar(max = num.pars * num.stations, style = 3)
    count <- 0
  }
  
  for (i.par in 1:num.pars) {
    
    if (is.data.table) {
      df.par <- df[.(observations$ParameterNames[i.par])]
    } else {
      selected.rows <- df[[col.par]] == observations$ParameterNames[i.par]
      df.par <- df[selected.rows, ]
    }
    
    for (station.id in unique.pts$..Station.ID..) {
      
      # Which position to write the data
      i.station <- which(unique.pts$..Station.ID.. == station.id)
      
      # Subset the observations to the selected station ID
      if (is.data.table) {
        df.sub <- dplyr::select(
          df.par[.(observations$ParameterNames[i.par], station.id)],
                 !!! rlang::syms(time.val.cols))
        
      } else {
        selected.rows <- df.par$..Station.ID.. == station.id
        df.sub <- dplyr::select(df.par[selected.rows, ],
                                !!! rlang::syms(time.val.cols))
      }
      
      df.sub <- merge(
        x = df.template, y = df.sub,
        by.x = 'target', by.y = col.time,
        sort = T, all.x = T, all.y = F)
      
      if (nrow(df.sub) > nrow(df.template)) {
        # Found duplicated times
        
        if (remove.duplicates) {
          df.sub <- df.sub[!duplicated(df.sub$target), ]
          stopifnot(nrow(df.sub) == nrow(df.template))
          
        } else {
          msg <- paste0(
            'Each time should only have one measurement ',
            'but duplicated time is found for the above station.',
            ' Use remove.duplicates to keep the first value.')
          print(unique.pts[i.station, ])
          stop(msg)
        }
      }
      
      observations$Data[i.par, i.station, ] <- df.sub[[col.value]]
      
      if (show.progress) {
        count <- count + 1
        setTxtProgressBar(pb, count)
      }
    }
  }
  
  if (show.progress) {
    close(pb)
  }
  
  # Sort observations if needed
  if (identical(sort.stations, NULL)) {
    if (verbose) {
      cat("Observation stations are not sorted. Use sort.stations to sort them.\n")
      cat("Make sure station orders are consistent between forecasts and observations.\n")
    }
    
  } else {
    
    if (identical(sort.stations, 'Xs')) {
      if (verbose) {
        cat("Sort observations by Xs ...\n")
      }
      
      index <- order(observations$Xs)
      
    } else if (identical(sort.stations, 'Ys')) {
      if (verbose) {
        cat("Sort observations by Ys ...\n")
      }
      
      index <- order(observations$Ys)
      
    } else if (identical(sort.stations, 'StationNames')) {
      
      if (is.null(observations$StationNames)) {
        warning("Station names are missing. Observations are not sorted.")
        index <- NULL
        
      } else {
        if (verbose) {
          cat("Sort observations by station names ...\n")
        }
        
        index <- order(observations$StationNames)
      }
      
    } else {
      warning("Sorting tag unsupported. Observations are not sorted.")
      index <- NULL
    }
    
    if (!identical(index, NULL)) {
      observations$Xs <- observations$Xs[index]
      observations$Ys <- observations$Ys[index]
      observations$StationNames <- observations$StationNames[index]
      observations$Data <- observations$Data[, index, , drop = F]
    }
    
  }
  
  if (verbose) {
    cat('Done (formatObservations)!\n')
  }
  return(observations)
}
