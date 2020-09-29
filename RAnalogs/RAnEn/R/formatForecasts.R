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

#' RAnEn::formatForecasts
#' 
#' RAnEn::formatForecasts generates the AnEn Forecasts list required
#' by analog computation.
#' 
#' @details 
#' The AnEn Forecasts is an R list with members including `ParameterNames`,
#' `Xs`, `Ys`, `Data`, and etc., with a full list accessible
#' [here](https://weiming-hu.github.io/AnalogsEnsemble/2019/01/16/NetCDF-File-Types.html#forecasts).
#' RAnEn::formatForecasts make it easier to convert a data frame into
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
#' @param col.time The column name for forecast cycle time. The column should be POSIXct.
#' @param col.lead.time The column name for forecast lead time. The column should be numeric
#' for the number of seconds from the forecast cycle time.
#' @param time.series The forecast cycle times to be extract into AnEn Forecasts. This should
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
#' @return An R list for AnEn Forecasts.
#' 
#' @seealso RAnEn::formatObservations
#' @md
#' @export
formatForecasts <- function(
  df, col.par, col.x, col.y, col.time, col.lead.time, time.series, col.value,
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
  
  if (!is.numeric(df[[col.lead.time]])) {
    stop(paste(col.lead.time, 'should be numeric (the number of seconds from the cycle time)'))
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
  
  # Initialize Forecasts list
  if (verbose) {
    cat('Preprocessing the data frame input ...\n')
  }
  
  # Remove extra rows that are outside of the specified time period.
  time.series <- sort(time.series)
  df <- df[which(
    df[[col.time]] >= time.series[1] &
      df[[col.time]] <= time.series[length(time.series)]), ]
  
  # Initialize an empty Forecasts object
  forecasts <- RAnEn::generateForecastsTemplate()
  
  # Format the basic elements
  ret <- formatBasic(forecasts, df, col.par, circular.pars, col.x, col.y,
                     col.station.name, time.series)
  
  unique.pts <- ret$unique.pts
  forecasts <- ret$obj
  df <- ret$df
  rm(ret)
  
  # Format lead times
  forecasts$FLTs <- sort(unique(df[[col.lead.time]]))
  
  num.pars <- length(forecasts$ParameterNames)
  num.times <- length(time.series)
  num.lead.times <- length(forecasts$FLTs)
  num.stations <- nrow(unique.pts)
  
  if (verbose) {
    cat(num.pars, ' unique parameters extracted: ',
        paste(head(forecasts$ParameterNames, n = preview), collapse = ', '),
        ifelse(num.pars <= preview, '', ' ...'), '\n',
        num.stations, ' unique stations extracted\n',
        num.times, ' unique cycle times defined: ',
        paste(as.character(head(forecasts$Times, n = preview)), collapse = ', '),
        ifelse(num.times <= preview, '', ' ...'), '\n',
        num.lead.times, ' unique lead times defined: ',
        paste(as.character(head(forecasts$FLTs, n = preview)), collapse = ', '),
        ifelse(num.lead.times <= preview, '', ' ...'), '\n', sep = '')
  }
  
  # Preallocate array data
  fcst.dims <- c(num.pars, num.stations, num.times, num.lead.times)
  if (verbose) {
    cat('Creating the forecasts data array [',
        paste(fcst.dims, collapse = ','), '] ...\n')
  }
  
  forecasts$Data <- array(NA, dim = fcst.dims)
  
  # Pre define some variables to reuse during the for loop
  df.template <- data.frame(target = forecasts$FLTs)
  lead.time.val.cols <- c(col.lead.time, col.value)
  
  # Set keys for fast row subset
  if (is.data.table) {
    if (verbose) {
      cat("Setting data table subset keys for fast row subset ...\n")
    }
    
    data.table::setkeyv(df, c(col.par, col.time, '..Station.ID..'))
  }
  
  if (verbose) {
    cat("Converting data frame to array ...\n")
  }
  
  if (show.progress) {
    pb <- txtProgressBar(max = num.pars * num.times, style = 3)
    count <- 0
  }
  
  for (i.par in 1:num.pars) {
    
    # Subset the data frame to the current parameter
    if (is.data.table) {
      df.par <- df[.(forecasts$ParameterNames[i.par])]
    } else {
      selected.rows <- df[[col.par]] == forecasts$ParameterNames[i.par]
      df.par <- df[selected.rows, ]
    }
    
    for (i.time in 1:num.times) {
      
      # Subset the data frame to the current cycle time
      if (is.data.table) {
        df.par.time <- df.par[.(forecasts$ParameterNames[i.par], forecasts$Times[i.time])]
      } else {
        selected.rows <- df.par[[col.time]] == forecasts$Times[i.time]
        df.par.time <- df.par[selected.rows, ]
      }
      
      for (station.id in unique.pts$..Station.ID..) {
        
        # Which position to write the data
        i.station <- which(unique.pts$..Station.ID.. == station.id)
        
        # Subset the forecasts to the selected station ID
        if (is.data.table) {
          df.sub <- dplyr::select(
            df.par.time[.(forecasts$ParameterNames[i.par], forecasts$Times[i.time], station.id)],
            !!! rlang::syms(lead.time.val.cols))
          
        } else {
          selected.rows <- df.par.time$..Station.ID.. == station.id
          df.sub <- dplyr::select(df.par.time[selected.rows, ],
                                  !!! rlang::syms(lead.time.val.cols))
        }
        
        df.sub <- merge(
          x = df.template, y = df.sub,
          by.x = 'target', by.y = col.lead.time,
          sort = T, all.x = T, all.y = F)
        
        if (nrow(df.sub) > nrow(df.template)) {
          # Found duplicated times
          
          if (remove.duplicates) {
            df.sub <- df.sub[!duplicated(df.sub$target), ]
            stopifnot(nrow(df.sub) == nrow(df.template))
            
          } else {
            msg <- paste0(
              'Each lead time should only have one measurement ',
              'but duplicated lead time is found for the above station.',
              ' Use remove.duplicates to keep the first value.')
            print(unique.pts[i.station, ])
            stop(msg)
          }
        }
        
        forecasts$Data[i.par, i.station, i.time, ] <- df.sub[[col.value]]
      }
      
      if (show.progress) {
        count <- count + 1
        setTxtProgressBar(pb, count)
      }
    }
  }
  
  if (show.progress) {
    close(pb)
  }
  
  forecasts <- sortStations(forecasts, sort.stations, verbose)
  
  if (verbose) {
    cat('Done (formatForecasts)!\n')
  }
  
  return(forecasts)
}

# By specifying this variable, I'm aware when I will be tapping into
# the data.table structure.
#
# Reference:
# https://cran.r-project.org/web/packages/data.table/vignettes/datatable-importing.html
#
.datatable.aware = TRUE
