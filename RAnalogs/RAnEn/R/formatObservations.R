formatObservations <- function(
  df, col.par, col.x, col.y, col.time, time.series, col.value,
  verbose = T, preview = 2, remove.duplicates = T,
  circular.pars = NA, col.station.name = NA,
  show.progress = F) {
  
  # Sanity check
  stopifnot(is.data.frame(df))
  for (name in c(col.par, col.x, col.y, col.time, col.value)) {
    if (! name %in% names(df)) {
      stop(paste(name, 'is not a variable in the input data frame.'))
    }
  }
  
  if (!inherits(time.series, 'POSIXct')) {
    stop('time.series should be POSIXct')
  }
  
  if (any(duplicated(time.series))) {
    stop('time.seires does not allow duplicates.')
  }
  
  # Initialize observation list
  if (verbose) {
    cat('Start formatting the observation list ...\n')
  }
  
  observations <- list(
    ParameterNames = NA,
    Xs = NA,
    Ys = NA,
    Times = NA,
    Data = NA)
  
  # Assign unique parameters
  observations$ParameterNames <- unique(df[[col.par]])
  num.pars <- length(observations$ParameterNames)
  
  if (verbose) {
    cat(num.pars, 'unique parameters extracted:',
        head(observations$ParameterNames, n = preview),
        ifelse(num.pars <= preview, '', '...'), '\n')
  }
  
  if (!identical(NA, circular.pars)) {
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
  df$Station.ID <- df %>%
    dplyr::group_by_at(.vars = vars(col.x, col.x)) %>%
    dplyr::group_indices()
  
  # Extract the unique points
  cols <- c(col.x, col.y, 'Station.ID')
  if (!identical(col.station.name, NA)) {
    stopifnot(col.station.name %in% names(df))
    cols <- c(cols, col.station.name)
  }
  
  unique.pts <- df[, cols] %>%
    distinct(Station.ID, .keep_all = T)
  
  # Assign unique stations
  if (!identical(col.station.name, NA)) {
    if (nrow(unique.pts) != length(unique(unique.pts[, col.station.name]))) {
      stop(paste('Some entries in', col.station.name,
                 'correspond to multiple coordinates.',
                 'You might want to change the variable.'))
    } 
    observations$StationNames <- unique.pts[, col.station.name]
  }
  
  observations$Xs <- unique.pts$Longitude
  observations$Ys <- unique.pts$Latitude
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
  observations$Data <- array(
    NA, dim = c(num.pars, num.stations, num.times))
  
  if (verbose) {
    cat('Observation data array created [',
        paste(dim(observations$Data), collapse = ','),
        ']\nAssigning data values ...\n')
  }
  
  df.template <- data.frame(target = time.series)
  
  if (show.progress) {
    pb <- txtProgressBar(max = num.pars * num.stations, style = 3)
    count <- 0
  }
  
  for (i.par in 1:num.pars) {
    for (station.id in unique.pts$Station.ID) {
      
      # Which position to write the data
      i.station <- which(unique.pts$Station.ID == station.id)
      
      # Subset the observations to the selected station ID
      df.sub <- df[which(df[[col.par]] == observations$ParameterNames[i.par]), ]
      df.sub <- df.sub[which(df.sub$Station.ID == station.id),
                       c(col.time, col.value)]
      df.sub <- merge(x = df.template, y = df.sub,
                      by.x = 'target', by.y = col.time,
                      sort = T, all.x = T)
      
      if (nrow(df.sub) > nrow(df.template)) {
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
  
  if (verbose) {
    cat('Done (formatObservations)!\n')
  }
  return(observations)
}


library(RAnEn)

obs <- read.table('~/Desktop/hourly_44201_2019.csv',
                  sep = ',', quote = '"', header = T, stringsAsFactors = F)

df <- obs[sample(nrow(obs), floor(nrow(obs) * 0.01)), ]

df$POSIX <- as.POSIXct(
  paste(df$Date.GMT, df$Time.GMT),
  format = '%Y-%m-%d %H:%M', tz = 'UTC')

time.series <- seq(
  from = as.POSIXct('2019-03-03', tz = 'UTC'),
  to = as.POSIXct('2019-06-03', tz = 'UTC'),
  by = 'hour')

observations <- formatObservations(
  df = df, col.par = 'Parameter.Name',
  col.x = 'Longitude', col.y = 'Latitude',
  col.time = 'POSIX', time.series = time.series,
  col.value = 'Sample.Measurement',
  circular.pars = 'Ozone',
  col.station.name = 'Site.Num',
  show.progress = T)


i.station = 50
plot(observations$Times,
     observations$Data[1, i.station, ],
     col = 'red', pch = 16)

df.sub <- subset(
  df, Latitude == observations$Ys[i.station] &
    Longitude == observations$Xs[i.station])
df.sub <- df.sub[order(df.sub$POSIX),]
lines(df.sub$POSIX, df.sub$Sample.Measurement)

library(RAnEn)
writeNetCDF('Observations', observations, '~/Desktop/obs.nc')


# Add
# - ParameterWeights
# - ParameterCirculars
# - StationNames
# 