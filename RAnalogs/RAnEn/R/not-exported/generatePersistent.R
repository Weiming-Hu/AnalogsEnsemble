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

#' RAnEn::generatePersistence
#' 
#' RAnEn::generatePersistence generates persistent analog ensemble forecasts.
#' For each test forecast, it simply takes the historical observation values
#' that are associated with the historical forecasts that are directly prior
#' to the current forecasts. The number of values taken would be the number of
#' ensemble members desired.
#' 
#' @author Weiming Hu \email{weiming@@psu.edu}
#' 
#' @details 
#' For example, to generate a 3-member persistent analog ensemble for a particular 
#' forecast at a certain grid point, a certain lead time, and on July 25, 2019,
#' the **observations** associated with the historical forecasts on July 22,
#' 24, and 24 will be included in the persistent analogs.
#' 
#' @param config A Configuration object created from \code{\link{generateConfiguration}}.
#' @param forecast.time.interval The forecast time interval in seconds. This is 
#' usually `24 * 60 * 60` because the third dimension of forecasts is usually day.
#' If it is half day, change the value accordingly.
#' @param show.progress Whether to show a progress bar.
#' @param silent Parameter passed to \code{\link{alignObservations}}.
#' 
#' @return An 5-dimension array. The format is exactly the same as the member
#' `analogs` specified in \code{\link{generateAnalogs}}.
#' 
#' @md
generatePersistence <- function(
  config, forecast.time.interval = 86400, show.progress = F, silent = F) {
  # Get config names
  names <- getConfigNames()
  
  # Sanity checks
  if (class(config) != 'Configuration') {
    stop('Please generate the input config using RAnEn::generateConfiguration.')
  }
  
  if (length(unique(config[[names$`_OBS_TIMES`]])) != length(config[[names$`_OBS_TIMES`]])) {
    stop('Observation times in configuration have duplicates!')
  }
  
  if (max(config[[names$`_OBS_TIMES`]]) < max(config[[names$`_TEST_TIMES`]]) + max(config[[names$`_FLTS`]])) {
    stop('The observation times do not cover all test times!')
  }
  
  # Read meta info
  num.grids <- dim(config[[names$`_FCSTS`]])[2]
  num.test.times <- length(config[[names$`_TEST_TIMES`]])
  num.flts <- dim(config[[names$`_FCSTS`]])[4]
  
  # Generate a historical time sequence for each test time. Observations for this historical
  # time sequence will be collected for persistence.
  # 
  time.prev <- sapply(
    config[[names$`_TEST_TIMES`]],
    function(x) {return(x - (0:config[[names$`_NUM_MEMBERS`]]) * forecast.time.interval)})
  
  # Generate the times that will be extracted for persistence
  times.to.extract <- sort(toDateTime(unique(as.vector(time.prev))))
  
  # Align observations
  if (!silent) cat('Aligning observations ...\n')
  obs.align <- alignObservations(
    config[[names$`_OBS`]][config[[names$`_OBS_ID`]], , , drop = F],
    config[[names$`_OBS_TIMES`]], times.to.extract, config[[names$`_FLTS`]],
    silent = silent, show.progress = show.progress)
  
  # Generate mapping from the forecast times to be extracted to observation times
  mapping <- generateTimeMapping(times.to.extract, config[[names$`_FLTS`]], config[[names$`_OBS_TIMES`]])
  
  # Change the dimensions of observations to make it easier for value extraction
  dim(obs.align) <- dim(obs.align)[-1]
  
  # Dimensions become [stations, FLTs, forecast times]
  obs.align <- aperm(obs.align, c(1, 3, 2))
  
  # Initialize memory for persistent analogs
  persistent <- array(NA, dim = c(
    num.grids, num.test.times, num.flts, config[[names$`_NUM_MEMBERS`]], 3))
  
  if (!silent) cat('Generating persistence ...\n')
  if (show.progress) {
    pb <- txtProgressBar(max = num.test.times, style = 3)
    counter <- 0
  }
  
  for (i.test.time in 1:num.test.times) {
    extract.times <- which(config[[names$`_TEST_TIMES`]][i.test.time] == times.to.extract) - 1:config[[names$`_NUM_MEMBERS`]]
    persistent[, i.test.time, , , 1] <- obs.align[, , extract.times]
    persistent[, i.test.time, , , 2] <- rep(1:num.grids, times = num.flts*config$num_members)
    persistent[, i.test.time, , , 3] <- rep(mapping[as.matrix(expand.grid(1:num.flts, extract.times))], each = num.grids)
    
    if (show.progress) {
      counter <- counter + 1
      setTxtProgressBar(pb, counter)
    }
  }
  
  
  if (show.progress) {
    close(pb)
  }
  
  if (!silent) cat('Done (generatePersistence)!\n')
  return(persistent)
}
