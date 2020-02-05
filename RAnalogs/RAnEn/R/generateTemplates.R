generateForecastsTemplate <- function() {
  
  # Get a config
  config <- new(Config)
  
  # Get the name pairs
  pairs <- config$getNames()
  
  # Create the template
  forecast.names <- c(
    pairs$`_DATA`, pairs$`_PAR_NAMES`, pairs$`_CIRCULARS`,
    pairs$`_STATION_NAMES`, pairs$`_XS`, pairs$`_YS`,
    pairs$`_TIMES`, pairs$`_FLTS`)
  
  forecasts <- vector("list", length(forecast.names))
  names(forecasts) <- forecast.names
  
  class(forecasts) <- 'Forecasts'
  return(forecasts)
}

generateObservationsTemplate <- function() {
  
  # Get a config
  config <- new(Config)
  
  # Get the name pairs
  pairs <- config$getNames()
  
  # Create the template
  obs.names <- c(
    pairs$`_DATA`, pairs$`_PAR_NAMES`, pairs$`_STATION_NAMES`,
    pairs$`_XS`, pairs$`_YS`, pairs$`_TIMES`)
  
  observations <- vector("list", length(obs.names))
  names(observations) <- obs.names
  
  class(forecasts) <- 'Observations'
  return(observations)
}

